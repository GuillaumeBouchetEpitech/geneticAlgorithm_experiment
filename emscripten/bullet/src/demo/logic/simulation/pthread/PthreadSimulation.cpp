
#include "PthreadSimulation.hpp"

#include "demo/utilities/ErrorHandler.hpp"
#include "demo/utilities/TraceLogger.hpp"

#include <chrono>

void PthreadSimulation::initialise(const Definition& def)
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

    _totalCores = def.totalCores;
    _genomesPerCore = def.genomesPerCore;
    unsigned int totalGenomes = _genomesPerCore * _totalCores;

    GeneticAlgorithm::Definition genAlgoDef;
    genAlgoDef.topology = def.neuralNetworkTopology;
    genAlgoDef.totalGenomes = totalGenomes;

    _geneticAlgorithm.initialise(genAlgoDef);

    _multithreadProducer = new multithreading::Producer(_totalCores);

    _physicWorlds.resize(_totalCores);
    for (unsigned int ii = 0; ii < _totalCores; ++ii)
        _physicWorlds[ii] = new PhysicWorld();

    _coreStates.resize(_totalCores);

    { // generate circuit

        int groundIndex = 0;
        auto onNewPhysicGroundPatch = [&](const CircuitBuilder::Vec3Array& vertices,
                                          const CircuitBuilder::Vec3Array& colors,
                                          const CircuitBuilder::Vec3Array& normals,
                                          const CircuitBuilder::Indices& indices) -> void {

            static_cast<void>(colors); // <= unused
            static_cast<void>(normals); // <= unused

            for (auto& physicWorld : _physicWorlds)
                physicWorld->createGround(vertices, indices, groundIndex);
            groundIndex++;

            if (def.onNewGroundPatch)
                def.onNewGroundPatch(vertices, colors, normals, indices);
        };

        auto onNewPhysicWallPatch = [&](const CircuitBuilder::Vec3Array& vertices,
                                        const CircuitBuilder::Vec3Array& colors,
                                        const CircuitBuilder::Vec3Array& normals,
                                        const CircuitBuilder::Indices& indices) -> void {

            static_cast<void>(colors); // <= unused
            static_cast<void>(normals); // <= unused

            for (auto& physicWorld : _physicWorlds)
                physicWorld->createWall(vertices, indices);

            if (def.onNewWallPatch)
                def.onNewWallPatch(vertices, colors, normals, indices);
        };

        CircuitBuilder circuitBuilder;
        circuitBuilder.load(def.filename);
        circuitBuilder.generateSkeleton(def.onSkeletonPatch);
        circuitBuilder.generate(onNewPhysicGroundPatch, onNewPhysicWallPatch);

        _startTransform = circuitBuilder.getStartTransform();

    } // generate circuit

    { // generate cars

        _cars.reserve(totalGenomes); // pre-allocate
        for (auto& physicWorld : _physicWorlds)
            for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
                _cars.push_back(Car(*physicWorld,
                                    _startTransform.position,
                                    _startTransform.quaternion));

    } // generate cars

    _carsData.resize(totalGenomes);
}

void PthreadSimulation::update()
{
    for (unsigned int threadIndex = 0; threadIndex < _physicWorlds.size(); ++threadIndex)
    {
        auto taskCallback = [this, threadIndex]() -> void
        {
            auto start = std::chrono::high_resolution_clock::now();

            //
            //

            auto& coreState = _coreStates[threadIndex];
            coreState.genomesAlive = 0;

            // update physical world

            _physicWorlds[threadIndex]->step();

            // update cars

            const auto& neuralNets = _geneticAlgorithm.getNeuralNetworks();

            for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
            {
                unsigned int index = threadIndex * _genomesPerCore + ii;

                auto& car = _cars[index];

                if (!car.isAlive())
                    continue;

                car.update(neuralNets[index]);
                coreState.genomesAlive++;
            }

            //
            //

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

    _updateCarResult();

    if (_isFirstGenerationFrame)
    {
        _isFirstGenerationFrame = false;

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
        // this penalty reward fast cars (reaching farther in less updates)
        const float fitnessPenalty = float(_carsData[ii].totalUpdates) / 10000;

        _geneticAlgorithm.rateGenome(ii, _carsData[ii].fitness - fitnessPenalty);
    }

    bool isSmarter = _geneticAlgorithm.breedPopulation();

    if (_callbacks.onGenerationEnd)
        _callbacks.onGenerationEnd(isSmarter);

    //
    //

    for (Car& car : _cars)
        car.reset(_startTransform.position, _startTransform.quaternion);

    _isFirstGenerationFrame = true;
}

void PthreadSimulation::_updateCarResult()
{
    for (unsigned int ii = 0; ii < _cars.size(); ++ii)
    {
        const auto& car = _cars[ii];
        auto& carData = _carsData[ii];

        bool carWasAlive = carData.isAlive;
        carData.isAlive = car.isAlive();
        carData.life = car.getLife();
        carData.fitness = car.getFitness();
        carData.totalUpdates = car.getTotalUpdates();
        carData.groundIndex = car.getGroundIndex();

        if (carWasAlive && !carData.isAlive)
        {
            if (_callbacks.onGenomeDie)
                _callbacks.onGenomeDie(ii);
        }

        if (!carData.isAlive)
            continue;

        const auto& vehicle = car.getVehicle();

        // transformation matrix of the car
        vehicle.getOpenGLMatrix(carData.transform);

        // transformation matrix of the wheels
        for (unsigned int jj = 0; jj < 4; ++jj)
            vehicle.getWheelOpenGLMatrix(jj, carData.wheelsTransform[jj]);

        carData.velocity = vehicle.getVelocity();

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

const AbstactSimulation::CoreState& PthreadSimulation::getCoreState(unsigned int index) const
{
    return _coreStates.at(index);
}

const CarData& PthreadSimulation::getCarResult(unsigned int index) const
{
    return _carsData.at(index);
}

unsigned int PthreadSimulation::getTotalCars() const
{
    return _carsData.size();
}

void PthreadSimulation::setOnGenerationResetCallback(AbstactSimulation::SimpleCallback callback)
{
    _callbacks.onResetAndProcess = callback;
}

void PthreadSimulation::setOnGenerationStepCallback(AbstactSimulation::SimpleCallback callback)
{
    _callbacks.onProcessStep = callback;
}

void PthreadSimulation::setOnGenomeDieCallback(AbstactSimulation::GenomeDieCallback callback)
{
    _callbacks.onGenomeDie = callback;
}

void PthreadSimulation::setOnGenerationEndCallback(AbstactSimulation::GenerationEndCallback callback)
{
    _callbacks.onGenerationEnd = callback;
}

const Genomes& PthreadSimulation::getGenomes() const
{
    return _geneticAlgorithm.getGenomes();
}

const Genome& PthreadSimulation::getBestGenome() const
{
    return _geneticAlgorithm.getBestGenome();
}

unsigned int PthreadSimulation::getGenerationNumber() const
{
    return _geneticAlgorithm.getGenerationNumber();
}

const glm::vec3& PthreadSimulation::getStartPosition() const
{
    return _startTransform.position;
}
