
#include "GeneticAlgorithm.hpp"

#include "demo/utilities/math/RandomNumberGenerator.hpp"
#include "demo/utilities/ErrorHandler.hpp"
#include "demo/utilities/TraceLogger.hpp"

#include "demo/constants.hpp"

#include <iomanip> // <= std::fixed / setprecision
#include <ctime> // <= time(NULL)
#include <algorithm> // <= std::find_if

GeneticAlgorithm::GeneticAlgorithm()
{
	t_RNG::setSeed(time(NULL));
}

void	GeneticAlgorithm::initialise(const t_def& def)
{
	if (def.totalGenomes == 0)
    	D_THROW(std::invalid_argument,
				"received invalid number of genomes"
    			<< ", input=" << def.totalGenomes);

	if (!def.topology.isValid())
    	D_THROW(std::invalid_argument, "received invalid topology");

	_genomes.resize(def.totalGenomes);

	_neuralNetworkTopology = def.topology;

	generateRandomPopulation();
}

void	GeneticAlgorithm::generateRandomPopulation()
{
	if (_genomes.empty())
		D_THROW(std::runtime_error, "not initialised");

	// set the genomes and their neural network

	_neuralNetworks.reserve(_genomes.size());

	for (auto& genome : _genomes)
	{
		genome.id = _currentId++;

		genome.weights.resize(_neuralNetworkTopology.getTotalWeights());

		for (float& weight : genome.weights)
			weight = t_RNG::getRangedValue(-1.0f, 1.0f);


		_neuralNetworks.push_back(NeuralNetwork(_neuralNetworkTopology));
		_neuralNetworks.back().setWeights(genome.weights);
	}
}

bool	GeneticAlgorithm::breedPopulation()
{
	if (_genomes.empty())
		D_THROW(std::runtime_error, "not initialised");

    t_genomes bestGenomes;
	getBestGenomes(8, bestGenomes);

	const auto&	latestBestGenome = bestGenomes[0];

	bool	smarterGeneration = (latestBestGenome.fitness > _bestGenome.fitness);
	bool	stallingGeneration = (latestBestGenome.fitness == _bestGenome.fitness);

	D_MYLOG((smarterGeneration ? "++" : (stallingGeneration ? "==" : "--"))
			<< " generation=" << _currentGeneration
			<< std::fixed
			<< std::setprecision(1)
			<< ", fitness:"
			<< " [best=" << _bestGenome.fitness << "]"
			<< " [latest=" << latestBestGenome.fitness << "]"
			<< ", diff=" << (smarterGeneration ? "+" : "")
			<< (latestBestGenome.fitness - _bestGenome.fitness));

	if (smarterGeneration)
		_bestGenome = latestBestGenome;

	t_genomes	offsprings;
	offsprings.reserve(_genomes.size());

	// elitism: keep the current best

	if (_bestGenome.fitness > 0.0f)
		offsprings.push_back(_bestGenome);

	// crossover: breed best genomes

	typedef std::pair<unsigned int, unsigned int>	t_parentPair;
	std::vector<t_parentPair>	parentGenomes;

	for (unsigned int ii = 0; ii < bestGenomes.size(); ++ii)
		for (unsigned int jj = ii + 1; jj < bestGenomes.size(); ++jj)
			parentGenomes.push_back(std::make_pair(ii, jj));

	for (const auto& parentPair : parentGenomes)
	{
		// stop here if the offsprings container is already full
		if (offsprings.size() >= _genomes.size())
			break;

		const auto&	parentA = bestGenomes[parentPair.first];
		const auto&	parentB = bestGenomes[parentPair.second];

		t_genome	offspringA, offspringB;

		crossBreed(parentA, parentB, offspringA, offspringB);

		offspringA.id = _currentId++;
		mutate(offspringA);
		offsprings.push_back(offspringA);

		// stop here if the offsprings container is already full
		if (offsprings.size() >= _genomes.size())
			break;

		offspringB.id = _currentId++;
		mutate(offspringB);
		offsprings.push_back(offspringB);
	}

	// diversity: add random genomes

	// if there is any space left: add some random genome.
	int remainingOffsprings = static_cast<int>(_genomes.size() - offsprings.size());

	for (int ii = 0; ii < remainingOffsprings; ++ii)
	{
		offsprings.push_back(t_genome());
		auto&	newGenome = offsprings.back();

		newGenome.id = _currentId++;

		newGenome.weights.resize(_neuralNetworkTopology.getTotalWeights());
		for (float& weight : newGenome.weights)
			weight = t_RNG::getRangedValue(-1.0f, 1.0f);
	}

	_genomes = offsprings; // TODO: "std::move" instead of a hard copy?

	for (unsigned int ii = 0; ii < _genomes.size(); ++ii)
		_neuralNetworks[ii].setWeights(_genomes[ii].weights);

	++_currentGeneration;

	return smarterGeneration;
}

