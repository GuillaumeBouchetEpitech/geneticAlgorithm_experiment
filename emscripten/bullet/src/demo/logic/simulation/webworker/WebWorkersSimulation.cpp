
#include "demo/defines.hpp"

#include "WebWorkersSimulation.hpp"

#include "demo/utilities/ErrorHandler.hpp"
#include "demo/utilities/TraceLogger.hpp"

void WebWorkersSimulation::initialise(const Definition& def)
{
    if (def.genomesPerCore == 0)
        D_THROW(std::invalid_argument,
                "received invalid number of genomes per core"
                << ", input=" << def.genomesPerCore);

    if (def.totalCores == 0)
        D_THROW(std::invalid_argument,
                "received invalid number of cores"
                << ", input=" << def.totalCores);


    if (!def.neuralNetworkTopology.isValid())
        D_THROW(std::invalid_argument,
                "received invalid neural network topology");

    CircuitBuilder circuit;
    circuit.load(def.filename);

    circuit.generateSkeleton(def.onSkeletonPatch);
    circuit.generate(def.onNewGroundPatch, def.onNewWallPatch);

    //

    _totalCores = def.totalCores;
    _genomesPerCore = def.genomesPerCore;
    _totalGenomes = _genomesPerCore * _totalCores;

    _carLiveStatus.resize(_totalGenomes, true);

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
    for (unsigned int ii = 0; ii < _totalCores; ++ii)
        _workerProducers.emplace_back(new WorkerProducer(workerDef));

    _currentRequest = WorkerRequest::WorkersLoading;
}

void WebWorkersSimulation::update()
{
    // do nothing if the worker(s) are:
    // => not initialised
    // => not finished working
    for (auto* workerProducer : _workerProducers)
        if (!workerProducer->isLoaded() || workerProducer->isProcessing())
            return;

    // if this line is reached it mean the worker(s) are now available

    if (_currentRequest == WorkerRequest::WorkersLoading)
    {
        // ask to reset if any worker(s) are not yet updated
        // => it should only happen the first time

        if (_callbacks.onWorkersReady)
            _callbacks.onWorkersReady();

        _resetAndProcessSimulation();
        return;
    }

    bool incompleteSimulation = false;

    for (unsigned int ii = 0; ii < _totalGenomes; ++ii)
    {
        const auto& carResult = getCarResult(ii);

        if (!carResult.isAlive)
            continue;

        incompleteSimulation = true;
    }

    if (_currentRequest == WorkerRequest::ResetAndProcess)
    {
        if (_callbacks.onGenerationReset)
            _callbacks.onGenerationReset();
    }
    else if (_currentRequest == WorkerRequest::Process)
    {
        for (unsigned int ii = 0; ii < _totalGenomes; ++ii)
        {
            const auto& carResult = getCarResult(ii);

            if (carResult.isAlive == _carLiveStatus[ii])
                continue;

            _carLiveStatus[ii] = false;

            if (_callbacks.onGenomeDie)
                _callbacks.onGenomeDie(ii);
        }

        if (_callbacks.onGenerationStep)
            _callbacks.onGenerationStep();
    }

    if (incompleteSimulation)
    {
        // ask the worker(s) to process/update the (physic) simulation
        _processSimulation();
        return;
    }

    for (unsigned int ii = 0; ii < _totalGenomes; ++ii)
    {
        const auto& carData = getCarResult(ii);

        // this penalty reward fast cars (reaching farther in less updates)
        const float fitnessPenalty = float(carData.totalUpdates) / 10000;

        _geneticAlgorithm.rateGenome(ii, carData.fitness - fitnessPenalty);
    }

    bool isSmarter = _geneticAlgorithm.breedPopulation();

    if (_callbacks.onGenerationEnd)
        _callbacks.onGenerationEnd(isSmarter);

    // reset cars status
    _carLiveStatus.assign(_totalGenomes, true);

    // ask the worker(s) to reset the (physic) simulation
    _resetAndProcessSimulation();
}

void WebWorkersSimulation::_processSimulation()
{
    for (auto* workerProducer : _workerProducers)
        workerProducer->processSimulation();

    _currentRequest = WorkerRequest::Process;
}

void WebWorkersSimulation::_resetAndProcessSimulation()
{
    const auto& NNetworks = _geneticAlgorithm.getNeuralNetworks();

    for (unsigned int ii = 0; ii < _workerProducers.size(); ++ii)
    {
        const auto* neuralNetworks = NNetworks.data() + ii * _genomesPerCore;

        _workerProducers[ii]->resetAndProcessSimulation(neuralNetworks);
    }

    _currentRequest = WorkerRequest::ResetAndProcess;
}

unsigned int WebWorkersSimulation::getTotalCores() const
{
    return _totalCores;
}

const AbstactSimulation::CoreState& WebWorkersSimulation::getCoreState(unsigned int index) const
{
    return _workerProducers.at(index)->getCoreState();
}

const CarData& WebWorkersSimulation::getCarResult(unsigned int index) const
{
    const unsigned int workerIndex = index / _genomesPerCore;
    const unsigned int carDataIndex = index % _genomesPerCore;

    return _workerProducers.at(workerIndex)->getCarsData().at(carDataIndex);
}

unsigned int WebWorkersSimulation::getTotalCars() const
{
    return _totalGenomes;
}

void WebWorkersSimulation::setOnWorkersReadyCallback(AbstactSimulation::SimpleCallback callback)
{
    _callbacks.onWorkersReady = callback;
}

void WebWorkersSimulation::setOnGenerationResetCallback(AbstactSimulation::SimpleCallback callback)
{
    _callbacks.onGenerationReset = callback;
}

void WebWorkersSimulation::setOnGenerationStepCallback(AbstactSimulation::SimpleCallback callback)
{
    _callbacks.onGenerationStep = callback;
}

void WebWorkersSimulation::setOnGenomeDieCallback(AbstactSimulation::GenomeDieCallback callback)
{
    _callbacks.onGenomeDie = callback;
}

void WebWorkersSimulation::setOnGenerationEndCallback(AbstactSimulation::GenerationEndCallback callback)
{
    _callbacks.onGenerationEnd = callback;
}

const Genomes& WebWorkersSimulation::getGenomes() const
{
    return _geneticAlgorithm.getGenomes();
}

const Genome& WebWorkersSimulation::getBestGenome() const
{
    return _geneticAlgorithm.getBestGenome();
}

unsigned int WebWorkersSimulation::getGenerationNumber() const
{
    return _geneticAlgorithm.getGenerationNumber();
}

const glm::vec3& WebWorkersSimulation::getStartPosition() const
{
    return _startPosition;
}
