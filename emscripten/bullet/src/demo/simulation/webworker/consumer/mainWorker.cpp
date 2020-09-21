
#include "demo/defines.hpp"

#include "WorkerConsumer.hpp"

#include "../preprocessing.hpp"

#include "demo/utilities/TraceLogger.hpp"

#include <emscripten/emscripten.h> // <= EMSCRIPTEN_KEEPALIVE

namespace /* anonymous */
{
    WorkerConsumer* consumer = nullptr;
};

extern "C"
{

EMSCRIPTEN_KEEPALIVE
void WORKER_MAIN_FUNC(char* dataPointer, int dataSize, void* arg)
{
    static_cast<void>(arg); // <= unused

    if (!consumer)
        consumer = new WorkerConsumer();

    consumer->processMessage(dataPointer, dataSize);
}

}
