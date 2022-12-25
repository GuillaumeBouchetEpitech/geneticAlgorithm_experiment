
#include "WorkerProducer.hpp"

#include "demo/logic/simulation/webworker/preprocessing.hpp"

#include "framework/system/ErrorHandler.hpp"
#include "framework/system/TraceLogger.hpp"

#include "demo/defines.hpp"

WorkerProducer::WorkerProducer(const Definition& def,
                               GeneticAlgorithm& geneticAlgorithm,
                               uint32_t coreIndex)
  : _def(def), _geneticAlgorithm(geneticAlgorithm), _coreIndex(coreIndex) {
  _workerHandle = emscripten_create_worker(D_WORKER_SCRIPT_URL);

  _flags[asValue(Status::WebWorkerLoaded)] = false;
  _flags[asValue(Status::Processing)] = false;
  _flags[asValue(Status::Updated)] = false;

  _carsData.resize(def.genomesPerCore);

  { // send intiialisation message to worker consumer

    _message.clear();
    _message << char(Messages::FromProducer::LoadWorker);

    _message << def.startTransform.position;
    _message << def.startTransform.quaternion;

    _message << int(def.knots.size());

    for (const auto& knot : def.knots)
      _message << knot.left << knot.right << knot.size << knot.color;

    _message << def.genomesPerCore;

    const auto& topology = def.neuralNetworkTopology;
    const auto& hiddenLayers = topology.getHiddens();

    _message << topology.isUsingBias();
    _message << topology.getInput();
    _message << int(hiddenLayers.size());
    for (unsigned int layerValue : hiddenLayers)
      _message << layerValue;
    _message << topology.getOutput();

    _sendToConsumer();

  } // send intiialisation message to worker consumer
}

void WorkerProducer::_onMessageCallback(char* dataPointer, int dataSize,
                                        void* arg) {
  WorkerProducer* self = static_cast<WorkerProducer*>(arg);

  self->_processMessage(dataPointer, dataSize);
}

void WorkerProducer::_processMessage(const char* dataPointer, int dataSize) {
  _flags[asValue(Status::Processing)] = false;

  MessageView receivedMsg(dataPointer, dataSize);

  char messageType = 0;
  receivedMsg >> messageType;

  switch (Messages::FromConsumer(messageType)) {
  case Messages::FromConsumer::WebWorkerLoaded: {
    D_MYLOG("web worker loaded");
    _flags[asValue(Status::WebWorkerLoaded)] = true;
    break;
  }

  case Messages::FromConsumer::SimulationResult: {
    receivedMsg >> _coreState.delta >> _coreState.genomesAlive;

    std::vector<float> neuronsValues;

    // for (auto& car : _carsData)
    for (uint32_t carIndex = 0; carIndex < _carsData.size(); ++carIndex) {
      auto& car = _carsData.at(carIndex);

      receivedMsg >> car.isAlive >> car.life >> car.fitness >>
        car.totalUpdates >> car.groundIndex;

      //
      //
      //

      int totalHistory = 0;
      receivedMsg >> totalHistory;

      car.latestTransformsHistory.clear();
      car.latestTransformsHistory.reserve(totalHistory);

      CarData::Transforms newData;
      for (int ii = 0; ii < totalHistory; ++ii) {
        receivedMsg >> newData.chassis;
        for (std::size_t jj = 0; jj < newData.wheels.size(); ++jj)
          receivedMsg >> newData.wheels.at(jj);

        car.latestTransformsHistory.push_back(newData);
      }

      //
      //
      //

      if (!car.isAlive)
        continue;

      receivedMsg >> car.liveTransforms.chassis;
      for (auto& transform : car.liveTransforms.wheels)
        receivedMsg >> transform;

      receivedMsg >> car.velocity;

      for (auto& sensor : car.eyeSensors)
        receivedMsg >> sensor.near >> sensor.far >> sensor.value;

      auto& gSensor = car.groundSensor;
      receivedMsg >> gSensor.near >> gSensor.far >> gSensor.value;

      auto& output = car.neuralNetworkOutput;
      receivedMsg >> output.steer >> output.speed;

      std::size_t totalNeuronsValues;
      receivedMsg >> totalNeuronsValues;
      neuronsValues.clear();
      neuronsValues.resize(totalNeuronsValues);
      for (std::size_t neuronIndex = 0; neuronIndex < totalNeuronsValues;
           ++neuronIndex)
        receivedMsg >> neuronsValues.at(neuronIndex);

      _geneticAlgorithm.getNeuralNetworks()
        .at(_coreIndex * _def.genomesPerCore + carIndex)
        ->setNeuronsValues(neuronsValues);
    }

    _flags[asValue(Status::Updated)] = true;
    break;
  }

  default: {
    D_THROW(std::runtime_error, "unknown worker message");
  }
  }
}

void WorkerProducer::_sendToConsumer() {
  _flags[asValue(Status::Processing)] = true;

  char* dataPointer = const_cast<char*>(_message.getData());
  unsigned int dataSize = _message.getSize();

  em_worker_callback_func callback = WorkerProducer::_onMessageCallback;

  emscripten_call_worker(_workerHandle, D_WORKER_MAIN_STR, dataPointer,
                         dataSize, callback, (void*)this);
}

void WorkerProducer::resetAndProcessSimulation(
  float elapsedTime, unsigned int totalSteps,
  const NeuralNetworks& neuralNetworks) {
  _message.clear();
  _message << char(Messages::FromProducer::ResetAndProcessSimulation);
  _message << elapsedTime;
  _message << totalSteps;

  std::vector<float> weights;

  for (std::size_t ii = 0; ii < _carsData.size(); ++ii) {
    neuralNetworks.at(ii)->getWeights(weights);

    _message.append(weights.data(), weights.size() * sizeof(float));
  }

  _sendToConsumer();
}

void WorkerProducer::processSimulation(float elapsedTime,
                                       unsigned int totalSteps) {
  _message.clear();
  _message << char(Messages::FromProducer::ProcessSimulation);
  _message << elapsedTime;
  _message << totalSteps;

  _sendToConsumer();
}

bool WorkerProducer::isLoaded() const {
  return _flags[asValue(Status::WebWorkerLoaded)];
}

bool WorkerProducer::isProcessing() const {
  return _flags[asValue(Status::Processing)];
}

bool WorkerProducer::isUpdated() const {
  return _flags[asValue(Status::Updated)];
}

const CarDatas& WorkerProducer::getCarsData() const { return _carsData; }

const AbstactSimulation::CoreState& WorkerProducer::getCoreState() const {
  return _coreState;
}
