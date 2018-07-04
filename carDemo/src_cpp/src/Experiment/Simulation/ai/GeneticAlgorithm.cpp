

#include "GeneticAlgorithm.hpp"


#include "../utils/TraceLogger.hpp"
#include "../utils/randomiser.hpp"


#include <ctime> // <- time


GeneticAlgorithm::GeneticAlgorithm()
	:	m_current_id(1),
		m_current_generation(1),
		m_best_fitness(0.0f),
		m_is_a_great_generation(false)
{
	my_srand( time(NULL) );
}

void	GeneticAlgorithm::init(NeuralNetworkTopology& NNTopology)
{
	m_pNNTopology = &NNTopology;
	this->generateRandomPopulation();
}

void	GeneticAlgorithm::generateRandomPopulation()
{
	// reset the genomes
	m_genomes.resize(200);

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
}

void	GeneticAlgorithm::BreedPopulation()
{
	if (m_genomes.empty())
		return;

	std::vector<t_genome>	bestGenomes;
	getBestGenomes(20, bestGenomes);


	std::vector<t_genome>	children;
	children.reserve( m_genomes.size() );
	

	if (m_best_fitness < bestGenomes[0].m_fitness)
	{
		m_best_fitness = bestGenomes[0].m_fitness;

		D_MYLOG("m_current_generation=" << m_current_generation
			<< std::fixed << ", m_best_fitness=" << m_best_fitness);

		m_is_a_great_generation = true;
	}
	else
	{
		m_is_a_great_generation = false;
	}


	unsigned int current_index = 0;

	if (m_best_fitness > m_alpha_genome.m_fitness)
	{
		m_alpha_genome = bestGenomes[0];
	}
	else
	{
		// elitism :
		// here the best (alpha) genome from previous
		// generation is pushed as an unmodified child

		t_genome	bestDude;
		bestDude.m_fitness = 0.0f;
		bestDude.m_id = m_alpha_genome.m_id;
		bestDude.m_weights = m_alpha_genome.m_weights;
		bestDude.m_index = current_index++;

		// mutate(bestDude);
		children.push_back(bestDude);
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

	// breed best genomes

	for (unsigned int i = 0; i < bestGenomes.size(); ++i)
		for (unsigned int j = i+1; j < bestGenomes.size(); ++j)
		{
			t_genome	baby1, baby2;

			CrossBreed(bestGenomes[i], bestGenomes[j], baby1, baby2);
			mutate(baby1);
			mutate(baby2);

			baby1.m_index = current_index++;
			baby2.m_index = current_index++;

			if (children.size() < children.capacity())
				children.push_back(baby1);
			if (children.size() < children.capacity())
				children.push_back(baby2);
		}

	// For the remainding n population, add some random kiddies.
	int remainingChildren = static_cast<int>(m_genomes.size() - children.size());

	for (int i = 0; i < remainingChildren; i++)
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

	m_genomes = children;
	++m_current_generation;

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
		if (randomFloat() < 0.1f)
		{
			weight += (randomClamped() * 0.2f);

			++total_mutated;
		}
}