void	GeneticAlgorithm::getBestGenomes(unsigned int totalAsked, t_genomes& output) const
{
	output.clear();

	if (_genomes.empty())
		return;

	// realistic total outputed genomes
	if (totalAsked > _genomes.size())
		totalAsked = _genomes.size();

	typedef std::pair<float, unsigned int>	t_sortPair;
	std::vector<t_sortPair>	sortedGenomes;

	sortedGenomes.reserve(_genomes.size());
	for (unsigned int ii = 0; ii < _genomes.size(); ++ii)
		sortedGenomes.push_back(std::make_pair(_genomes[ii].fitness, ii));

	// sort by fitness: the higher the better
    std::sort(sortedGenomes.begin(), sortedGenomes.end(), [](t_sortPair a, t_sortPair b) {
        return a.first > b.first;
    });

	output.reserve(totalAsked);
	for (unsigned int ii = 0; ii < totalAsked; ++ii)
		output.push_back(_genomes[sortedGenomes[ii].second]);
}

void	GeneticAlgorithm::crossBreed(const t_genome& parentA, const t_genome& parentB, t_genome& offspringA, t_genome& offspringB)
{
	const unsigned int totalWeights = _neuralNetworkTopology.getTotalWeights();
	const unsigned int crossoverIndex = t_RNG::getRangedValue(0.0f, 0.99f) * totalWeights;

	offspringA.weights.resize(totalWeights);
	offspringB.weights.resize(totalWeights);

	for (unsigned int ii = 0; ii < crossoverIndex; ++ii)
	{
		offspringA.weights[ii] = parentA.weights[ii];
		offspringB.weights[ii] = parentB.weights[ii];
	}

	for (unsigned int ii = crossoverIndex; ii < totalWeights; ++ii)
	{
		offspringA.weights[ii] = parentB.weights[ii];
		offspringB.weights[ii] = parentA.weights[ii];
	}
}

void	GeneticAlgorithm::mutate(t_genome& genome) const
{
	const float	mutationMaxChance = 0.2f;
	const float	mutationMaxEffect = 0.2f;

	for (float& weight : genome.weights)
		if (t_RNG::getNormalisedValue() < mutationMaxChance)
			weight += t_RNG::getRangedValue(-mutationMaxEffect, mutationMaxEffect);
}

//

const GeneticAlgorithm::t_NeuralNetworks&	GeneticAlgorithm::getNeuralNetworks() const
{
	return _neuralNetworks;
}

const GeneticAlgorithm::t_genomes&	GeneticAlgorithm::getGenomes() const
{
	return _genomes;
}

const GeneticAlgorithm::t_genome&	GeneticAlgorithm::getBestGenome() const
{
	return _bestGenome;
}

unsigned int	GeneticAlgorithm::getGenerationNumber() const
{
	return _currentGeneration;
}

//

void	GeneticAlgorithm::rateGenome(unsigned int index, float fitness)
{
	if (_genomes.empty())
		D_THROW(std::runtime_error, "not initialised");

	_genomes.at(index).fitness = fitness;
}
