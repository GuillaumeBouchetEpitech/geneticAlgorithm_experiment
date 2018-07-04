

#pragma once


#include "NeuralNetwork.hpp"


class GeneticAlgorithm
{
public:
	struct t_genome
	{
		unsigned int		m_id;
		float				m_fitness;
		std::vector<float>	m_weights;
		unsigned int		m_index;
	};

private: // attributs
	std::vector<t_genome>	m_genomes;

	unsigned int			m_current_id = 1; // for the genome id
	unsigned int			m_current_generation = 0; // generation number

	float					m_best_fitness = 0.0f;

	t_genome				m_alpha_genome;

	NeuralNetworkTopology*	m_pNNTopology;

	typedef std::vector<NeuralNetwork>	t_NNetworks;
	t_NNetworks	m_NNetworks;

	bool	m_is_a_great_generation = false;

public: // ctor/dtor
	GeneticAlgorithm(unsigned int number_of_genome = 90);

public: // methods
	void	init(NeuralNetworkTopology& NNTopology);

private: // methods
	void	generateRandomPopulation();

public: // methods
	void	BreedPopulation();

private: // methods
	void	getBestGenomes(unsigned int totalAsked, std::vector<t_genome>& out) const;
	void	CrossBreed(const t_genome& g1, const t_genome& g2, t_genome& baby1, t_genome& baby2);
	void	mutate(t_genome& genome) const;

public: // getters/setters
	inline unsigned int getCurrentGeneration() const { return m_current_generation; }
	inline float getBestFitness() const { return m_best_fitness; }
	inline const t_NNetworks&	getNNetworks() const { return m_NNetworks; }
	inline bool	isAGreatGeneration() const { return m_is_a_great_generation; }
	inline const t_genome&	getAlpha() const { return m_alpha_genome; }
	inline const std::vector<t_genome>&	getGenomes() const { return m_genomes; }
	inline void rateGenome(unsigned int index, float fitness) { m_genomes.at(index).m_fitness = fitness; }

};
