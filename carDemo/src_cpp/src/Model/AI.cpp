


#include "AI.hpp"


// #include <sstream>
#include <iomanip>

#include <exception>
#include <stdexcept>


//
//
//
// UTILS

namespace
{
	unsigned long	s_seed = 1;
}


int	my_rand()
{
	long	hi, lo, x;

	if (s_seed == 0)
		s_seed = 123459876;

	hi = s_seed / 127773;
	lo = s_seed % 127773;
	x = 16807 * lo - 2836 * hi;

	if (x < 0)
		x += 0x7fffffff;

	return ( (s_seed = x) % ((unsigned long)D_RAND_MAX + 1));
}

void	my_srand(int seed)
{
	s_seed = seed;
}

// UTILS
//
//
//












NeuralNetworkTopology::NeuralNetworkTopology(unsigned int input, const std::vector<unsigned int>& hiddens, unsigned int output)
	:	m_input(input),
		m_hiddens(hiddens),
		m_output(output),
		m_totalWeights(0)
{
	if (!input)
		throw std::invalid_argument( "received invalid number of inputs" );

	// if (hiddens.empty())
	// 	throw std::invalid_argument( "received invalid number of hidden layer" );

	for (unsigned int val : hiddens)
		if (!val)
			throw std::invalid_argument( "received invalid number of hidden neurons" );

	if (!output)
		throw std::invalid_argument( "received invalid number of outputs" );

	//

	unsigned int prev_layer_num_neuron = input;
	for (unsigned int num_neuron : hiddens)
	{
		m_totalWeights += prev_layer_num_neuron * num_neuron;
		prev_layer_num_neuron = num_neuron;
	}
	m_totalWeights += prev_layer_num_neuron * output;
}















//
//
//
// NeuralNetwork

NeuralNetwork::NeuralNetwork(const NeuralNetworkTopology& topology)
	:	m_topology(topology)
{
	unsigned int prev_layer_num_neuron = m_topology.getInput();

	m_layerHidden.resize( m_topology.getHiddens().size() );
	for (unsigned int i = 0; i < m_layerHidden.size(); ++i)
	{
		t_layer&	layer = m_layerHidden[i];

		layer.resize( m_topology.getHiddens()[i] );
		for (t_neuron& neuron : layer)
		{
			neuron.m_weights.resize( prev_layer_num_neuron );
			for (float& weight : neuron.m_weights)
				weight = randomClamped();
		}

		prev_layer_num_neuron = m_topology.getHiddens()[i];
	}

	m_layerOutput.resize( m_topology.getOutput() );
	for (t_neuron& neuron : m_layerOutput)
	{
		neuron.m_weights.resize( prev_layer_num_neuron );
		for (float& weight : neuron.m_weights)
			weight = randomClamped();
	}
}

void NeuralNetwork::process(const std::vector<float>& input, std::vector<float>& output) const
{
	if (input.size() != 5)
	    throw std::invalid_argument( "received invalid number of inputs" );

	output.clear();

	//
	// process hidden layer

	std::vector<float>	hidden_input = input;
	std::vector<float>	hidden_output;

	// Cycle over all the neurons and sum their weights against the inputs.
	for (const t_layer&	current_layer : m_layerHidden)
	{
		hidden_output.clear();

		for (unsigned int i = 0; i < current_layer.size(); i++)
		{
			float activation = 0.0f;

			// Sum the weights to the activation value.
			// We do the sizeof the weights - 1 so that we can add in the bias to the 
			// activation afterwards.
			for (unsigned int j = 0; j < hidden_input.size(); j++)
				activation += hidden_input[j] * current_layer[i].m_weights[j];

			// // Add the bias, it will act as a threshold value
			// activation += (-1) * current_layer[i].m_weights[ (hidden_input.size()-1) ];

			// hidden_output.push_back( sigmoid(activation, 1.0f) );
			hidden_output.push_back( activation );
		}

		hidden_input = hidden_output;
	}

	// process hidden layer
	//

	//
	// process output layer

	// Cycle over all the neurons and sum their weights against the inputs.
	for (unsigned int i = 0; i < m_layerOutput.size(); i++)
	{
		float activation = 0.0f;

		// Sum the weights to the activation value.
		// We do the sizeof the weights - 1 so that we can add in the bias to the 
		// activation afterwards.
		for (unsigned int j = 0; j < hidden_input.size(); j++)
			activation += hidden_input[j] * m_layerOutput[i].m_weights[j];

		// // Add the bias, it will act as a threshold value
		// activation += (-1) * m_layerOutput[i].m_weights[ (hidden_output.size() - 1) ];

		// output.push_back( sigmoid(activation, 1.0f) );
		output.push_back( activation );
	}

	// process output layer
	//
}

