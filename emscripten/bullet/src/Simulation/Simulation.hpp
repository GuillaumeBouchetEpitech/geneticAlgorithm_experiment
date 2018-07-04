

#pragma once


#include "IPhysicWrapper.hpp"

// #include "Circuit.hpp"
#include "CircuitBuilder.hpp"
#include "Car.hpp"

#include "AI/GeneticAlgorithm.hpp"


#include <list>


class Simulation
{
private:
	IPhysicWrapper*	m_pPhysicWrapper;

	// Circuit						m_Circuit;
	std::vector<Car>			m_Cars;

	NeuralNetworkTopology		m_NNTopology;
	GeneticAlgorithm			m_GenAlgo;

	std::list< std::vector<float> >	m_car_trails;

	std::vector<float>			m_best_weights;
	Car*						m_pBest_car = nullptr;

public:
	Simulation(IPhysicWrapper* pPhysicWrapper);
	~Simulation();

public:
	void	initialise(
		// const std::vector<float>& arr_left,
		// const std::vector<float>& arr_right,
		// Circuit::t_geometry_callback callback,
		const std::string& filename,
		CircuitBuilder::t_callback skeleton,
		CircuitBuilder::t_callback ground,
		CircuitBuilder::t_callback wall
	);

public:
	void	update(int world_index, float elapsed_time);
	void	evolve();
	// void	updateBest(float elapsed_time);

public:
	const std::vector<Car>&	getCars() const { return m_Cars; }
	const std::list< std::vector<float> >&	getCarTrails() const { return m_car_trails; }

	const Car&				getBestCar() const { return *m_pBest_car; }
};
