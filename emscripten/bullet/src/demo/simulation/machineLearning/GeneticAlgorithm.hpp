
#pragma once

#include "NeuralNetwork.hpp"

#include <deque>

class GeneticAlgorithm
{
public:
	struct t_def
	{
		unsigned int			totalGenomes = 0;
		NeuralNetworkTopology	topology;
	};

	struct t_genome
	{
		unsigned int		id = 0;
		float				fitness = 0.0f;
		std::vector<float>	weights;
	};

	typedef std::vector<t_genome>	t_genomes;

private: // attributs
	t_genomes	_genomes;

	unsigned int			_currentId = 1; // for the genome id
	unsigned int			_currentGeneration = 1; // generation number

	t_genome				_bestGenome;

	NeuralNetworkTopology	_neuralNetworkTopology;

	typedef std::vector<NeuralNetwork>	t_NeuralNetworks;
	t_NeuralNetworks	_neuralNetworks;

public: // ctor/dtor
	GeneticAlgorithm();

public: // methods
	void	initialise(const t_def& def);

private: // methods
	void	generateRandomPopulation();

public: // methods
	bool	breedPopulation();

private: // methods
	void	getBestGenomes(t_genomes& output) const;
	void	crossBreed(const t_genome& g1, const t_genome& g2, t_genome& offspring1, t_genome& offspring2);
	void	mutate(t_genome& genome) const;

public: // getter(s)
	const t_NeuralNetworks&	getNeuralNetworks() const;
	const t_genomes&	getGenomes() const;
	const t_genome&		getBestGenome() const;
	unsigned int		getGenerationNumber() const;

public: // setter(s)
	void rateGenome(unsigned int index, float fitness);

};
