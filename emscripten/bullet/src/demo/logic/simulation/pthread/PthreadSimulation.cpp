
#include "PthreadSimulation.hpp"

#include "demo/utilities/ErrorHandler.hpp"
#include "demo/utilities/TraceLogger.hpp"

#include <chrono>

PthreadSimulation::~PthreadSimulation()
{
    // the threads must be stopped before anything else is destroyed
    _multithreadProducer.quit();
}

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

    _multithreadProducer.initialise(_totalCores);

    _physicWorlds.resize(_totalCores);
    for (unsigned int ii = 0; ii < _totalCores; ++ii)
        _physicWorlds[ii] = std::make_unique<PhysicWorld>();

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

        _carAgents.reserve(totalGenomes); // pre-allocate
        for (auto& physicWorld : _physicWorlds)
        {
            for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
            {
                _carAgents.push_back(CarAgent(*physicWorld, _startTransform.position, _startTransform.quaternion));
            }
        }

    } // generate cars

    _carsData.resize(totalGenomes);
    for (auto& carData : _carsData)
        carData.latestTransformsHistory.reserve(50); // TODO: hardcoded (o_o)

    for (CarAgent& car : _carAgents)
        car.reset(_startTransform.position, _startTransform.quaternion);
}

void PthreadSimulation::update(float elapsedTime, unsigned int totalSteps)
{
    if (!_multithreadProducer.allCompleted())
        return;

    //
    //
    //

    bool generationCompleted = true;

    for (const auto& car : _carAgents)
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

    if (generationCompleted)
    {
        // rate genomes
        for (std::size_t ii = 0; ii < _carsData.size(); ++ii)
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

        for (std::size_t threadIndex = 0; threadIndex < _physicWorlds.size(); ++threadIndex)
            _physicWorlds[threadIndex]->reset();

        for (CarAgent& car : _carAgents)
            car.reset(_startTransform.position, _startTransform.quaternion);

        _isFirstGenerationFrame = true;
    }

    for (std::size_t ii = 0; ii < _carsData.size(); ++ii)
        _carsData[ii].latestTransformsHistory.clear();

    for (std::size_t threadIndex = 0; threadIndex < _physicWorlds.size(); ++threadIndex)
    {
        auto taskCallback = [this, threadIndex, elapsedTime, totalSteps]() -> void
        {
            auto start = std::chrono::high_resolution_clock::now();

            //
            //

            const auto& neuralNets = _geneticAlgorithm.getNeuralNetworks();

            for (unsigned int step = 0; step < totalSteps; ++step)
            {
                // update physical world

                _physicWorlds[threadIndex]->step(elapsedTime);

                // update cars

                for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
                {
                    unsigned int index = threadIndex * _genomesPerCore + ii;

                    auto& car = _carAgents[index];

                    if (!car.isAlive())
                        continue;

                    car.update(elapsedTime, neuralNets[index]);

                    {
                        const auto& vehicle = car.getVehicle();

                        CarData::Transforms newData;

                        // transformation matrix of the car
                        vehicle.getOpenGLMatrix(newData.chassis);

                        // transformation matrix of the wheels
                        for (unsigned int jj = 0; jj < 4; ++jj)
                            vehicle.getWheelOpenGLMatrix(jj, newData.wheels[jj]);

                        _carsData[index].latestTransformsHistory.push_back(newData);
                    }
                }
            }

            auto& coreState = _coreStates[threadIndex];
            coreState.genomesAlive = 0;

            for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
            {
                unsigned int index = threadIndex * _genomesPerCore + ii;

                auto& car = _carAgents[index];

                if (car.isAlive())
                    coreState.genomesAlive++;
            }

            //
            //

            auto finish = std::chrono::high_resolution_clock::now();
            auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
            coreState.delta = microseconds.count();
        };

        _multithreadProducer.push(taskCallback);
    }

// #if not defined D_NATIVE_PTHREAD_BUILD
//     _multithreadProducer.waitUntilAllCompleted();
// #endif
}

void PthreadSimulation::_updateCarResult()
{
    for (std::size_t ii = 0; ii < _carAgents.size(); ++ii)
    {
        const auto& carAgent = _carAgents[ii];
        auto& carData = _carsData[ii];

        bool carWasAlive = carData.isAlive;
        carData.isAlive = carAgent.isAlive();
        carData.life = carAgent.getLife();
        carData.fitness = carAgent.getFitness();
        carData.totalUpdates = carAgent.getTotalUpdates();
        carData.groundIndex = carAgent.getGroundIndex();

        if (carWasAlive && !carData.isAlive)
        {
            if (_callbacks.onGenomeDie)
                _callbacks.onGenomeDie(ii);
        }

        if (!carData.isAlive)
            continue;

        const auto& vehicle = carAgent.getVehicle();

        // transformation matrix of the car
        vehicle.getOpenGLMatrix(carData.liveTransforms.chassis);

        // transformation matrix of the wheels
        for (std::size_t jj = 0; jj < carData.liveTransforms.wheels.size(); ++jj)
            vehicle.getWheelOpenGLMatrix(jj, carData.liveTransforms.wheels[jj]);

        carData.velocity = vehicle.getVelocity();

        const auto& eyeSensors = carAgent.getEyeSensors();
        for (std::size_t jj = 0; jj < eyeSensors.size(); ++jj)
        {
            carData.eyeSensors[jj].near = eyeSensors[jj].near;
            carData.eyeSensors[jj].far = eyeSensors[jj].far;
            carData.eyeSensors[jj].value = eyeSensors[jj].value;
        }

        const auto& gSensor = carAgent.getGroundSensor();
        carData.groundSensor.near = gSensor.near;
        carData.groundSensor.far = gSensor.far;
        carData.groundSensor.value = gSensor.value;

        const auto& output = carAgent.getNeuralNetworkOutput();
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

const GeneticAlgorithm& PthreadSimulation::getGeneticAlgorithm() const
{
    return _geneticAlgorithm;
}
