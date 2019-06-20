
#pragma once

#include "demo/defines.hpp"

#if not defined D_WEB_WEBWORKER_BUILD
#	error "exclude this file to build natively or with multi thread support"
#endif

#include "demo/logic/physic/PhysicWorld.hpp"

#include "demo/simulation/machineLearning/NeuralNetwork.hpp"

#include "demo/simulation/logic/Car.hpp"

#include "demo/simulation/webworker/common.hpp"

#include "demo/utilities/message/MessageBuffer.hpp"
#include "demo/utilities/message/MessageView.hpp"

#include "thirdparty/GLMath.hpp"

#include <vector>

class WorkerConsumer
{
private:
    unsigned int		_genomesPerCore = 0;

	PhysicWorld		_physicWorld;
	std::vector<Car>	_cars;

	NeuralNetworkTopology		_neuralNetworkTopology;
	std::vector<NeuralNetwork>	_neuralNetworks;

	// std::vector<std::pair<glm::vec3, glm::vec3>>	m_contacts;

	MessageBuffer	_message;

public:
	WorkerConsumer();

private:
	void	generateCircuit();
	void	generateCars();
	void	generateNeuralNetworks();

public:
	void	processMessage(const char* dataPointer, int dataSize);

private:
	void	send();

private:
	void	initialiseSimulation(MessageView& message);
	void	resetSimulation(MessageView& message);
	void	processSimulation();

};
