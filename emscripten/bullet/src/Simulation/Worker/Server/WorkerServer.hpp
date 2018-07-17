

#pragma once


#include "Logic/Physic/World.hpp"

#include "Simulation/AI/NeuralNetwork/NeuralNetwork.hpp"

#include "Car.hpp"

#include <vector>

namespace workerClientMessages
{
	struct t_Message;
};

class WorkerServer
{
private:
	Physic::World		m_PhysicWorld;
	std::vector<Car>	m_Cars;

	NeuralNetworkTopology		m_NeuralNetworkTopology;
	std::vector<NeuralNetwork>	m_NeuralNetworks;

public:
	WorkerServer();

private:
	void	generateCircuit();
	void	generateCars();
	void	generateNeuralNetworks();

public:
	void	processMessage(char* pData, int dataLength);

private:
	void	sendMessage(char* data, int size);

private:
	void	resetSimulation(workerClientMessages::t_Message* pMessage);
	void	processSimulation();

};

