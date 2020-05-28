
#include "GeneticAlgorithm.hpp"

#include "demo/utilities/math/RandomNumberGenerator.hpp"
#include "demo/utilities/ErrorHandler.hpp"
#include "demo/utilities/TraceLogger.hpp"

#include <iomanip> // <= std::fixed / setprecision
// #include <ctime> // <= time(NULL)
#include <algorithm> // <= std::find_if
#include <chrono>

void GeneticAlgorithm::initialise(const t_def& def)
{
    if (def.totalGenomes == 0)
        D_THROW(std::invalid_argument,
                "received invalid number of genomes"
                << ", input=" << def.totalGenomes);

    if (!def.topology.isValid())
        D_THROW(std::invalid_argument, "received invalid topology");

    //

    auto currTime = std::chrono::high_resolution_clock::now();
    auto seed = currTime.time_since_epoch().count();
    t_RNG::setSeed(static_cast<unsigned int>(seed));

    //

    _genomes.resize(def.totalGenomes);

    _neuralNetworkTopology = def.topology;

    generateRandomPopulation();
}

bool GeneticAlgorithm::breedPopulation()
{
    if (_genomes.empty())
        D_THROW(std::runtime_error, "not initialised");

    t_genomes bestGenomes;
    getBestGenomes(bestGenomes);

    const auto& latestBestGenome = bestGenomes.front();

    bool smarterGeneration = (latestBestGenome.fitness > _bestGenome.fitness);
    bool stallingGeneration = (latestBestGenome.fitness == _bestGenome.fitness);

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

    t_genomes offsprings;
    offsprings.reserve(_genomes.size());

    // elitism: keep the current best

    if (_bestGenome.fitness > 0.0f)
        offsprings.push_back(_bestGenome);

    // crossover: breed best genomes

    typedef std::pair<unsigned int, unsigned int> t_parentPair;
    std::vector<t_parentPair> parentsPairsGenomes;

    // build all the possible "parents" pairs
    for (unsigned int ii = 0; ii < bestGenomes.size(); ++ii)
        for (unsigned int jj = ii + 1; jj < bestGenomes.size(); ++jj)
            parentsPairsGenomes.push_back(std::make_pair(ii, jj));

    // sort the possible "parents" pair by summed fitness
    auto cmpFunc = [&bestGenomes](const t_parentPair& a, const t_parentPair& b) {
        float fitnessPairA = bestGenomes[a.first].fitness + bestGenomes[a.second].fitness;
        float fitnessPairB = bestGenomes[b.first].fitness + bestGenomes[b.second].fitness;
        return (fitnessPairA > fitnessPairB); // <= the higher the better
    };
    std::sort(parentsPairsGenomes.begin(), parentsPairsGenomes.end(), cmpFunc);

    int maxOffspring = int(_genomes.size() * 0.9f);
    for (const auto& parentPair : parentsPairsGenomes)
    {
        // stop here if the offsprings container is already full
        if (offsprings.size() >= _genomes.size())
            break;

        // stop here if the max amount of children is reched
        if (maxOffspring-- <= 0)
            break;

        const auto& parentGenomeA = bestGenomes[parentPair.first];
        const auto& parentGenomeB = bestGenomes[parentPair.second];

        t_genome newOffspring;

        reproduce(parentGenomeA, parentGenomeB, newOffspring);

        newOffspring.id = _currentId++;
        mutate(newOffspring);
        offsprings.push_back(newOffspring);
    }

    // diversity: add random genomes

    // if there is any space left: add some random genome.
    int remainingOffsprings = int(_genomes.size() - offsprings.size());

    for (int ii = 0; ii < remainingOffsprings; ++ii)
    {
        offsprings.push_back(t_genome());
        auto& newGenome = offsprings.back();

        newGenome.id = _currentId++;

        newGenome.weights.resize(_neuralNetworkTopology.getTotalWeights());
        for (float& weight : newGenome.weights)
            weight = t_RNG::getRangedValue(-1.0f, 1.0f);
    }

    _genomes = std::move(offsprings);

    for (unsigned int ii = 0; ii < _genomes.size(); ++ii)
        _neuralNetworks[ii].setWeights(_genomes[ii].weights);

    ++_currentGeneration;

    return smarterGeneration;
}

void GeneticAlgorithm::generateRandomPopulation()
{
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

void GeneticAlgorithm::getBestGenomes(t_genomes& output) const
{
    output.clear();

    struct t_sortPair
    {
        float           fitness;
        unsigned int    index;
    };
    std::vector<t_sortPair> sortedGenomes;

    sortedGenomes.reserve(_genomes.size());
    for (unsigned int ii = 0; ii < _genomes.size(); ++ii)
        sortedGenomes.push_back({ _genomes[ii].fitness, ii });

    // sort by fitness
    std::sort(sortedGenomes.begin(), sortedGenomes.end(), [](t_sortPair a, t_sortPair b) {
        return a.fitness > b.fitness; // the higher the better
    });

    output.reserve(_genomes.size());
    for (const auto& sortedGenome : sortedGenomes)
        output.push_back(_genomes[sortedGenome.index]);
}

void GeneticAlgorithm::reproduce(const t_genome& parentA,
                                    const t_genome& parentB,
                                    t_genome& offspring) const
{
    offspring.weights.resize(_neuralNetworkTopology.getTotalWeights());

    // 50/50 chances for both parents
    int chancesForParentA = 50;

    // 70/30 chances for the fittest parent
    if (parentA.fitness > parentB.fitness)
        chancesForParentA = 70;
    else if (parentA.fitness < parentB.fitness)
        chancesForParentA = 30;

    for (unsigned int ii = 0; ii < offspring.weights.size(); ++ii)
    {
        if (t_RNG::getRangedValue(0, 100) < chancesForParentA)
            offspring.weights[ii] = parentA.weights[ii];
        else
            offspring.weights[ii] = parentB.weights[ii];
    }
}

void GeneticAlgorithm::mutate(t_genome& genome) const
{
    const float mutationMaxChance = 0.1f;
    const float mutationMaxEffect = 0.2f;

    for (float& weight : genome.weights)
        if (t_RNG::getNormalisedValue() < mutationMaxChance)
            weight += t_RNG::getRangedValue(-mutationMaxEffect, mutationMaxEffect);
}

//

const GeneticAlgorithm::t_NeuralNetworks& GeneticAlgorithm::getNeuralNetworks() const
{
    return _neuralNetworks;
}

const GeneticAlgorithm::t_genomes& GeneticAlgorithm::getGenomes() const
{
    return _genomes;
}

const GeneticAlgorithm::t_genome& GeneticAlgorithm::getBestGenome() const
{
    return _bestGenome;
}

unsigned int GeneticAlgorithm::getGenerationNumber() const
{
    return _currentGeneration;
}

//

void GeneticAlgorithm::rateGenome(unsigned int index, float fitness)
{
    if (_genomes.empty())
        D_THROW(std::runtime_error, "not initialised");

    _genomes.at(index).fitness = fitness;
}
