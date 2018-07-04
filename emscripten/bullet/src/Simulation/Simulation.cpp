

#include "Simulation.hpp"

#include "Utility/TraceLogger.hpp"


Simulation::Simulation(IPhysicWrapper* pPhysicWrapper)
	: m_pPhysicWrapper(pPhysicWrapper)
{
}

Simulation::~Simulation()
{
}

void	Simulation::initialise(
	// const std::vector<float>& arr_left,
	// const std::vector<float>& arr_right,
	// Circuit::t_geometry_callback callback,
	const std::string& filename,
	CircuitBuilder::t_callback skeleton,
	CircuitBuilder::t_callback ground,
	CircuitBuilder::t_callback wall)
{
	// m_Circuit.initialise(m_pPhysicWrapper, arr_left, arr_right, callback);

	{
		CircuitBuilder	circuit;
		if (!circuit.load(filename))
		{
			D_MYLOG("failure");
			return /*false*/; // <= TODO
		}

		circuit.generate_skeleton(skeleton);
		circuit.generate(ground, wall);
	}
	

	unsigned int input = 15;
	std::vector<unsigned int> tmp_hidden = { 10, 5 };
	unsigned int output = 15;

	m_NNTopology.init(input, tmp_hidden, output);

	m_GenAlgo.init(m_NNTopology);

	unsigned int	num_car = m_GenAlgo.getGenomes().size();

	m_pPhysicWrapper->createVehicles(num_car + 1); // plus one for the best car

	Car::initialise(m_pPhysicWrapper);

	m_Cars.reserve(num_car);
	for (unsigned int ii = 0; ii < num_car; ++ii)
		m_Cars.push_back(Car(ii));

	m_pBest_car = new Car(num_car);
}

void	Simulation::update(int world_index, float elapsed_time)
{
	// D_MYLOG("world_index=" << world_index);

	m_pPhysicWrapper->step(world_index, elapsed_time);

	// bool	someone_is_alive = false;

	// for (unsigned int i = 0; i < m_Cars.size(); ++i)

	int step = m_Cars.size() / 3;
	for (unsigned int i = step*world_index; i < step*(world_index+1); ++i)
	{
		if (!m_Cars[i].isAlive())
			continue;

		// someone_is_alive = true;

		m_Cars[i].update( m_GenAlgo.getNNetworks()[i] );
	}
}

void	Simulation::evolve()
{
	bool	someone_is_alive = false;

	for (unsigned int i = 0; i < m_Cars.size(); ++i)
	{
		if (!m_Cars[i].isAlive())
			continue;

		someone_is_alive = true;

		// m_Cars[i].update( m_GenAlgo.getNNetworks()[i] );
	}

	if (someone_is_alive)
		return;

	// rate genomes
	for (unsigned int i = 0; i < m_Cars.size(); ++i)
	{
		float final_fitness = m_Cars[i].getFitness();

		// this will reward the fastest car once the reaching the end of the circuit
		final_fitness += static_cast<float>(m_Cars[i].getTotalUpdates()) * 0.00001f;

		m_GenAlgo.rateGenome(i, final_fitness);
	}

	m_GenAlgo.BreedPopulation();

	if (m_GenAlgo.isAGreatGeneration())
	{
		Car& car = m_Cars[ m_GenAlgo.getAlpha().m_index ];

		m_car_trails.push_back( car.getTrail() );

		while (m_car_trails.size() > 5)
			m_car_trails.pop_front();
	}

	//
	//

	for (Car& car : m_Cars)
		car.reset();
}

// void	Simulation::updateBest(float elapsed_time)
// {
// 	if (m_GenAlgo.getAlpha().m_fitness == 0.0f)
// 		return;

// 	m_pPhysicWrapper->step(elapsed_time);

// 	if (m_pBest_car->isAlive())
// 	{
// 		NeuralNetwork	tmp_ann(m_NNTopology);

// 		tmp_ann.setWeights( m_GenAlgo.getAlpha().m_weights );

// 		m_pBest_car->update( tmp_ann );
// 	}
// 	else
// 	{
// 		m_pBest_car->reset();
// 	}
// }