void NeuralNetwork::setWeights(const std::vector<float>& in_weights)
{
	if (in_weights.size() != m_topology.getTotalWeights())
	    throw std::invalid_argument( "received invalid number of weights" );

	unsigned int weights_inc = 0;

	for (t_layer& layer : m_layerHidden)
		for (t_neuron& neuron : layer)
			for (float& weight : neuron.m_weights)
				weight = in_weights[ weights_inc++ ];

	for (t_neuron& neuron : m_layerOutput)
		for (float& weight : neuron.m_weights)
			weight = in_weights[ weights_inc++ ];
}

void NeuralNetwork::getWeights(std::vector<float>& out_weights) const
{
	out_weights.reserve( m_topology.getTotalWeights() );

	for (const t_layer& layer : m_layerHidden)
		for (const t_neuron& neuron : layer)
			for (const float weight : neuron.m_weights)
				out_weights.push_back( weight );

	for (const t_neuron& neuron : m_layerOutput)
		for (const float weight : neuron.m_weights)
			out_weights.push_back( weight );
}

// NeuralNetwork
//
//
//






















//
//
//
// GeneticAlgorithm

GeneticAlgorithm::GeneticAlgorithm()
	:	m_current_id(1),
		m_current_generation(1),
		m_best_fitness(0.0f),
		m_stalling_generations(0),
		// m_pNNTopology(NULL),
		m_is_a_great_generation(false)
{
	my_srand( time(NULL) );

	// float min = 1.0f;
	// float max = -1.0f;
	// for (int i = 0; i < 1000; ++i)
	// 	// std::cout << randomFloat() << std::endl;
	// {
	// 	float	v = randomClamped();
	// 	// float	v = randomFloat();
	// 	std::cout << v << std::endl;
	// 	max = std::max(v, max);
	// 	min = std::min(v, min);
	// }
	// std::cout << "max=" << max << std::endl;
	// std::cout << "min=" << min << std::endl;

	this->generateRandomPopulation();
}

// GeneticAlgorithm::~GeneticAlgorithm()
// {
// 	delete m_pNNTopology, m_pNNTopology = NULL;
// }

void	GeneticAlgorithm::generateRandomPopulation()
{
	std::vector<unsigned int> tmp_hidden;
	tmp_hidden.push_back(8);
	tmp_hidden.push_back(7);
	// tmp_hidden.push_back(6);
	// tmp_hidden.push_back(7);
	// tmp_hidden.push_back(7);
	// tmp_hidden.push_back(7);
	// tmp_hidden.push_back(6);
	// tmp_hidden.push_back(5);
	// tmp_hidden.push_back(4);
	// tmp_hidden.push_back(4);
	// tmp_hidden.push_back(3);
	m_pNNTopology = t_pNNTopology(new NeuralNetworkTopology(5, tmp_hidden, 2));



	// reset the genomes
	m_genomes.resize(30);

	for (unsigned int i = 0; i < m_genomes.size(); ++i)
	{
		t_genome& genome = m_genomes[i];

		genome.m_index = i;
		genome.m_id = m_current_id++;
		genome.m_fitness = 0.0f;

		genome.m_weights.resize( m_pNNTopology->getTotalWeights() );

		for (float& weight : genome.m_weights)
			weight = randomClamped();
	}




	m_NNetworks.reserve( m_genomes.size() );
	for (unsigned int i = 0; i < m_genomes.size(); ++i)
	{
		m_NNetworks.push_back( NeuralNetwork(*m_pNNTopology) );

		m_NNetworks[i].setWeights( m_genomes[i].m_weights );
	}





	m_current_generation = 1;
	m_best_fitness = 0.0f;

	m_alpha_genome.m_index = -1;
	m_alpha_genome.m_id = -1;
	m_alpha_genome.m_fitness = 0.0f;

	m_stalling_generations = 0;
}

