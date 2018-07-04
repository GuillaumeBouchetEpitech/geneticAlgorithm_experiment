
#include "Simulation.hpp"


#include "utils/TraceLogger.hpp"



//
//
//
// SIMULATION

Simulation::Simulation(const std::string& filename)
{
	m_Circuit.loadMap(filename);

	std::vector<unsigned int> tmp_hidden;
	tmp_hidden.push_back(4);
	tmp_hidden.push_back(3);
	m_NNTopology.init(5, tmp_hidden, 2);

	m_GenAlgo.init(m_NNTopology);

	m_Cars.resize( m_GenAlgo.getGenomes().size() );

	for (unsigned int i = 0; i < m_Cars.size(); ++i)
	{
		Car& car = m_Cars[i];
		car.reset(m_Circuit);
	}
}

void	Simulation::update(float step)
{
	bool	someone_is_alive = false;

	#pragma omp parallel num_threads(3)
	{
		// This code will be executed by three threads.

		// Chunks of this loop will be divided amongst
		// the (three) threads of the current team.
		#pragma omp for
		for (unsigned int i = 0; i < m_Cars.size(); ++i)
		{
			if (!m_Cars[i].isAlive())
				continue;

			someone_is_alive = true;

			m_Cars[i].update(step, m_Circuit, m_GenAlgo.getNNetworks()[i]);
		}
	}

	if (someone_is_alive)
		return;

	// rate genomes
	for (unsigned int i = 0; i < m_Cars.size(); ++i)
		m_GenAlgo.rateGenome(i, m_Cars[i].getFitness());

	m_GenAlgo.BreedPopulation();

	if (m_GenAlgo.isAGreatGeneration())
	{
		Car& car = m_Cars[ m_GenAlgo.getAlpha().m_index ];

		m_trails.push_back( car.getTrail() );

		while (m_trails.size() > 5)
			m_trails.erase( m_trails.begin() );
	}

	//
	//

	for (Car& car : m_Cars)
		car.reset(m_Circuit);
}

// SIMULATION
//
//
//








