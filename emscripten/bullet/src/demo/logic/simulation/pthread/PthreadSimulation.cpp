
#include "PthreadSimulation.hpp"

#include "../common.hpp"

#include "framework/asValue.hpp"

#include "framework/ErrorHandler.hpp"
#include "framework/TraceLogger.hpp"

#include <chrono>

PthreadSimulation::~PthreadSimulation() {
  // the threads must be stopped before anything else is destroyed
  _multithreadProducer.quit();
}

void PthreadSimulation::initialise(const Definition& def) {
  if (def.genomesPerCore == 0)
    D_THROW(std::invalid_argument, "received invalid number of genomes per core"
                                     << ", input=" << def.genomesPerCore);

  if (def.totalCores == 0)
    D_THROW(std::invalid_argument, "received invalid number of cores"
                                     << ", input=" << def.totalCores);

  if (!def.neuralNetworkTopology.isValid())
    D_THROW(std::invalid_argument, "received invalid neural network topology");

  _totalCores = def.totalCores;
  _genomesPerCore = def.genomesPerCore;
  unsigned int totalGenomes = _genomesPerCore * _totalCores;

  GeneticAlgorithm::Definition genAlgoDef;
  genAlgoDef.topology = def.neuralNetworkTopology;
  genAlgoDef.totalGenomes = totalGenomes;

  _geneticAlgorithm.initialise(genAlgoDef);

  _multithreadProducer.initialise(_totalCores);

  _coreStates.resize(_totalCores);

  {

    auto onNewPhysicGroundPatch =
      [&](const CircuitBuilder::Vec3Array& vertices,
          const CircuitBuilder::Vec3Array& colors,
          const CircuitBuilder::Vec3Array& normals,
          const CircuitBuilder::Indices& indices) -> void {
      if (def.onNewGroundPatch)
        def.onNewGroundPatch(vertices, colors, normals, indices);
    };

    auto onNewPhysicWallPatch =
      [&](const CircuitBuilder::Vec3Array& vertices,
          const CircuitBuilder::Vec3Array& colors,
          const CircuitBuilder::Vec3Array& normals,
          const CircuitBuilder::Indices& indices) -> void {
      if (def.onNewWallPatch)
        def.onNewWallPatch(vertices, colors, normals, indices);
    };

    _circuitBuilder.parse(def.filename);
    _circuitBuilder.generateWireframeSkeleton(def.onSkeletonPatch);
    _circuitBuilder.generateCircuitGeometry(onNewPhysicGroundPatch,
                                            onNewPhysicWallPatch);
    _startTransform = _circuitBuilder.getStartTransform();
  }

  _resetPhysic();

  _carAgents.resize(totalGenomes);

  _carsData.resize(totalGenomes);
  for (auto& carData : _carsData)
    carData.latestTransformsHistory.reserve(50); // TODO: hardcoded (o_o)

  for (std::size_t ii = 0; ii < _carAgents.size(); ++ii) {
    const std::size_t worldIndex = ii / _genomesPerCore;

    _carAgents.at(ii).reset(_physicWorlds.at(worldIndex).get(),
                            _startTransform.position,
                            _startTransform.quaternion);
  }
}

