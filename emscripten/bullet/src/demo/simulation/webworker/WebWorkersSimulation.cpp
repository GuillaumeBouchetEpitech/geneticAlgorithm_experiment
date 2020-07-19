
#include "demo/defines.hpp"

#if not defined D_WEB_WEBWORKER_BUILD
#   error "exclude this file to build natively or with multi thread support"
#endif

#include "WebWorkersSimulation.hpp"

#include "demo/utilities/TraceLogger.hpp"

void WebWorkersSimulation::initialise(const t_def& def)
{
    CircuitBuilder circuit;
    circuit.load(def.filename);

    circuit.generateSkeleton(def.onSkeletonPatch);
    circuit.generate(def.onNewGroundPatch, def.onNewWallPatch);

    //

    _totalCores = def.totalCores;
    _genomesPerCore = def.genomesPerCore;
    _totalGenomes = _genomesPerCore * _totalCores;

    _carLiveStatus.resize(_totalGenomes, true);

    GeneticAlgorithm::t_def genAlgoDef;
    genAlgoDef.topology = def.neuralNetworkTopology;
    genAlgoDef.totalGenomes = _totalGenomes;

    _geneticAlgorithm.initialise(genAlgoDef);

    //

    WorkerProducer::t_def workerDef;
    // workerDef.filename = def.filename;
    workerDef.startTransform = circuit.getStartTransform();
    workerDef.knots = circuit.getKnots();
    workerDef.genomesPerCore = _genomesPerCore;
    workerDef.neuralNetworkTopology = def.neuralNetworkTopology;

    for (unsigned int ii = 0; ii < _totalCores; ++ii)
        _workerProducers.push_back(new WorkerProducer(workerDef));

    _currentRequest = WorkerRequest::eWorkersLoading;
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

    if (_currentRequest == WorkerRequest::eWorkersLoading)
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

    if (_currentRequest == WorkerRequest::eResetAndProcess)
    {
        if (_callbacks.onGenerationReset)
            _callbacks.onGenerationReset();
    }
    else if (_currentRequest == WorkerRequest::eProcess)
    {
        for (unsigned int ii = 0; ii < _totalGenomes; ++ii)
        {
            const auto& carResult = getCarResult(ii);

            _carLiveStatus.resize(_totalGenomes, true);

            if (!carResult.isAlive && _carLiveStatus[ii])
            {
                _carLiveStatus[ii] = false;

                if (_callbacks.onGenomeDie)
                    _callbacks.onGenomeDie(ii);
            }
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

        float extraFitness = float(carData.totalUpdates) / 1000;

        _geneticAlgorithm.rateGenome(ii, carData.fitness - extraFitness);
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

    _currentRequest = WorkerRequest::eProcess;
}

void WebWorkersSimulation::_resetAndProcessSimulation()
{
    const auto& NNetworks = _geneticAlgorithm.getNeuralNetworks();

    for (unsigned int ii = 0; ii < _workerProducers.size(); ++ii)
    {
        const auto& neuralNetworks = NNetworks.data() + ii * _genomesPerCore;

        _workerProducers[ii]->resetAndProcessSimulation(neuralNetworks);
    }

    _currentRequest = WorkerRequest::eResetAndProcess;
}

unsigned int WebWorkersSimulation::getTotalCores() const
{
    return _totalCores;
}

const AbstactSimulation::t_coreState& WebWorkersSimulation::getCoreState(unsigned int index) const
{
    return _workerProducers.at(index)->getCoreState();
}

const t_carData& WebWorkersSimulation::getCarResult(unsigned int index) const
{
    const unsigned int  workerIndex = index / _genomesPerCore;
    const unsigned int  carDataIndex = index % _genomesPerCore;

    const WorkerProducer*   workerProducer = _workerProducers.at(workerIndex);

    return workerProducer->getCarsData().at(carDataIndex);
}

unsigned int WebWorkersSimulation::getTotalCars() const
{
    return _totalGenomes;
}

void WebWorkersSimulation::setOnWorkersReadyCallback(AbstactSimulation::t_callback callback)
{
    _callbacks.onWorkersReady = callback;
}

void WebWorkersSimulation::setOnGenerationResetCallback(AbstactSimulation::t_callback callback)
{
    _callbacks.onGenerationReset = callback;
}

void WebWorkersSimulation::setOnGenerationStepCallback(AbstactSimulation::t_callback callback)
{
    _callbacks.onGenerationStep = callback;
}

void WebWorkersSimulation::setOnGenomeDieCallback(AbstactSimulation::t_genomeDieCallback callback)
{
    _callbacks.onGenomeDie = callback;
}

void WebWorkersSimulation::setOnGenerationEndCallback(AbstactSimulation::t_generationEndCallback callback)
{
    _callbacks.onGenerationEnd = callback;
}

const GeneticAlgorithm::t_genomes& WebWorkersSimulation::getGenomes() const
{
    return _geneticAlgorithm.getGenomes();
}

const GeneticAlgorithm::t_genome& WebWorkersSimulation::getBestGenome() const
{
    return _geneticAlgorithm.getBestGenome();
}

unsigned int WebWorkersSimulation::getGenerationNumber() const
{
    return _geneticAlgorithm.getGenerationNumber();
}
