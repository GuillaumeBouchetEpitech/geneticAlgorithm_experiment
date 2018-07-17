

#include "WorkerServer.hpp"

#include "Utility/TraceLogger.hpp"

namespace /* anonymous */
{
	WorkerServer*	pWorkerServer = nullptr;
};

extern "C"
{
	void onMessage(char* pData, int dataLength, void* arg)
	{
		if (!pWorkerServer)
			pWorkerServer = new WorkerServer();

		pWorkerServer->processMessage(pData, dataLength);
	}
}
