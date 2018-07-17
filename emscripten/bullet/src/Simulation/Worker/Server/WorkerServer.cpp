

#include "WorkerServer.hpp"

#include "Simulation/Worker/workerMessages.hpp"

#include "Simulation/Circuit/CircuitBuilder.hpp"

#include "Car.hpp"

#include "Logic/Physic/Vehicle.hpp"

#include "Utility/TraceLogger.hpp"

#include "constants.hpp"


#include <emscripten/emscripten.h> // <= emscripten_worker_respond()


typedef workerClientMessages::t_ResetAndProcessSimulationMessage	t_ResetAndProcess;


WorkerServer::WorkerServer()
{
	generateCircuit();
	generateCars();
	generateNeuralNetworks();
}

void	WorkerServer::generateCircuit()
{
	CircuitBuilder	circuitBuilder;

	if (!circuitBuilder.load(D_CIRCUIT_FILENAME))
	{
		D_MYLOG("failure");
		return /*false*/; // <= TODO
	}

	int physic_index = 0;

	auto onNewGroundPatch = [&](const std::vector<float>& buf, const std::vector<int>& indices) -> void {

		m_PhysicWorld.createGround(buf, indices, physic_index++);
	};

	auto onNewWallPatch = [&](const std::vector<float>& buf, const std::vector<int>& indices) -> void {

		m_PhysicWorld.createWall(buf, indices);
	};

	circuitBuilder.generate(onNewGroundPatch, onNewWallPatch);
}

void	WorkerServer::generateCars()
{
	Car::initialise(&m_PhysicWorld);

	m_Cars.reserve(D_CARS_NUMBER_PER_WORKER);
	for (int ii = 0; ii < D_CARS_NUMBER_PER_WORKER; ++ii)
		m_Cars.push_back(Car());
}


void	WorkerServer::generateNeuralNetworks()
{
	unsigned int layerInput = D_NEURONS_NUMBER_INPUT;
	std::vector<unsigned int> layerHidden = {
		D_NEURONS_NUMBER_HIDDEN_1,
		D_NEURONS_NUMBER_HIDDEN_2
	};
	unsigned int layerOutput = D_NEURONS_NUMBER_OUTPUT;

	m_NeuralNetworkTopology.init(layerInput, layerHidden, layerOutput);

	m_NeuralNetworks.reserve(D_CARS_NUMBER_PER_WORKER);
	for (int ii = 0; ii < D_CARS_NUMBER_PER_WORKER; ++ii)
		m_NeuralNetworks.push_back(NeuralNetwork(m_NeuralNetworkTopology));
}


void	WorkerServer::processMessage(char* pData, int dataLength)
{
	auto*	pMessage = reinterpret_cast<workerClientMessages::t_Message*>(pData);

	switch (pMessage->type)
	{
		case workerClientMessages::eInitialiseSimulation:
		{
			workerServerMessages::t_SimulationInitialisedMessage	message;

			sendMessage(reinterpret_cast<char*>(&message), sizeof(message));
		}
		break;

		case workerClientMessages::eResetAndProcessSimulation:
		{
			resetSimulation(pMessage);
			processSimulation();
		}
		break;

		case workerClientMessages::eProcessSimulation:
		{
			processSimulation();
		}
		break;

		default:
		{
			D_MYLOG("WORKER message => Unknown");
		}
	}
}

void	WorkerServer::sendMessage(char* data, int size)
{
	emscripten_worker_respond(data, size);
}

void	WorkerServer::resetSimulation(workerClientMessages::t_Message* pMessage)
{
	const unsigned int	floatWeightsSize = m_NeuralNetworkTopology.getTotalWeights();
	const unsigned int	byteWeightsSize = floatWeightsSize * sizeof(float);

	auto*	pResetMessage = reinterpret_cast<t_ResetAndProcess*>(pMessage);

	auto*	newWeights = pResetMessage->neuralNetworkWeights;

	std::vector<float>	weightsBuffer;
	weightsBuffer.resize(floatWeightsSize);
	float*	weightsBufferRaw = &weightsBuffer[0];

	for (unsigned int ii = 0; ii < D_CARS_NUMBER_PER_WORKER; ++ii)
	{
		float*	pNewWeights = newWeights[ii].weights;

		memcpy(weightsBufferRaw, pNewWeights, byteWeightsSize);
		m_NeuralNetworks[ii].setWeights(weightsBuffer);

		m_Cars[ii].reset();
	}
}

void	WorkerServer::processSimulation()
{
	workerServerMessages::t_SimulationResultMessage	message;

	const float fixedDeltaTime = 1.0f / 60;

	m_PhysicWorld.step(fixedDeltaTime);

	for (unsigned int ii = 0; ii < D_CARS_NUMBER_PER_WORKER; ++ii)
	{
		auto& carResult = message.carsResult[ii];
		auto& car = m_Cars[ii];

		carResult.isAlive = car.isAlive();
		carResult.fitness = car.getFitness();

		if (!car.isAlive())
			continue;

		car.update(m_NeuralNetworks[ii]);

		auto* pVehicle = m_PhysicWorld.getVehicle(ii);

		// record the transformation matrix of the car
		pVehicle->getOpenGLMatrix(carResult.chassisMat4);

		// record the transformation matrix of the wheels
		for (int jj = 0; jj < 4; ++jj)
			pVehicle->getWheelOpenGLMatrix(jj, carResult.wheelsMat4[jj]);

		auto&& sensors = car.getSensors();
		memcpy(carResult.sensors, &sensors, sizeof(sensors));

		auto&& groundSensor = car.getGroundSensor();
		memcpy(&carResult.groundSensor, &groundSensor, sizeof(groundSensor));
	}

	sendMessage(reinterpret_cast<char*>(&message), sizeof(message));
}

