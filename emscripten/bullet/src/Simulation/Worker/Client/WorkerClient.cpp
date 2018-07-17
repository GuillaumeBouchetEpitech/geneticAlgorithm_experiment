

#include "WorkerClient.hpp"

#include "Utility/TraceLogger.hpp"

#include "constants.hpp"

#include <cstring>


WorkerClient::WorkerClient()
{
	m_workerHandle = emscripten_create_worker(D_WORKER_SCRIPT_URL);

	m_flags[e_Status::eInitialised] = false;
	m_flags[e_Status::eProcessing] = false;
	m_flags[e_Status::eUpdated] = false;

	memset(&m_latestResults, 0, sizeof(m_latestResults));
}

// WorkerClient::~WorkerClient()
// {
// 	emscripten_destroy_worker(m_workerHandle);
// }

void	WorkerClient::onMessageCallback(char* pData, int size, void* arg)
{
	WorkerClient*	pWorkerClient = static_cast<WorkerClient*>(arg);

	pWorkerClient->processMessage(pData, size);
}

void	WorkerClient::processMessage(char* pData, int dataLength)
{
	auto*	pMessage = reinterpret_cast<workerServerMessages::t_Message*>(pData);

	m_flags[e_Status::eProcessing] = false;

	switch (pMessage->type)
	{
		case workerServerMessages::eSimulationInitialised:
			D_MYLOG("worker initialised");
			m_flags[e_Status::eInitialised] = true;
			break;

		case workerServerMessages::eSimulationResult:
			memcpy(&m_latestResults, pData, dataLength);
			m_flags[e_Status::eUpdated] = true;
			break;

		default:
			D_MYLOG("WORKER message => Unknown");
	}
}

void	WorkerClient::sendMessage(char* pData, int size)
{
	m_flags[e_Status::eProcessing] = true;

	auto	callback = WorkerClient::onMessageCallback;

	emscripten_call_worker(m_workerHandle, D_WORKER_MAIN, pData, size, callback, (void*)this);
}

void	WorkerClient::initialiseSimulation()
{
	workerClientMessages::t_InitialiseSimulationMessage	message;

	sendMessage(reinterpret_cast<char*>(&message), sizeof(message));
}

void	WorkerClient::resetAndProcessSimulation(char* pData, int size)
{
	workerClientMessages::t_ResetAndProcessSimulationMessage	message;

	if (size != sizeof(message.neuralNetworkWeights))
	{
		D_MYLOG("wrong amount of data");
		return;
	}

	memcpy(message.neuralNetworkWeights, pData, size);

	sendMessage(reinterpret_cast<char*>(&message), sizeof(message));
}

void	WorkerClient::processSimulation()
{
	workerClientMessages::t_ProcessSimulationMessage	message;

	sendMessage(reinterpret_cast<char*>(&message), sizeof(message));
}