void	GeneticAlgorithm::BreedPopulation()
{
	// D_MYLOG("step");

	if (m_genomes.empty())
		return;

	std::vector<t_genome>	bestGenomes;
	getBestGenomes(4, bestGenomes);


	// D_MYLOG("current_best=" << bestGenomes[0].m_fitness);


	// D_MYLOG("step");

	// for (t_genome& g : bestGenomes)
	// 	D_MYLOG("g=" << g.m_id);

	std::vector<t_genome>	children;
	children.reserve( m_genomes.size() );

	

	// m_best_fitness = std::max(m_best_fitness, bestGenomes[0].m_fitness);
	if (m_best_fitness < bestGenomes[0].m_fitness)
	{
		m_best_fitness = bestGenomes[0].m_fitness;

		D_MYLOG("m_current_generation=" << m_current_generation
			<< ", m_best_fitness=" << m_best_fitness
			<< ", m_stalling_generations=" << m_stalling_generations << "/2000");

		m_is_a_great_generation = true;

		m_stalling_generations = 0;
	}
	else
	{
		++m_stalling_generations;

		m_is_a_great_generation = false;
	}

	if (m_stalling_generations >= 2000)
	{
		D_MYLOG("stalling generations detected, value=" << m_stalling_generations);
		m_stalling_generations = 0;
	}


	unsigned int current_index = 0;

	if (m_best_fitness > m_alpha_genome.m_fitness)
	{
		m_alpha_genome = bestGenomes[0];
		// D_MYLOG("new alpha");
	}
	else
	{
		t_genome	bestDude;
		bestDude.m_fitness = 0.0f;
		bestDude.m_id = m_alpha_genome.m_id;
		bestDude.m_weights = m_alpha_genome.m_weights;
		bestDude.m_index = current_index++;

		mutate(bestDude);
		children.push_back(bestDude);

		// D_MYLOG("alpha reused");
	}


	{
		// Carry on the best dude.
		t_genome	bestDude;
		bestDude.m_fitness = 0.0f;
		bestDude.m_id = bestGenomes[0].m_id;
		bestDude.m_weights = bestGenomes[0].m_weights;
		bestDude.m_index = current_index++;

		mutate(bestDude);
		children.push_back(bestDude);
	}


	// D_MYLOG("step");

	// breed best genomes

	for (unsigned int i = 0; i < bestGenomes.size(); ++i)
		for (unsigned int j = i+1; j < bestGenomes.size(); ++j)
		{
			// D_MYLOG("i=" << i << ", j=" << j);

			t_genome	baby1, baby2;

			CrossBreed(bestGenomes[i], bestGenomes[j], baby1, baby2);
			mutate(baby1);
			mutate(baby2);

			baby1.m_index = current_index++;
			baby2.m_index = current_index++;

			children.push_back(baby1);
			children.push_back(baby2);
		}

	// D_MYLOG("step");

	// For the remainding n population, add some random kiddies.
	unsigned int remainingChildren = (m_genomes.size() - children.size());

	// D_MYLOG("step m_genomes.size()" << m_genomes.size());
	// D_MYLOG("step children.size()" << children.size());
	// D_MYLOG("step remainingChildren" << remainingChildren);

	for (unsigned int i = 0; i < remainingChildren; i++)
	{
		t_genome	genome;
		genome.m_id = m_current_id++;
		genome.m_fitness = 0.0f;

		genome.m_weights.resize( m_pNNTopology->getTotalWeights() );

		for (float& weight : genome.m_weights)
			weight = randomClamped();

		genome.m_index = current_index++;
		children.push_back( genome );
	}

	// D_MYLOG("step");

	m_genomes = children;
	++m_current_generation;

	// D_MYLOG("/step");

	for (unsigned int i = 0; i < m_genomes.size(); ++i)
		m_NNetworks[i].setWeights( m_genomes[i].m_weights );
}

void	GeneticAlgorithm::getBestGenomes(unsigned int totalAsked, std::vector<t_genome>& out) const
{
	// realistic total outputed genomes 
	totalAsked = std::min<unsigned int>(m_genomes.size(), totalAsked);

	out.clear();

	while (out.size() < totalAsked)
	{
		float bestFitness = 0;
		int bestIndex = -1;
		for (unsigned int i = 0; i < m_genomes.size(); i++)
			if (m_genomes[i].m_fitness > bestFitness)
			{
				bool isUsed = false;

				// no duplicate
				for (unsigned int j = 0; j < out.size(); j++)
					if (out[j].m_id == m_genomes[i].m_id)
					{
						isUsed = true;
						break;
					}

				if (isUsed)
					continue;

				bestIndex = i;
				bestFitness = m_genomes[bestIndex].m_fitness;
			}

		if (bestIndex != -1)
			out.push_back( m_genomes[bestIndex] );
	}
}

void	GeneticAlgorithm::CrossBreed(const t_genome& g1, const t_genome& g2, t_genome& baby1, t_genome& baby2)
{
	unsigned int totalWeights = g1.m_weights.size();
	unsigned int crossover = std::min(randomFloat(), 0.99f) * totalWeights;

	// D_MYLOG("crossover=" << crossover << "/" << totalWeights);

	baby1.m_id = m_current_id++;
	baby1.m_weights.resize(totalWeights);

	baby2.m_id = m_current_id++;
	baby2.m_weights.resize(totalWeights);

	for (unsigned int i = 0; i < crossover; i++)
	{
		baby1.m_weights[i] = g1.m_weights[i];
		baby2.m_weights[i] = g2.m_weights[i];
	}

	for (unsigned int i = crossover; i < totalWeights; i++)
	{
		baby1.m_weights[i] = g2.m_weights[i];
		baby2.m_weights[i] = g1.m_weights[i];
	}
}

void	GeneticAlgorithm::mutate(t_genome& genome) const
{
	int total_mutated = 0;

	for (float& weight : genome.m_weights)
		if (randomFloat() < 0.6f)
		{
			weight += (randomClamped() * 0.3f);

			++total_mutated;
		}

	// D_MYLOG("total_mutated=" << ((float)total_mutated / genome.m_weights.size()) * 100);
}

// GeneticAlgorithm
//
//
//


