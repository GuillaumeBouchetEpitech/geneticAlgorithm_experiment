

#include "WorkerConsumer.hpp"

#include "../../common.hpp"

#include "demo/defines.hpp"

#include "framework/ErrorHandler.hpp"

#include <memory> // std::make_unique
#include <chrono>

#include <emscripten/emscripten.h> // <= emscripten_worker_respond()

void WorkerConsumer::processMessage(const char* dataPointer, int dataSize)
{
    MessageView receivedMsg(dataPointer, dataSize);

    char messageType = 0;
    receivedMsg >> messageType;

    switch (Messages::FromProducer(messageType))
    {
        case Messages::FromProducer::LoadWorker:
        {
            _initialiseSimulation(receivedMsg);
            break;
        }

        case Messages::FromProducer::ResetAndProcessSimulation:
        {
            float elapsedTime;
            unsigned int totalSteps;
            receivedMsg >> elapsedTime >> totalSteps;

            _resetSimulation(receivedMsg);
            _processSimulation(elapsedTime, totalSteps);
            break;
        }

        case Messages::FromProducer::ProcessSimulation:
        {
            float elapsedTime;
            unsigned int totalSteps;
            receivedMsg >> elapsedTime >> totalSteps;

            _processSimulation(elapsedTime, totalSteps);
            break;
        }

        default:
        {
            D_THROW(std::runtime_error, "unknown worker message");
        }
    }
}

void WorkerConsumer::_sendBackToProducer()
{
    emscripten_worker_respond(const_cast<char*>(_messageToSend.getData()), _messageToSend.getSize());
}

void WorkerConsumer::_initialiseSimulation(MessageView& receivedMsg)
{
    CircuitBuilder::Knots circuitKnots;

    bool                        isUsingBias = true;
    unsigned int                layerInput = 0;
    std::vector<unsigned int>   layerHidden;
    unsigned int                layerOutput = 0;

    { // read initialisation packet

        receivedMsg >> _startTransform.position;
        receivedMsg >> _startTransform.quaternion;

        int knotsLength = 0;
        receivedMsg >> knotsLength;

        circuitKnots.reserve(knotsLength); // <= pre-allocate

        for (int ii = 0; ii < knotsLength; ++ii)
        {
            CircuitBuilder::Knot knot;

            receivedMsg >> knot.left >> knot.right >> knot.size >> knot.color;

            circuitKnots.emplace_back(knot);
        }

        //
        //

        receivedMsg >> _genomesPerCore;

        // extract neural network topology
        receivedMsg >> isUsingBias;
        receivedMsg >> layerInput;

        unsigned int totalHidden = 0;
        receivedMsg >> totalHidden;

        layerHidden.reserve(totalHidden);
        for (unsigned int ii = 0; ii < totalHidden; ++ii)
        {
            unsigned int layerValue = 0;
            receivedMsg >> layerValue;

            layerHidden.emplace_back(layerValue);
        }
        receivedMsg >> layerOutput;

    } // read initialisation packet

    { // generate circuit

        _circuitBuilder.load(_startTransform, circuitKnots);

        _resetPhysic();

    } // generate circuit

    { // generate cars

        _carAgents.resize(_genomesPerCore);

        _latestTransformsHistory.resize(_genomesPerCore);
        for (auto& transforms : _latestTransformsHistory)
            transforms.reserve(50); // TODO: hardcoded

    } // generate cars

    { // generate neural networks

        _neuralNetworkTopology.init(layerInput, layerHidden, layerOutput, isUsingBias);

        _neuralNetworks.reserve(_genomesPerCore); // pre-allocate
        for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
            _neuralNetworks.emplace_back(std::make_shared<NeuralNetwork>(_neuralNetworkTopology));

    } // generate neural networks

    _messageToSend.clear();
    _messageToSend << char(Messages::FromConsumer::WebWorkerLoaded);

    _sendBackToProducer();
}

void WorkerConsumer::_resetSimulation(MessageView& receivedMsg)
{
    _resetPhysic();

    const unsigned int floatWeightsSize = _neuralNetworkTopology.getTotalWeights();
    const unsigned int byteWeightsSize = floatWeightsSize * sizeof(float);

    auto newWeights = std::make_unique<float[]>(floatWeightsSize);
    float* newWeightsRaw = newWeights.get();

    std::vector<float> weightsBuffer(floatWeightsSize);
    float* weightsBufferRaw = weightsBuffer.data();

    for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
    {
        receivedMsg.read(newWeightsRaw, byteWeightsSize);

        std::memcpy(weightsBufferRaw, newWeightsRaw, byteWeightsSize);
        _neuralNetworks.at(ii)->setWeights(weightsBuffer);

        _carAgents.at(ii).reset(_physicWorld.get(), _startTransform.position, _startTransform.quaternion);
    }
}