void PthreadSimulation::_resetPhysic() {
  _physicWorlds.clear();
  _physicWorlds.resize(_totalCores);
  for (unsigned int ii = 0; ii < _totalCores; ++ii) {
    _physicWorlds.at(ii) = std::make_unique<PhysicWorld>();

    { // generate circuit

      int groundIndex = 0;
      auto onNewPhysicGroundPatch =
        [&](const CircuitBuilder::Vec3Array& vertices,
            const CircuitBuilder::Vec3Array& colors,
            const CircuitBuilder::Vec3Array& normals,
            const CircuitBuilder::Indices& indices) -> void {
        static_cast<void>(colors);  // <= unused
        static_cast<void>(normals); // <= unused

        PhysicShapeDef shapeDef;
        shapeDef.type = PhysicShapeDef::Type::staticMesh;
        shapeDef.data.staticMesh.verticesData =
          const_cast<glm::vec3*>(vertices.data());
        shapeDef.data.staticMesh.verticesLength = vertices.size();
        shapeDef.data.staticMesh.indicesData =
          const_cast<int32_t*>(static_cast<const int32_t*>(indices.data()));
        shapeDef.data.staticMesh.indicesLength = indices.size();

        PhysicBodyDef bodyDef;
        bodyDef.shape = shapeDef;
        bodyDef.mass = 0.0f;
        bodyDef.group = asValue(Groups::ground);
        bodyDef.mask = asValue(Masks::ground);

        auto body =
          _physicWorlds.at(ii)->getPhysicBodyManager().createAndAddBody(
            bodyDef);
        body->setFriction(1.0f);
        body->setUserValue(groundIndex);

        groundIndex++;
      };

      auto onNewPhysicWallPatch =
        [&](const CircuitBuilder::Vec3Array& vertices,
            const CircuitBuilder::Vec3Array& colors,
            const CircuitBuilder::Vec3Array& normals,
            const CircuitBuilder::Indices& indices) -> void {
        static_cast<void>(colors);  // <= unused
        static_cast<void>(normals); // <= unused

        PhysicShapeDef shapeDef;
        shapeDef.type = PhysicShapeDef::Type::staticMesh;
        shapeDef.data.staticMesh.verticesData =
          const_cast<glm::vec3*>(vertices.data());
        shapeDef.data.staticMesh.verticesLength = vertices.size();
        shapeDef.data.staticMesh.indicesData =
          const_cast<int32_t*>(static_cast<const int32_t*>(indices.data()));
        shapeDef.data.staticMesh.indicesLength = indices.size();

        PhysicBodyDef bodyDef;
        bodyDef.shape = shapeDef;
        bodyDef.mass = 0.0f;
        bodyDef.group = asValue(Groups::wall);
        bodyDef.mask = asValue(Masks::wall);

        _physicWorlds.at(ii)->getPhysicBodyManager().createAndAddBody(bodyDef);
      };

      _circuitBuilder.generateCircuitGeometry(onNewPhysicGroundPatch,
                                              onNewPhysicWallPatch);

    } // generate circuit

    { // floor

      PhysicShapeDef shapeDef;
      shapeDef.type = PhysicShapeDef::Type::box;
      shapeDef.data.box.size = {1000, 1000, 0.5f};

      PhysicBodyDef bodyDef;
      bodyDef.shape = shapeDef;
      bodyDef.mass = 0.0f;
      bodyDef.group = asValue(Groups::ground);
      bodyDef.mask = asValue(Masks::ground);

      auto body =
        _physicWorlds.at(ii)->getPhysicBodyManager().createAndAddBody(bodyDef);
      body->setPosition({0.0f, 0.0f, -0.5f});

    } // floor
  }
}

void PthreadSimulation::update(float elapsedTime, unsigned int totalSteps) {
  if (!_multithreadProducer.allCompleted())
    return;

  //
  //
  //

  _updateCarResult();

  if (_isFirstGenerationFrame) {
    _isFirstGenerationFrame = false;

    if (_callbacks.onResetAndProcess)
      _callbacks.onResetAndProcess();
  } else {
    if (_callbacks.onProcessStep)
      _callbacks.onProcessStep();
  }

  if (isGenerationComplete())
    return;

  for (std::size_t ii = 0; ii < _carsData.size(); ++ii)
    _carsData.at(ii).latestTransformsHistory.clear();

  for (std::size_t threadIndex = 0; threadIndex < _physicWorlds.size();
       ++threadIndex) {
    auto taskCallback = [this, threadIndex, elapsedTime, totalSteps]() -> void {
      auto start = std::chrono::high_resolution_clock::now();

      //
      //

      const auto& neuralNets = _geneticAlgorithm.getNeuralNetworks();

      for (unsigned int step = 0; step < totalSteps; ++step) {
        // update physical world

        constexpr uint32_t maxSubSteps = 0;
        constexpr float fixedTimeStep = 1.0f / 30.0f;
        _physicWorlds.at(threadIndex)
          ->step(fixedTimeStep, maxSubSteps, fixedTimeStep);

        // update cars

        for (unsigned int ii = 0; ii < _genomesPerCore; ++ii) {
          unsigned int index = threadIndex * _genomesPerCore + ii;

          auto& car = _carAgents.at(index);

          if (!car.isAlive())
            continue;

          car.update(elapsedTime, neuralNets.at(index));

          {
            const auto body = car.getBody();
            const auto vehicle = car.getVehicle();

            CarData::Transforms newData;

            // transformation matrix of the car
            body->getTransform(newData.chassis);

            // transformation matrix of the wheels
            for (unsigned int jj = 0; jj < 4; ++jj)
              vehicle->getWheelTransform(jj, newData.wheels.at(jj));

            _carsData.at(index).latestTransformsHistory.push_back(newData);
          }
        }
      }

      auto& coreState = _coreStates.at(threadIndex);
      coreState.genomesAlive = 0;

      for (unsigned int ii = 0; ii < _genomesPerCore; ++ii) {
        unsigned int index = threadIndex * _genomesPerCore + ii;

        auto& car = _carAgents.at(index);

        if (car.isAlive())
          coreState.genomesAlive++;
      }

      //
      //

      auto finish = std::chrono::high_resolution_clock::now();
      auto microseconds =
        std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
      coreState.delta = microseconds.count();
    };

    _multithreadProducer.push(taskCallback);
  }

  // #if not defined D_NATIVE_PTHREAD_BUILD
  //     _multithreadProducer.waitUntilAllCompleted();
  // #endif
}

