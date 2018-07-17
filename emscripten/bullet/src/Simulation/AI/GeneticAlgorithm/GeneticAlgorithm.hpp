

#pragma once


#include "Simulation/AI/NeuralNetwork/NeuralNetwork.hpp"

#include <deque>


class GeneticAlgorithm
{
public:
	struct t_genome
	{
		unsigned int		id = 0;
		float				fitness = 0.0f;
		std::vector<float>	weights;

		t_genome() = default;
	};

	typedef std::vector<t_genome>	t_genomes;

private: // attributs
	t_genomes	m_genomes;

	unsigned int			m_currentId = 1; // for the genome id
	unsigned int			m_currentGeneration = 1; // generation number

	t_genome				m_bestGenome;

	NeuralNetworkTopology*	m_pNeuralNetworkTopology = nullptr;

	typedef std::vector<NeuralNetwork>	t_NeuralNetworks;
	t_NeuralNetworks	m_NeuralNetworks;

public: // ctor/dtor
	GeneticAlgorithm();

public: // methods
	void	init(NeuralNetworkTopology& NNTopology);

private: // methods
	void	generateRandomPopulation();

public: // methods
	bool	breedPopulation();

private: // methods
	void	getBestGenomes(unsigned int totalAsked, t_genomes& output) const;
	void	crossBreed(const t_genome& g1, const t_genome& g2, t_genome& offspring1, t_genome& offspring2);
	void	mutate(t_genome& genome) const;

public: // getter(s)

	inline const t_NeuralNetworks&	getNeuralNetworks() const { return m_NeuralNetworks; }
	inline const t_genomes&	getGenomes() const { return m_genomes; }
	inline const t_genome&	getBestGenome() const { return m_bestGenome; }

public: // setter(s)
	inline void rateGenome(unsigned int index, float fitness) { m_genomes.at(index).fitness = fitness; }

};