void WorkerConsumer::_processSimulation(float elapsedTime, unsigned int totalSteps)
{
    // update the simulation

    const auto startTime = std::chrono::high_resolution_clock::now();

    //
    //

    for (auto& transformsHistory : _latestTransformsHistory)
        transformsHistory.clear();

    CarData::Transforms newData;

    for (unsigned int step = 0; step < totalSteps; ++step)
    {
        constexpr uint32_t maxSubSteps = 0;
        constexpr float fixedTimeStep = 1.0f / 30.0f;
        _physicWorld->step(elapsedTime, maxSubSteps, fixedTimeStep);

        for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
        {
            CarAgent& car = _carAgents.at(ii);

            if (!car.isAlive())
                continue;

            car.update(elapsedTime, _neuralNetworks.at(ii));

            {
                const auto body = car.getBody();
                const auto vehicle = car.getVehicle();

                // transformation matrix of the car
                body->getTransform(newData.chassis);

                // transformation matrix of the wheels
                for (unsigned int jj = 0; jj < 4U; ++jj)
                    vehicle->getWheelTransform(jj, newData.wheels.at(jj));

                _latestTransformsHistory.at(ii).push_back(newData);
            }
        }
    }

    //
    //

    const auto finishTime = std::chrono::high_resolution_clock::now();
    const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(finishTime - startTime);
    const unsigned int delta = milliseconds.count() * 1000;

    //
    //

    unsigned int genomesAlive = 0;
    for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
        if (_carAgents.at(ii).isAlive())
            ++genomesAlive;

    // send back the result

    _messageToSend.clear();
    _messageToSend << char(Messages::FromConsumer::SimulationResult);

    _messageToSend << delta << genomesAlive;

    glm::mat4 transform;

    for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
    {
        const CarAgent& car = _carAgents.at(ii);

        _messageToSend
            << car.isAlive()
            << car.getLife()
            << car.getFitness()
            << car.getTotalUpdates()
            << car.getGroundIndex();

        //
        //
        //

        auto& transformsHistory = _latestTransformsHistory.at(ii);

        _messageToSend << int(transformsHistory.size());
        for (const CarData::Transforms& tranforms : transformsHistory)
        {
            _messageToSend << tranforms.chassis;

            for (const glm::mat4& wheel : tranforms.wheels)
                _messageToSend << wheel;
        }

        //
        //
        //

        if (!car.isAlive())
            continue;

        const auto body = car.getBody();
        const auto vehicle = car.getVehicle();

        // record the transformation matrix of the car
        _messageToSend << body->getTransform(transform);

        // record the transformation matrix of the wheels
        for (int jj = 0; jj < 4; ++jj)
            _messageToSend << vehicle->getWheelTransform(jj, transform);

        _messageToSend << body->getLinearVelocity();

        const auto& eyeSensors = car.getEyeSensors();
        for (const auto& sensor : eyeSensors)
            _messageToSend << sensor.near << sensor.far << sensor.value;

        const auto& gSensor = car.getGroundSensor();
        _messageToSend << gSensor.near << gSensor.far << gSensor.value;

        const auto& output = car.getNeuralNetworkOutput();
        _messageToSend << output.steer << output.speed;
    }

    _sendBackToProducer();
}

void WorkerConsumer::_resetPhysic()
{
    _physicWorld = std::make_unique<PhysicWorld>();

    { // generate circuit

        int groundIndex = 0;

        auto onNewGroundPatch = [&](
            const CircuitBuilder::Vec3Array& vertices,
            const CircuitBuilder::Vec3Array& colors,
            const CircuitBuilder::Vec3Array& normals,
            const CircuitBuilder::Indices& indices) -> void
        {
            static_cast<void>(colors); // <= unused
            static_cast<void>(normals); // <= unused

            PhysicShapeDef shapeDef;
            shapeDef.type = PhysicShapeDef::Type::staticMesh;
            shapeDef.data.staticMesh.verticesData = const_cast<glm::vec3*>(vertices.data());
            shapeDef.data.staticMesh.verticesLength = vertices.size();
            shapeDef.data.staticMesh.indicesData = const_cast<int32_t*>(static_cast<const int32_t*>(indices.data()));
            shapeDef.data.staticMesh.indicesLength = indices.size();

            PhysicBodyDef bodyDef;
            bodyDef.shape = shapeDef;
            bodyDef.mass = 0.0f;
            bodyDef.group = asValue(Groups::ground);
            bodyDef.mask = asValue(Masks::ground);

            auto body = _physicWorld->getPhysicBodyManager().createAndAddBody(bodyDef);
            body->setFriction(1.0f);
            body->setUserValue(groundIndex++);
        };

        auto onNewWallPatch = [&](
            const CircuitBuilder::Vec3Array& vertices,
            const CircuitBuilder::Vec3Array& colors,
            const CircuitBuilder::Vec3Array& normals,
            const CircuitBuilder::Indices& indices) -> void
        {
            static_cast<void>(colors); // <= unused
            static_cast<void>(normals); // <= unused

            PhysicShapeDef shapeDef;
            shapeDef.type = PhysicShapeDef::Type::staticMesh;
            shapeDef.data.staticMesh.verticesData = const_cast<glm::vec3*>(vertices.data());
            shapeDef.data.staticMesh.verticesLength = vertices.size();
            shapeDef.data.staticMesh.indicesData = const_cast<int32_t*>(static_cast<const int32_t*>(indices.data()));
            shapeDef.data.staticMesh.indicesLength = indices.size();

            PhysicBodyDef bodyDef;
            bodyDef.shape = shapeDef;
            bodyDef.mass = 0.0f;
            bodyDef.group = asValue(Groups::wall);
            bodyDef.mask = asValue(Masks::wall);

            _physicWorld->getPhysicBodyManager().createAndAddBody(bodyDef);
        };

        _circuitBuilder.generateCircuitGeometry(onNewGroundPatch, onNewWallPatch);

    } // generate circuit

    { // generate floor

        PhysicShapeDef shapeDef;
        shapeDef.type = PhysicShapeDef::Type::box;
        shapeDef.data.box.size = { 1000, 1000, 0.5f};

        PhysicBodyDef bodyDef;
        bodyDef.shape = shapeDef;
        bodyDef.group = asValue(Groups::ground);
        bodyDef.mask = asValue(Masks::ground);
        bodyDef.mass = 0.0f;

        auto body = _physicWorld->getPhysicBodyManager().createAndAddBody(bodyDef);
        body->setPosition({ 0.0f, 0.0f, -0.5f });

    } // generate floor
}
