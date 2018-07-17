

#include "GeneticAlgorithm.hpp"

#include "Simulation/AI/Utility/RandomNumberGenerator.hpp"

#include "Utility/TraceLogger.hpp"

#include "constants.hpp"

#include <ctime> // <= time(NULL)
#include <algorithm> // <= std::find_if

GeneticAlgorithm::GeneticAlgorithm()
{
	t_RNG::setSeed(time(NULL));

	m_genomes.resize(D_CARS_NUMBER_TOTAL);
}

void	GeneticAlgorithm::init(NeuralNetworkTopology& neuralNetworkTopology)
{
	m_pNeuralNetworkTopology = &neuralNetworkTopology;
	generateRandomPopulation();
}

void	GeneticAlgorithm::generateRandomPopulation()
{
	// set the genomes and their neural network

	m_NeuralNetworks.reserve(D_CARS_NUMBER_TOTAL);

	for (auto&& genome : m_genomes)
	{
		genome.id = m_currentId++;

		genome.weights.resize(m_pNeuralNetworkTopology->getTotalWeights());

		for (float& weight : genome.weights)
			weight = t_RNG::getRangedValue(-1.0f, 1.0f);


		m_NeuralNetworks.push_back(NeuralNetwork(*m_pNeuralNetworkTopology));
		m_NeuralNetworks.back().setWeights(genome.weights);
	}
}

bool	GeneticAlgorithm::breedPopulation()
{
	if (m_genomes.empty())
		return false;

    t_genomes	bestGenomes;
	getBestGenomes(8, bestGenomes);

	auto&&	latestBestGenome = bestGenomes[0];

	bool	smarterGeneration = (latestBestGenome.fitness > m_bestGenome.fitness);

	D_MYLOG((smarterGeneration ? "++" : "--")
			<< " generation=" << m_currentGeneration
			<< std::fixed
			<< std::setprecision(1)
			<< ", diff=" << (latestBestGenome.fitness - m_bestGenome.fitness)
			<< ", fitness:"
			<< " [best=" << m_bestGenome.fitness << "]"
			<< " [latest=" << latestBestGenome.fitness << "]");

	if (smarterGeneration)
		m_bestGenome = latestBestGenome;

	t_genomes	offsprings;
	offsprings.reserve(D_CARS_NUMBER_TOTAL);

	// elitism: keep the current best

	offsprings.push_back(m_bestGenome);

	// crossover: breed best genomes

	typedef std::pair<unsigned int, unsigned int>	t_parentPair;
	std::vector<t_parentPair>	parentGenomes;

	for (unsigned int ii = 0; ii < bestGenomes.size(); ++ii)
		for (unsigned int jj = ii + 1; jj < bestGenomes.size(); ++jj)
			parentGenomes.push_back(std::make_pair(ii, jj));

	for (auto&& parentPair : parentGenomes)
	{
		// stop here if the offsprings container is already full
		if (offsprings.size() >= D_CARS_NUMBER_TOTAL)
			break;

		auto&&	parentA = bestGenomes[parentPair.first];
		auto&&	parentB = bestGenomes[parentPair.second];

		t_genome	offspringA, offspringB;

		crossBreed(parentA, parentB, offspringA, offspringB);

		offspringA.id = m_currentId++;
		mutate(offspringA);
		offsprings.push_back(offspringA);

		// stop here if the offsprings container is already full
		if (offsprings.size() >= D_CARS_NUMBER_TOTAL)
			break;

		offspringB.id = m_currentId++;
		mutate(offspringB);
		offsprings.push_back(offspringB);
	}

	// diversity: add random genomes

	// if there is any space left: add some random genome.
	int remainingOffsprings = static_cast<int>(m_genomes.size() - offsprings.size());

	for (int ii = 0; ii < remainingOffsprings; ++ii)
	{
		offsprings.push_back(t_genome());
		auto&	newGenome = offsprings.back();

		newGenome.id = m_currentId++;

		newGenome.weights.resize(m_pNeuralNetworkTopology->getTotalWeights());
		for (float& weight : newGenome.weights)
			weight = t_RNG::getRangedValue(-1.0f, 1.0f);
	}

	m_genomes = offsprings; // TODO: "std::move" instead of a hard copy?

	for (unsigned int ii = 0; ii < m_genomes.size(); ++ii)
		m_NeuralNetworks[ii].setWeights(m_genomes[ii].weights);

	++m_currentGeneration;

	return smarterGeneration;
}

void	GeneticAlgorithm::getBestGenomes(unsigned int totalAsked, t_genomes& output) const
{
	output.clear();

	if (m_genomes.empty())
		return;

	// realistic total outputed genomes
	if (totalAsked > m_genomes.size())
		totalAsked = m_genomes.size();

	typedef std::pair<float, unsigned int>	t_sortPair;
	std::vector<t_sortPair>	sortedGenomes;

	sortedGenomes.reserve(totalAsked);
	for (unsigned int ii = 0; ii < m_genomes.size(); ++ii)
		sortedGenomes.push_back(std::make_pair(m_genomes[ii].fitness, ii));

	// sort by fitness: the higher the better
    std::sort(sortedGenomes.begin(), sortedGenomes.end(), [](t_sortPair a, t_sortPair b) {
        return a.first > b.first;
    });

	output.reserve(totalAsked);
	for (unsigned int ii = 0; ii < totalAsked; ++ii)
		output.push_back(m_genomes[sortedGenomes[ii].second]);
}

void	GeneticAlgorithm::crossBreed(const t_genome& parentA, const t_genome& parentB, t_genome& offspringA, t_genome& offspringB)
{
	const unsigned int totalWeights = m_pNeuralNetworkTopology->getTotalWeights();
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
