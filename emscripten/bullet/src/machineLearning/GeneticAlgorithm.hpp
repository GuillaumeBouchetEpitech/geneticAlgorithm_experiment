
#pragma once

#include "NeuralNetwork.hpp"

#include "Genome.hpp"

#include "framework/NonCopyable.hpp"

#include <array>

class GeneticAlgorithm : public NonCopyable {
public:
  struct Definition {
    unsigned int totalGenomes = 0;
    NeuralNetworkTopology topology;
  };

private: // attributes
  Genomes _genomes;
  Genomes _eliteGenomes; // keep X elites

  unsigned int _currentGeneration = 1; // generation number

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
  void _reproduce(const Genome& parentA, const Genome& parentB,
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
