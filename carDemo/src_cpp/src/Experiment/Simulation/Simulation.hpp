
#ifndef D_SIMULATION_HPP
#define D_SIMULATION_HPP




#include "logic/Circuit.hpp"
#include "logic/Car.hpp"

#include "ai/GeneticAlgorithm.hpp"
#include "ai/NeuralNetworkTopology.hpp"

#include "utils/t_line.hpp"





//
// SIMULATION

class Simulation
{
private: // attributs
	Circuit						m_Circuit;
	std::vector<Car>			m_Cars;

	NeuralNetworkTopology		m_NNTopology;
	GeneticAlgorithm			m_GenAlgo;

	std::vector< std::vector<t_line> >	m_trails;

public: // ctor/dtor
	Simulation(const std::string& filename);

public: // methods
	void	update(float step);

public: // setter/getter
	inline const Circuit&			getCircuit() const { return m_Circuit; }
	inline const std::vector<Car>&	getCars() const { return m_Cars; }

	inline unsigned int getCurrentGeneration() const { return m_GenAlgo.getCurrentGeneration(); }
	inline unsigned int	getBestFitness() const { return m_GenAlgo.getBestFitness(); }

	inline const std::vector<GeneticAlgorithm::t_genome>&	getGenomes() const { return m_GenAlgo.getGenomes(); }
	inline const NeuralNetworkTopology& getNNTopology() const { return m_NNTopology; }

	inline const std::vector< std::vector<t_line> >&	getTrails() const { return m_trails; };


};

// SIMULATION
//



#endif // D_SIMULATION_HPP
