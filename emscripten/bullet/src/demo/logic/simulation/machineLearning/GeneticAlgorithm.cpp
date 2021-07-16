
#include "GeneticAlgorithm.hpp"

#include "demo/utilities/math/RandomNumberGenerator.hpp"
#include "demo/utilities/ErrorHandler.hpp"
#include "demo/utilities/TraceLogger.hpp"

#include <iomanip> // <= std::fixed / setprecision
#include <algorithm> // <= std::sort

void GeneticAlgorithm::initialise(const Definition& def)
{
    if (def.totalGenomes == 0)
        D_THROW(std::invalid_argument,
                "received invalid number of genomes"
                << ", input=" << def.totalGenomes);

    if (!def.topology.isValid())
        D_THROW(std::invalid_argument, "received invalid topology");

    //

    _neuralNetworkTopology = def.topology;

    // set the genomes and their neural network

    _genomes.resize(def.totalGenomes);

    unsigned int totalElites = def.totalGenomes * 0.1f; // 10%
    if (totalElites < 5)
        totalElites = 5;
    _bestGenomes.resize(totalElites);

    _neuralNetworks.reserve(def.totalGenomes); // pre-allocate

    RNG::ensureRandomSeed();

    for (auto& genome : _genomes)
    {
        genome.weights.resize(_neuralNetworkTopology.getTotalWeights());

        for (float& weight : genome.weights)
            weight = RNG::getRangedValue(-1.0f, 1.0f);

        auto newNeuralNet = std::make_shared<NeuralNetwork>(_neuralNetworkTopology);
        newNeuralNet->setWeights(genome.weights);
        _neuralNetworks.push_back(newNeuralNet);
    }
}

bool GeneticAlgorithm::breedPopulation()
{
    if (_genomes.empty())
        D_THROW(std::runtime_error, "not initialised");

    Genomes latestBestGenomes;
    _getBestGenomes(latestBestGenomes);

    const auto& latestBestGenome = latestBestGenomes.front();

    const auto& bestGenome = _bestGenomes[0];
    bool isSmarterGeneration = (latestBestGenome.fitness > bestGenome.fitness);
    bool isStallingGeneration = (latestBestGenome.fitness == bestGenome.fitness);

    D_MYLOG((isSmarterGeneration ? "++" : (isStallingGeneration ? "==" : "--"))
            << " generation=" << _currentGeneration
            << std::fixed
            << std::setprecision(1)
            << ", fitness:"
            << " [best=" << bestGenome.fitness << "]"
            << " [latest=" << latestBestGenome.fitness << "]"
            << ", diff=" << (isSmarterGeneration ? "+" : "")
            << (latestBestGenome.fitness - bestGenome.fitness));

    // refresh the _bestGenomes internal array
    for (unsigned int ii = 0; ii < _bestGenomes.size(); ++ii)
        if (_bestGenomes[ii].fitness < latestBestGenomes[ii].fitness)
            _bestGenomes[ii] = latestBestGenomes[ii];

    Genomes offsprings;
    offsprings.reserve(_genomes.size()); // pre-allocate

    { // elitism: keep the current best

        for (unsigned int ii = 0; ii < _bestGenomes.size(); ++ii)
            if (_bestGenomes[ii].fitness > 0.0f)
            {
                // copy, realloc of the weights
                offsprings.push_back(_bestGenomes[ii]);
            }

    } // elitism: keep the current best

    { // crossover: breed best genomes

        struct ParentPair
        {
            unsigned int parentA;
            unsigned int parentB;
        };

        std::vector<ParentPair> parentsPairsGenomes;

        // build all the possible "parents" pairs
        for (unsigned int ii = 0; ii < latestBestGenomes.size(); ++ii)
            for (unsigned int jj = ii + 1; jj < latestBestGenomes.size(); ++jj)
                parentsPairsGenomes.push_back({ii, jj});

        // sort the possible "parents" pair by summed fitness
        auto cmpFunc = [&latestBestGenomes](const ParentPair& a, const ParentPair& b)
        {
            float fitnessPairA = (
                latestBestGenomes[a.parentA].fitness +
                latestBestGenomes[a.parentB].fitness
            );
            float fitnessPairB = (
                latestBestGenomes[b.parentA].fitness +
                latestBestGenomes[b.parentB].fitness
            );
            return (fitnessPairA > fitnessPairB); // <= the higher the better
        };
        std::sort(parentsPairsGenomes.begin(), parentsPairsGenomes.end(), cmpFunc);

        int maxOffspring = int(_genomes.size() * 0.9f);
        for (const auto& parentPair : parentsPairsGenomes)
        {
            // stop here if the offsprings container is already full
            if (offsprings.size() >= _genomes.size())
                break;

            // stop here if the max amount of children is reached
            if (maxOffspring-- <= 0)
                break;

            const auto& parentGenomeA = latestBestGenomes[parentPair.parentA];
            const auto& parentGenomeB = latestBestGenomes[parentPair.parentB];

            Genome newOffspring;

            _reproduce(parentGenomeA, parentGenomeB, newOffspring);
            _mutate(newOffspring);

            // move, no realloc of the weights
            offsprings.push_back(std::move(newOffspring));
        }

    } // crossover: breed best genomes

    { // diversity: add random genomes

        // if there is any space left: add some random genome.
        std::size_t remainingOffsprings = _genomes.size() - offsprings.size();

        const unsigned int totalWeights = _neuralNetworkTopology.getTotalWeights();

        for (std::size_t ii = 0; ii < remainingOffsprings; ++ii)
        {
            Genome newGenome;

            newGenome.weights.reserve(totalWeights); // pre-allocate
            for (unsigned int jj = 0; jj < totalWeights; ++jj)
                newGenome.weights.push_back(RNG::getRangedValue(-1.0f, 1.0f));

            // move, no realloc of the weights
            offsprings.push_back(std::move(newGenome));
        }

    } // diversity: add random genomes

    _genomes = std::move(offsprings); // move, no realloc of the vector content

    for (unsigned int ii = 0; ii < _genomes.size(); ++ii)
        _neuralNetworks[ii]->setWeights(_genomes[ii].weights);

    ++_currentGeneration;

    return isSmarterGeneration;
}

