
#include "PthreadSimulation.hpp"

#include "demo/utilities/ErrorHandler.hpp"
#include "demo/utilities/TraceLogger.hpp"

#include <chrono>

PthreadSimulation::~PthreadSimulation()
{
}

void PthreadSimulation::initialise(const t_def& def)
{
    if (def.genomesPerCore == 0)
        D_THROW(std::invalid_argument,
                "received invalid number of genomes per core"
                << ", input=" << def.genomesPerCore);

    if (def.totalCores == 0)
        D_THROW(std::invalid_argument,
                "received invalid number of cores"
                << ", input=" << def.totalCores);

    _totalCores = def.totalCores;
    _genomesPerCore = def.genomesPerCore;
    unsigned int totalGenomes = _genomesPerCore * _totalCores;

    GeneticAlgorithm::t_def genAlgoDef;
    genAlgoDef.topology = def.neuralNetworkTopology;
    genAlgoDef.totalGenomes = totalGenomes;

    _geneticAlgorithm.initialise(genAlgoDef);

    _multithreadProducer = new Producer(_totalCores);

    _physicWorlds.resize(_totalCores);
    _coreStates.resize(_totalCores);

    {
        // generateCircuit()

        int groundIndex = 0;
        auto onNewPhysicGroundPatch = [&](const CircuitBuilder::t_vec3Array& vertices,
                                          const CircuitBuilder::t_vec3Array& colors,
                                          const CircuitBuilder::t_vec3Array& normals,
                                          const CircuitBuilder::t_indices& indices) -> void {

            static_cast<void>(colors); // <= unused
            static_cast<void>(normals); // <= unused

            for (auto& physicWorld : _physicWorlds)
                physicWorld.createGround(vertices, indices, groundIndex);
            groundIndex++;

            if (def.onNewGroundPatch)
                def.onNewGroundPatch(vertices, colors, normals, indices);
        };

        auto onNewPhysicWallPatch = [&](const CircuitBuilder::t_vec3Array& vertices,
                                        const CircuitBuilder::t_vec3Array& colors,
                                        const CircuitBuilder::t_vec3Array& normals,
                                        const CircuitBuilder::t_indices& indices) -> void {

            static_cast<void>(colors); // <= unused
            static_cast<void>(normals); // <= unused

            for (auto& physicWorld : _physicWorlds)
                physicWorld.createWall(vertices, indices);

            if (def.onNewWallPatch)
                def.onNewWallPatch(vertices, colors, normals, indices);
        };

        CircuitBuilder  circuitBuilder;
        circuitBuilder.load(def.filename);
        circuitBuilder.generateSkeleton(def.onSkeletonPatch);
        circuitBuilder.generate(onNewPhysicGroundPatch, onNewPhysicWallPatch);

        _startTransform = circuitBuilder.getStartTransform();
    }

    {
        // generateCars()

        _cars.reserve(totalGenomes);
        for (auto& physicWorld : _physicWorlds)
            for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
                _cars.push_back(Car(physicWorld,
                                    _startTransform.position,
                                    _startTransform.quaternion));
    }

    _carsData.resize(totalGenomes);
}

