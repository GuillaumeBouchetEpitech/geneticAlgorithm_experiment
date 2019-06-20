
#include "demo/defines.hpp"

#if not defined D_WEB_WEBWORKER_BUILD
#	error "exclude this file to build natively or with multi thread support"
#endif

#include "WorkerConsumer.hpp"

#include "demo/utilities/TraceLogger.hpp"

#include <emscripten/emscripten.h> // <= EMSCRIPTEN_KEEPALIVE

namespace /* anonymous */
{
	WorkerConsumer*	workerConsumer = nullptr;
};

extern "C"
{

EMSCRIPTEN_KEEPALIVE
void onMessage(char* dataPointer, int dataSize, void* arg)
{
	static_cast<void>(arg); // <= unused

	if (!workerConsumer)
		workerConsumer = new WorkerConsumer();

	workerConsumer->processMessage(dataPointer, dataSize);
}

}