void GeneticAlgorithm::_getBestGenomes(Genomes& output) const
{
    output.clear();

    struct SortPair
    {
        float fitness;
        unsigned int index;
    };
    std::vector<SortPair> sortedGenomes;

    sortedGenomes.reserve(_genomes.size()); // pre-allocate
    for (unsigned int ii = 0; ii < _genomes.size(); ++ii)
        sortedGenomes.push_back({ _genomes[ii].fitness, ii });

    // sort by fitness
    auto cmpFunc = [](const SortPair& a, const SortPair& b)
    {
        // the higher the better
        return a.fitness > b.fitness;
    };
    std::sort(sortedGenomes.begin(), sortedGenomes.end(), cmpFunc);

    output.reserve(_genomes.size()); // pre-allocate
    for (const auto& sortedGenome : sortedGenomes)
        output.push_back(_genomes[sortedGenome.index]);
}

void GeneticAlgorithm::_reproduce(const Genome& parentA,
                                  const Genome& parentB,
                                  Genome& offspring) const
{
    // default of 50/50 chances for both parents
    int chancesForParentA = 50; // 50%

    // 70/30 chances for the fittest parent
    if (parentA.fitness > parentB.fitness)
        chancesForParentA = 70; // 70%
    else if (parentA.fitness < parentB.fitness)
        chancesForParentA = 30; // 30%

    // crossover

    const unsigned int totalWeights = _neuralNetworkTopology.getTotalWeights();

    offspring.weights.clear();
    offspring.weights.reserve(totalWeights); // pre-allocate

    for (unsigned int ii = 0; ii < totalWeights; ++ii)
    {
        if (RNG::getRangedValue(0, 100) < chancesForParentA)
            offspring.weights.push_back(parentA.weights[ii]);
        else
            offspring.weights.push_back(parentB.weights[ii]);
    }
}

void GeneticAlgorithm::_mutate(Genome& genome) const
{
    constexpr int mutationMaxChance = 10; // 10%
    constexpr float mutationMaxEffect = 0.2f; // 20% x 2 = 40%

    for (float& weight : genome.weights)
        if (RNG::getRangedValue(0, 100) < mutationMaxChance)
            weight += RNG::getRangedValue(-mutationMaxEffect, +mutationMaxEffect);
}

//

const NeuralNetworks& GeneticAlgorithm::getNeuralNetworks() const
{
    return _neuralNetworks;
}

const Genomes& GeneticAlgorithm::getGenomes() const
{
    return _genomes;
}

const Genome& GeneticAlgorithm::getBestGenome() const
{
    return _bestGenomes[0];
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