void PthreadSimulation::update()
{
    const unsigned int carPerThread = _genomesPerCore;

    for (unsigned int threadIndex = 0; threadIndex < _physicWorlds.size(); ++threadIndex)
    {
        auto taskCallback = [this, threadIndex, carPerThread]() -> void {

            auto start = std::chrono::high_resolution_clock::now();

            auto& coreState = _coreStates[threadIndex];
            coreState.genomesAlive = 0;

            _physicWorlds[threadIndex].step();

            auto& neuralNets = _geneticAlgorithm.getNeuralNetworks();

            for (unsigned int jj = 0; jj < carPerThread; ++jj)
            {
                unsigned int index = threadIndex * carPerThread + jj;
                auto& car = _cars[index];

                if (!car.isAlive())
                    continue;

                car.update(neuralNets[index]);
                coreState.genomesAlive++;
            }

            auto finish = std::chrono::high_resolution_clock::now();
            auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
            coreState.delta = microseconds.count();
        };

        _multithreadProducer->push(taskCallback);
    }

    _multithreadProducer->waitUntilAllCompleted();

    // for (unsigned int ii = 0; ii < _coreStates.size(); ++ii)
    //     D_MYLOG(" => [" << ii << "] = " << _coreStates[ii].delta << " (" << _coreStates[ii].genomesAlive << ")");

    bool generationCompleted = true;

    for (const auto& car : _cars)
        if (car.isAlive())
        {
            generationCompleted = false;
            break;
        }

    updateCarResult();

    if (_isFirstFrame)
    {
        _isFirstFrame = false;

        if (_callbacks.onResetAndProcess)
            _callbacks.onResetAndProcess();
    }
    else
    {
        if (_callbacks.onProcessStep)
            _callbacks.onProcessStep();
    }

    if (!generationCompleted)
        return;

    // rate genomes
    for (unsigned int ii = 0; ii < _carsData.size(); ++ii)
    {
        float extraFitness = float(_carsData[ii].totalUpdates) / 1000;

        _geneticAlgorithm.rateGenome(ii, _carsData[ii].fitness + extraFitness);
    }

    bool isSmarter = _geneticAlgorithm.breedPopulation();

    if (_callbacks.onGenerationEnd)
        _callbacks.onGenerationEnd(isSmarter);

    //
    //

    // for (auto& physicWorld : _physicWorlds)
    //     physicWorld.reset();

    for (Car& car : _cars)
        car.reset(_startTransform.position, _startTransform.quaternion);

    // for (auto& physicWorld : _physicWorlds)
    //     physicWorld.removeAll();

    // for (Car& car : _cars)
    //     car.reset();

    // for (auto& physicWorld : _physicWorlds)
    //     physicWorld.reinsertAll();

    _isFirstFrame = true;
}

void PthreadSimulation::updateCarResult()
{
    for (unsigned int ii = 0; ii < _cars.size(); ++ii)
    {
        const auto& car = _cars[ii];
        auto& carData = _carsData[ii];

        carData.isAlive = car.isAlive();
        carData.life = car.getLife();
        carData.fitness = car.getFitness();
        carData.totalUpdates= car.getTotalUpdates();
        carData.groundIndex = car.getGroundIndex();

        if (!carData.isAlive)
            continue;

        const auto& vehicle = car.getVehicle();

        // transformation matrix of the car
        vehicle.getOpenGLMatrix(carData.transform);

        // transformation matrix of the wheels
        for (unsigned int jj = 0; jj < 4; ++jj)
            vehicle.getWheelOpenGLMatrix(jj, carData.wheelsTransform[jj]);

        const auto& eyeSensors = car.getEyeSensors();
        for (unsigned int jj = 0; jj < eyeSensors.size(); ++jj)
        {
            carData.eyeSensors[jj].near = eyeSensors[jj].near;
            carData.eyeSensors[jj].far = eyeSensors[jj].far;
            carData.eyeSensors[jj].value = eyeSensors[jj].value;
        }

        const auto& gSensor = car.getGroundSensor();
        carData.groundSensor.near = gSensor.near;
        carData.groundSensor.far = gSensor.far;
        carData.groundSensor.value = gSensor.value;

        const auto& output = car.getNeuralNetworkOutput();
        carData.neuralNetworkOutput.speed = output.speed;
        carData.neuralNetworkOutput.steer = output.steer;
    }
}

unsigned int PthreadSimulation::getTotalCores() const
{
    return _totalCores;
}

const AbstactSimulation::t_coreState& PthreadSimulation::getCoreState(unsigned int index) const
{
    return _coreStates.at(index);
}

const t_carData& PthreadSimulation::getCarResult(unsigned int index) const
{
    return _carsData.at(index);
}

unsigned int PthreadSimulation::getTotalCars() const
{
    return _carsData.size();
}

void PthreadSimulation::setOnGenerationResetCallback(AbstactSimulation::t_callback callback)
{
    _callbacks.onResetAndProcess = callback;
}

void PthreadSimulation::setOnGenerationStepCallback(AbstactSimulation::t_callback callback)
{
    _callbacks.onProcessStep = callback;
}

void PthreadSimulation::setOnGenerationEndCallback(AbstactSimulation::t_generationEndCallback callback)
{
    _callbacks.onGenerationEnd = callback;
}

const GeneticAlgorithm::t_genomes& PthreadSimulation::getGenomes() const
{
    return _geneticAlgorithm.getGenomes();
}

const GeneticAlgorithm::t_genome& PthreadSimulation::getBestGenome() const
{
    return _geneticAlgorithm.getBestGenome();
}

unsigned int PthreadSimulation::getGenerationNumber() const
{
    return _geneticAlgorithm.getGenerationNumber();
}