void PthreadSimulation::breed() {
  if (!_multithreadProducer.allCompleted())
    return;
  if (!isGenerationComplete())
    return;

  // rate genomes
  for (std::size_t ii = 0; ii < _carsData.size(); ++ii) {
    // this penalty reward fast cars (reaching farther in less updates)
    const float fitnessPenalty = float(_carsData.at(ii).totalUpdates) / 10000;

    _geneticAlgorithm.rateGenome(ii, _carsData.at(ii).fitness - fitnessPenalty);
  }

  const bool isSmarter = _geneticAlgorithm.breedPopulation();

  if (_callbacks.onGenerationEnd)
    _callbacks.onGenerationEnd(isSmarter);

  //
  //

  _updateCarResult();

  _resetPhysic();

  for (std::size_t ii = 0; ii < _carAgents.size(); ++ii) {
    const std::size_t worldIndex = ii / _genomesPerCore;

    _carAgents.at(ii).reset(_physicWorlds.at(worldIndex).get(),
                            _startTransform.position,
                            _startTransform.quaternion);
  }

  // _updateCarResult();

  _isFirstGenerationFrame = true;
}

bool PthreadSimulation::isGenerationComplete() const {
  for (const auto& car : _carAgents)
    if (car.isAlive())
      return false;
  return true;
}

void PthreadSimulation::_updateCarResult() {
  for (std::size_t ii = 0; ii < _carAgents.size(); ++ii) {
    const auto& carAgent = _carAgents.at(ii);
    auto& carData = _carsData.at(ii);

    bool carWasAlive = carData.isAlive;
    carData.isAlive = carAgent.isAlive();
    carData.life = carAgent.getLife();
    carData.fitness = carAgent.getFitness();
    carData.totalUpdates = carAgent.getTotalUpdates();
    carData.groundIndex = carAgent.getGroundIndex();

    if (carWasAlive && !carData.isAlive) {
      if (_callbacks.onGenomeDie)
        _callbacks.onGenomeDie(ii);
    }

    if (!carData.isAlive) {
      carData.latestTransformsHistory.clear();
      continue;
    }

    const auto body = carAgent.getBody();
    const auto vehicle = carAgent.getVehicle();

    // transformation matrix of the car
    body->getTransform(carData.liveTransforms.chassis);

    // transformation matrix of the wheels
    for (std::size_t jj = 0; jj < carData.liveTransforms.wheels.size(); ++jj)
      vehicle->getWheelTransform(jj, carData.liveTransforms.wheels.at(jj));

    carData.velocity = body->getLinearVelocity();

    const auto& eyeSensors = carAgent.getEyeSensors();
    for (std::size_t jj = 0; jj < eyeSensors.size(); ++jj) {
      carData.eyeSensors.at(jj).near = eyeSensors.at(jj).near;
      carData.eyeSensors.at(jj).far = eyeSensors.at(jj).far;
      carData.eyeSensors.at(jj).value = eyeSensors.at(jj).value;
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

unsigned int PthreadSimulation::getTotalCores() const { return _totalCores; }

const AbstactSimulation::CoreState&
PthreadSimulation::getCoreState(unsigned int index) const {
  return _coreStates.at(index);
}

const CarData& PthreadSimulation::getCarResult(unsigned int index) const {
  return _carsData.at(index);
}

unsigned int PthreadSimulation::getTotalCars() const {
  return _carsData.size();
}

void PthreadSimulation::setOnGenerationResetCallback(
  AbstactSimulation::SimpleCallback callback) {
  _callbacks.onResetAndProcess = callback;
}

void PthreadSimulation::setOnGenerationStepCallback(
  AbstactSimulation::SimpleCallback callback) {
  _callbacks.onProcessStep = callback;
}

void PthreadSimulation::setOnGenomeDieCallback(
  AbstactSimulation::GenomeDieCallback callback) {
  _callbacks.onGenomeDie = callback;
}

void PthreadSimulation::setOnGenerationEndCallback(
  AbstactSimulation::GenerationEndCallback callback) {
  _callbacks.onGenerationEnd = callback;
}

const Genomes& PthreadSimulation::getGenomes() const {
  return _geneticAlgorithm.getGenomes();
}

const Genome& PthreadSimulation::getBestGenome() const {
  return _geneticAlgorithm.getBestGenome();
}

unsigned int PthreadSimulation::getGenerationNumber() const {
  return _geneticAlgorithm.getGenerationNumber();
}

const glm::vec3& PthreadSimulation::getStartPosition() const {
  return _startTransform.position;
}

const GeneticAlgorithm& PthreadSimulation::getGeneticAlgorithm() const {
  return _geneticAlgorithm;
}
