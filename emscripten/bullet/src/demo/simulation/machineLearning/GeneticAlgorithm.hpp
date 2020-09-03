
#pragma once

#include "NeuralNetwork.hpp"

#include "Genome.hpp"

#include <array>

class GeneticAlgorithm
{
public:
    struct t_def
    {
        unsigned int totalGenomes = 0;
        NeuralNetworkTopology topology;
    };

private: // attributs
    t_genomes _genomes;

    unsigned int _currentGeneration = 1; // generation number

    std::array<Genome, 3> _bestGenomes; // keep 3 elites

    NeuralNetworkTopology _neuralNetworkTopology;

    typedef std::vector<NeuralNetwork> t_NeuralNetworks;
    t_NeuralNetworks _neuralNetworks;

public: // ctor/dtor
    GeneticAlgorithm() = default;

public: // methods
    void initialise(const t_def& def);

public: // methods
    bool breedPopulation();

private: // methods
    void _getBestGenomes(t_genomes& output) const;
    void _reproduce(const Genome& parentA, const Genome& parentB, Genome& offspring) const;
    void _mutate(Genome& genome) const;

public: // getter(s)
    const t_NeuralNetworks& getNeuralNetworks() const;
    const t_genomes& getGenomes() const;
    const Genome& getBestGenome() const;
    unsigned int getGenerationNumber() const;

public: // setter(s)
    void rateGenome(unsigned int index, float fitness);

};
