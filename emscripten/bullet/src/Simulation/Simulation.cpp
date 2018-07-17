

#include "Simulation.hpp"

#include "Utility/TraceLogger.hpp"

#include "constants.hpp"

#include <memory> // <= std::unique_ptr

Simulation::Simulation()
{
}

Simulation::~Simulation()
{
}

void	Simulation::initialise(
	const std::string& filename,
	CircuitBuilder::t_callback skeleton,
	CircuitBuilder::t_callback ground,
	CircuitBuilder::t_callback wall)
{
	{
		CircuitBuilder	circuit;
		if (!circuit.load(filename))
		{
			D_MYLOG("failure");
			return /*false*/; // <= TODO
		}

		circuit.generateSkeleton(skeleton);
		circuit.generate(ground, wall);
	}

	unsigned int layerInput = D_NEURONS_NUMBER_INPUT;
	std::vector<unsigned int> layerHidden = {
		D_NEURONS_NUMBER_HIDDEN_1,
		D_NEURONS_NUMBER_HIDDEN_2
	};
	unsigned int layerOutput = D_NEURONS_NUMBER_OUTPUT;

	m_NeuralNetworkTopology.init(layerInput, layerHidden, layerOutput, D_NEURAL_NETWORK_USE_BIAS);

	m_GeneticAlgorithm.init(m_NeuralNetworkTopology);

	for (int ii = 0; ii < D_WORKERS_NUMBER; ++ii)
	{
		WorkerClient* pWorkerClient = new WorkerClient();
		m_pWorkerClients.push_back(pWorkerClient);
	}

	initialiseSimulation();
}

void	Simulation::update()
{
	// do nothing if the worker(s) are:
	// => not initialised
	// => not finished working
	for (auto* pWorkerClient : m_pWorkerClients)
		if (!pWorkerClient->isInitialised() || pWorkerClient->isProcessing())
			return;

	// if this line is reached it mean the worker(s) are now available

	if (m_currentRequest == e_WorkerRequest::eInitialise)
	{
		// ask to reset if any worker(s) are not yet updated
		// => it should only happen the first time

		resetAndProcessSimulation();
		return;
	}

	bool	incompleteSimulation = false;

	for (unsigned int ii = 0; ii < D_CARS_NUMBER_TOTAL; ++ii)
	{
		auto&& carResult = getCarResult(ii);

		if (!carResult.isAlive)
			continue;

		incompleteSimulation = true;
	}

	if (m_currentRequest == e_WorkerRequest::eResetAndProcess)
	{
		if (m_onResetAndProcessCallback)
			m_onResetAndProcessCallback();
	}
	else if (m_currentRequest == e_WorkerRequest::eProcess)
	{
		if (m_onProcessCallback)
			m_onProcessCallback();
	}

	if (incompleteSimulation)
	{
		// ask the worker(s) to process/update the (physic) simulation
		processSimulation();
	}
	else
	{
		for (unsigned int ii = 0; ii < D_CARS_NUMBER_TOTAL; ++ii)
		{
			auto&& carResult = getCarResult(ii);

			float fitness = carResult.fitness;

			// // this reward a fast car once reached the end of the circuit
			// fitness += static_cast<float>(m_Cars[ii].getTotalUpdates()) * 0.00001f;

			m_GeneticAlgorithm.rateGenome(ii, fitness);
		}

		bool	smarterGeneration = m_GeneticAlgorithm.breedPopulation();

		if (smarterGeneration)
		{
			if (m_onSmarterCallback)
				m_onSmarterCallback();
		}

		// ask the worker(s) to reset the (physic) simulation
		resetAndProcessSimulation();
	}
}

void	Simulation::initialiseSimulation()
{
	for (auto* pWorkerClient : m_pWorkerClients)
		pWorkerClient->initialiseSimulation();

	m_currentRequest = e_WorkerRequest::eInitialise;
}

void	Simulation::processSimulation()
{
	for (auto* pWorkerClient : m_pWorkerClients)
		pWorkerClient->processSimulation();

	m_currentRequest = e_WorkerRequest::eProcess;
}

void	Simulation::resetAndProcessSimulation()
{
	const unsigned int	floatWeightsSize = m_NeuralNetworkTopology.getTotalWeights();
	const unsigned int	byteWeightsSize = floatWeightsSize * sizeof(float);
	const unsigned int	byteAllWeightsSize = D_CARS_NUMBER_PER_WORKER * byteWeightsSize;

	std::unique_ptr<char[]>	pBuffer(new char[byteAllWeightsSize]);
	char*	pBufferRaw = pBuffer.get();

	std::vector<float>	weights;

	auto&&	NNetworks = m_GeneticAlgorithm.getNeuralNetworks();

	unsigned int currentNNetworkIndex = 0;

	for (auto* pWorkerClient : m_pWorkerClients)
	{
		for (unsigned int ii = 0; ii < D_CARS_NUMBER_PER_WORKER; ++ii)
		{
			void*	pDest = &pBufferRaw[byteWeightsSize * ii];

			auto&&	currentNNetwork = NNetworks[currentNNetworkIndex++];
			currentNNetwork.getWeights(weights);

			memcpy(pDest, &weights[0], byteWeightsSize);
		}

		pWorkerClient->resetAndProcessSimulation(pBufferRaw, byteAllWeightsSize);
	}

	m_currentRequest = e_WorkerRequest::eResetAndProcess;
}

const t_carResult&	Simulation::getCarResult(unsigned int index) const
{
	const unsigned int workerIndex = index / D_CARS_NUMBER_PER_WORKER;
	const unsigned int workerResultIndex = index % D_CARS_NUMBER_PER_WORKER;

	const WorkerClient* pWorkerClient = m_pWorkerClients.at(workerIndex);

	return pWorkerClient->getSimulationResults().carsResult[workerResultIndex];
}
