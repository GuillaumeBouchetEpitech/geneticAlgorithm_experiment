
#include "demo/defines.hpp"

#include "WorkerConsumer.hpp"

#include "demo/utilities/TraceLogger.hpp"

#include <emscripten/emscripten.h> // <= EMSCRIPTEN_KEEPALIVE

namespace /* anonymous */
{
    WorkerConsumer* consumer = nullptr;
};

extern "C"
{

// hacky way to force the name of the function to be from the macro "D_WORKER_MAIN"
#define WORKER_MAIN_FUNC_NAME(func_name) _ ## func_name

EMSCRIPTEN_KEEPALIVE
void WORKER_MAIN_FUNC_NAME(D_WORKER_MAIN)(char* dataPointer, int dataSize, void* arg)
{
    static_cast<void>(arg); // <= unused

    if (!consumer)
        consumer = new WorkerConsumer();

    consumer->processMessage(dataPointer, dataSize);
}

}
