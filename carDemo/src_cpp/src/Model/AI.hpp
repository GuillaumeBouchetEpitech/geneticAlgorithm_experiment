
#ifndef D_AI_HPP
#define D_AI_HPP



#include "TraceLogger.hpp"


#include <memory>

#include <iostream>
#include <vector>
#include <cmath>









//
//
//
// UTILS

// #define	D_RAND_MAX	2147483647 // int max positive value : 2,147,483,647
#define	D_RAND_MAX	2147483648

int		my_rand();
void	my_srand(int seed);

inline int		my_rangeRand(int min, int max)
{
	return  ( min + ( my_rand() / D_RAND_MAX * (max - min) ) );
}

inline float	my_rangeRand(float min, float max)
{
	return  ( min + ( (float)my_rand() / D_RAND_MAX * (max - min) ) );
}

inline float randomFloat()
{
	// return ((float)(rand())) / ((float)(RAND_MAX));
	return my_rangeRand(0.0f, 1.0f);
}

inline float	randomClamped()
{
	// return (randomFloat() - randomFloat());
	return my_rangeRand(-1.0f, 1.0f);
}

// inline float	sigmoid(float a, float p)
// {
// 	return (1.0f / (1.0f + expf( (-a) / p )));
// }

// UTILS
//
//
//





//
//
//
// NeuralNetworkTopology

class NeuralNetworkTopology
{
private: // attributs
	unsigned int				m_input;
	std::vector<unsigned int>	m_hiddens;
	unsigned int				m_output;

	unsigned int				m_totalWeights;

public:
	NeuralNetworkTopology(unsigned int input, const std::vector<unsigned int>& hiddens, unsigned int output);

public:
	inline unsigned int	getInput() const { return m_input; }
	inline unsigned int	getOutput() const { return m_output; }
	inline const std::vector<unsigned int>&	getHiddens() const { return m_hiddens; }

	inline unsigned int	getTotalWeights() const { return m_totalWeights; }
};

// NeuralNetworkTopology
//
//
//














//
//
//
// NeuralNetwork

class NeuralNetwork
{
private: // internal structures
	struct t_neuron
	{
	public:
		std::vector<float>	m_weights;
	};

	typedef std::vector<t_neuron>	t_layer;

private: // attributs

	std::vector<t_layer>	m_layerHidden;
	t_layer					m_layerOutput;

	const NeuralNetworkTopology&	m_topology;

public: // ctor/dtor
	NeuralNetwork(const NeuralNetworkTopology& topology);

public: // methods
	void process(const std::vector<float>& input, std::vector<float>& output) const;

public: // setter/getter
	void setWeights(const std::vector<float>& in_weights);
	void getWeights(std::vector<float>& out_weights) const;

};

// NeuralNetwork
//
//
//












//
//
//
// GeneticAlgorithm

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
	unsigned int			m_current_id;
	unsigned int			m_current_generation;

	float					m_best_fitness;

	t_genome				m_alpha_genome;

	unsigned int			m_stalling_generations;

	typedef std::unique_ptr<NeuralNetworkTopology>	t_pNNTopology;
	t_pNNTopology			m_pNNTopology;

	typedef std::vector<NeuralNetwork>	t_NNetworks;
	t_NNetworks	m_NNetworks;

	bool	m_is_a_great_generation;

public: // ctor/dtor
	GeneticAlgorithm();
	// ~GeneticAlgorithm();

public: // getters/setters
	inline unsigned int getCurrentGeneration() const { return m_current_generation; }
	inline float getBestFitness() const { return m_best_fitness; }

	inline const t_NNetworks&	getNNetworks() const { return m_NNetworks; }

	inline bool	isAGreatGeneration() const { return m_is_a_great_generation; }
	inline const t_genome&	getAlpha() const { return m_alpha_genome; }

	inline const std::vector<t_genome>&	getGenomes() const { return m_genomes; }
	inline void rateGenome(unsigned int index, float fitness) { m_genomes.at(index).m_fitness = fitness; }

private: // methods
	void	generateRandomPopulation();

public: // methods
	void	BreedPopulation();

private: // methods
	void	getBestGenomes(unsigned int totalAsked, std::vector<t_genome>& out) const;
	void	CrossBreed(const t_genome& g1, const t_genome& g2, t_genome& baby1, t_genome& baby2);
	void	mutate(t_genome& genome) const;

};

// GeneticAlgorithm
//
//
//










#endif // D_AI_HPP
