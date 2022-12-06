
#pragma once

namespace Messages {
enum class FromProducer : int {
  LoadWorker = 0,
  ResetAndProcessSimulation,
  ProcessSimulation,
  Unknown,
};

enum class FromConsumer : int {
  WebWorkerLoaded = 0,
  SimulationResult,
  Unknown
};
}; // namespace Messages
