
#pragma once

#include "NeuralNetwork.hpp"

#include "Genome.hpp"

#include <array>

class GeneticAlgorithm
{
public:
    struct Definition
    {
        unsigned int totalGenomes = 0;
        NeuralNetworkTopology topology;
    };

private: // attributs
    Genomes _genomes;

    unsigned int _currentGeneration = 1; // generation number

    std::array<Genome, 3> _bestGenomes; // keep 3 elites

    NeuralNetworkTopology _neuralNetworkTopology;

    NeuralNetworks _neuralNetworks;

public: // ctor/dtor
    GeneticAlgorithm() = default;

public: // methods
    void initialise(const Definition& def);

public: // method(s)
    bool breedPopulation();

private: // method(s)
    void _getBestGenomes(Genomes& output) const;
    void _reproduce(const Genome& parentA,
                    const Genome& parentB,
                    Genome& offspring) const;
    void _mutate(Genome& genome) const;

public: // getter(s)
    const NeuralNetworks& getNeuralNetworks() const;
    const Genomes& getGenomes() const;
    const Genome& getBestGenome() const;
    unsigned int getGenerationNumber() const;

public: // setter(s)
    void rateGenome(unsigned int index, float fitness);

};
