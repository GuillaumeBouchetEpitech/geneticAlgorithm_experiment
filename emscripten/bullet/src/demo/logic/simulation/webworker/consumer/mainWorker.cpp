
#include "WorkerConsumer.hpp"

#include "demo/logic/simulation/webworker/preprocessing.hpp"

#include <emscripten/emscripten.h> // <= EMSCRIPTEN_KEEPALIVE

namespace /* anonymous */
{
WorkerConsumer* consumer = nullptr;
};

extern "C" {

EMSCRIPTEN_KEEPALIVE
void
D_WORKER_MAIN_FUNC(char* dataPointer, int dataSize, void* arg) {
  static_cast<void>(arg); // <= unused

  if (!consumer)
    consumer = new WorkerConsumer();

  consumer->processMessage(dataPointer, dataSize);
}
}
