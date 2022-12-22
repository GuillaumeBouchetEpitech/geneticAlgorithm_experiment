
#include "WebWorkersSimulation.hpp"

#include "framework/system/ErrorHandler.hpp"
#include "framework/system/TraceLogger.hpp"

#include "demo/defines.hpp"

void WebWorkersSimulation::initialise(const Definition& def) {
  if (def.genomesPerCore == 0)
    D_THROW(std::invalid_argument, "received invalid number of genomes per core"
                                     << ", input=" << def.genomesPerCore);

  if (def.totalCores == 0)
    D_THROW(std::invalid_argument, "received invalid number of cores"
                                     << ", input=" << def.totalCores);

  if (!def.neuralNetworkTopology.isValid())
    D_THROW(std::invalid_argument, "received invalid neural network topology");

  CircuitBuilder circuit;
  circuit.parse(def.filename);

  circuit.generateWireframeSkeleton(def.onSkeletonPatch);
  circuit.generateCircuitGeometry(def.onNewGroundPatch, def.onNewWallPatch);

  //

  _totalCores = def.totalCores;
  _genomesPerCore = def.genomesPerCore;
  _totalGenomes = _genomesPerCore * _totalCores;

  _synchronisedCarsData.resize(_totalGenomes);

  GeneticAlgorithm::Definition genAlgoDef;
  genAlgoDef.topology = def.neuralNetworkTopology;
  genAlgoDef.totalGenomes = _totalGenomes;

  _geneticAlgorithm.initialise(genAlgoDef);

  //

  _startPosition = circuit.getStartTransform().position;

  WorkerProducer::Definition workerDef;
  workerDef.startTransform = circuit.getStartTransform();
  workerDef.knots = circuit.getKnots();
  workerDef.genomesPerCore = _genomesPerCore;
  workerDef.neuralNetworkTopology = def.neuralNetworkTopology;

  _workerProducers.reserve(_totalCores);
  for (uint32_t coreIndex = 0; coreIndex < _totalCores; ++coreIndex)
    _workerProducers.emplace_back(std::make_shared<WorkerProducer>(
      workerDef, _geneticAlgorithm, coreIndex));

  _currentRequest = WorkerRequest::WorkersLoading;
}

void WebWorkersSimulation::update(float elapsedTime, unsigned int totalSteps) {
  // do nothing if the worker(s) are:
  // => not initialised
  // => not finished working
  for (const auto& workerProducer : _workerProducers)
    if (!workerProducer->isLoaded() || workerProducer->isProcessing())
      return;

  // if this line is reached it mean the worker(s) are now available

  if (_currentRequest == WorkerRequest::WorkersLoading) {
    // ask to reset if any worker(s) are not yet updated
    // => it should only happen the first time

    if (_callbacks.onWorkersReady)
      _callbacks.onWorkersReady();

    _resetAndProcessSimulation(elapsedTime, totalSteps);
    return;
  }

  for (unsigned int index = 0; index < _totalGenomes; ++index) {
    const unsigned int workerIndex = index / _genomesPerCore;
    const unsigned int carDataIndex = index % _genomesPerCore;

    const auto& latestData =
      _workerProducers.at(workerIndex)->getCarsData().at(carDataIndex);
    auto& oldData = _synchronisedCarsData.at(index);

    if (oldData.isAlive && !latestData.isAlive) {
      if (_callbacks.onGenomeDie)
        _callbacks.onGenomeDie(index);
    }

    oldData = latestData;
  }
  _wasSynchronised = true;

  if (_currentRequest == WorkerRequest::ResetAndProcess) {
    if (_callbacks.onGenerationReset)
      _callbacks.onGenerationReset();
  } else if (_currentRequest == WorkerRequest::Process) {
    if (_callbacks.onGenerationStep)
      _callbacks.onGenerationStep();
  }

  if (isGenerationComplete())
    return;

  // ask the worker(s) to process/update the (physic) simulation
  _processSimulation(elapsedTime, totalSteps);
}

