

#pragma once


#include "Simulation/Worker/workerMessages.hpp"

#include <bitset>

#include <emscripten/emscripten.h>


typedef workerServerMessages::t_SimulationResultMessage	t_Result;

class WorkerClient
{
private:
    worker_handle	m_workerHandle;

	enum e_Status
	{
		eInitialised = 0,
		eProcessing,
		eUpdated,
		eCount
	};

    std::bitset<e_Status::eCount>	m_flags;

    t_Result	m_latestResults;

public:
	WorkerClient();
	// ~WorkerClient();

private:
	static void	onMessageCallback(char* pData, int size, void* arg);

private:
	void	processMessage(char* pData, int dataLength);

private:
	void	sendMessage(char* pData, int size);

public:
	void	initialiseSimulation();
	void	resetAndProcessSimulation(char* pData, int size);
	void	processSimulation();

public:
	inline bool	isInitialised() const { return m_flags[e_Status::eInitialised]; }
	inline bool	isProcessing() const { return m_flags[e_Status::eProcessing]; }
	inline bool	isUpdated() const { return m_flags[e_Status::eUpdated]; }
	inline const t_Result&	getSimulationResults() const { return m_latestResults; }

};