void WebWorkersSimulation::breed() {
  if (!isGenerationComplete())
    return;

  for (unsigned int ii = 0; ii < _totalGenomes; ++ii) {
    const auto& carData = getCarResult(ii);

    // this penalty reward fast cars (reaching farther in less updates)
    const float fitnessPenalty = float(carData.totalUpdates) / 10000;

    _geneticAlgorithm.rateGenome(ii, carData.fitness - fitnessPenalty);
  }

  bool isSmarter = _geneticAlgorithm.breedPopulation();

  if (_callbacks.onGenerationEnd)
    _callbacks.onGenerationEnd(isSmarter);

  // ask the worker(s) to reset the (physic) simulation
  const float elapsedTime = 0.0f;
  const unsigned int totalSteps = 1;
  _resetAndProcessSimulation(elapsedTime, totalSteps);
}

bool WebWorkersSimulation::isGenerationComplete() const {

  if (!_wasSynchronised)
    return false;

  for (unsigned int ii = 0; ii < _totalGenomes; ++ii)
    if (getCarResult(ii).isAlive)
      return false;

  return true;
}

void WebWorkersSimulation::_processSimulation(float elapsedTime,
                                              unsigned int totalSteps) {
  for (auto workerProducer : _workerProducers)
    workerProducer->processSimulation(elapsedTime, totalSteps);

  _currentRequest = WorkerRequest::Process;
}

void WebWorkersSimulation::_resetAndProcessSimulation(float elapsedTime,
                                                      unsigned int totalSteps) {
  const auto& NNetworks = _geneticAlgorithm.getNeuralNetworks();

  for (std::size_t ii = 0; ii < _workerProducers.size(); ++ii) {
    auto first = NNetworks.begin() + (ii + 0) * _genomesPerCore;
    auto last = NNetworks.begin() + (ii + 1) * _genomesPerCore;
    const NeuralNetworks subNetwork(first, last);

    _workerProducers.at(ii)->resetAndProcessSimulation(elapsedTime, totalSteps,
                                                       subNetwork);
  }

  _wasSynchronised = false;
  _currentRequest = WorkerRequest::ResetAndProcess;
}

unsigned int WebWorkersSimulation::getTotalCores() const { return _totalCores; }

const AbstactSimulation::CoreState&
WebWorkersSimulation::getCoreState(unsigned int index) const {
  return _workerProducers.at(index)->getCoreState();
}

const CarData& WebWorkersSimulation::getCarResult(unsigned int index) const {
  return _synchronisedCarsData.at(index);
}

unsigned int WebWorkersSimulation::getTotalCars() const {
  return _totalGenomes;
}

void WebWorkersSimulation::setOnWorkersReadyCallback(
  AbstactSimulation::SimpleCallback callback) {
  _callbacks.onWorkersReady = callback;
}

void WebWorkersSimulation::setOnGenerationResetCallback(
  AbstactSimulation::SimpleCallback callback) {
  _callbacks.onGenerationReset = callback;
}

void WebWorkersSimulation::setOnGenerationStepCallback(
  AbstactSimulation::SimpleCallback callback) {
  _callbacks.onGenerationStep = callback;
}

void WebWorkersSimulation::setOnGenomeDieCallback(
  AbstactSimulation::GenomeDieCallback callback) {
  _callbacks.onGenomeDie = callback;
}

void WebWorkersSimulation::setOnGenerationEndCallback(
  AbstactSimulation::GenerationEndCallback callback) {
  _callbacks.onGenerationEnd = callback;
}

const Genomes& WebWorkersSimulation::getGenomes() const {
  return _geneticAlgorithm.getGenomes();
}

const Genome& WebWorkersSimulation::getBestGenome() const {
  return _geneticAlgorithm.getBestGenome();
}

unsigned int WebWorkersSimulation::getGenerationNumber() const {
  return _geneticAlgorithm.getGenerationNumber();
}

const glm::vec3& WebWorkersSimulation::getStartPosition() const {
  return _startPosition;
}

const GeneticAlgorithm& WebWorkersSimulation::getGeneticAlgorithm() const {
  return _geneticAlgorithm;
}
