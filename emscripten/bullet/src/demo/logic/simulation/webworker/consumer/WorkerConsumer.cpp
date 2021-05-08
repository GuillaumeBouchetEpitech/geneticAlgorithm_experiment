

#include "WorkerConsumer.hpp"

#include "demo/utilities/ErrorHandler.hpp"

#include "demo/defines.hpp"

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
            unsigned int totalSteps;
            receivedMsg >> totalSteps;

            _resetSimulation(receivedMsg);
            _processSimulation(totalSteps);
            break;
        }

        case Messages::FromProducer::ProcessSimulation:
        {
            unsigned int totalSteps;
            receivedMsg >> totalSteps;

            _processSimulation(totalSteps);
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

        int physicIndex = 0;

        auto onNewGroundPatch = [&](const CircuitBuilder::Vec3Array& vertices,
                                    const CircuitBuilder::Vec3Array& colors,
                                    const CircuitBuilder::Vec3Array& normals,
                                    const CircuitBuilder::Indices& indices) -> void {

            static_cast<void>(colors); // <= unused
            static_cast<void>(normals); // <= unused

            _physicWorld.createGround(vertices, indices, physicIndex++);
        };

        auto onNewWallPatch = [&](const CircuitBuilder::Vec3Array& vertices,
                                  const CircuitBuilder::Vec3Array& colors,
                                  const CircuitBuilder::Vec3Array& normals,
                                  const CircuitBuilder::Indices& indices) -> void {

            static_cast<void>(colors); // <= unused
            static_cast<void>(normals); // <= unused

            _physicWorld.createWall(vertices, indices);
        };

        CircuitBuilder circuitBuilder;
        circuitBuilder.load(_startTransform, circuitKnots);
        circuitBuilder.generate(onNewGroundPatch, onNewWallPatch);

    } // generate circuit

    { // generate cars

        _cars.reserve(_genomesPerCore); // pre-allocate
        for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
            _cars.emplace_back(Car(_physicWorld,
                                   _startTransform.position,
                                   _startTransform.quaternion));

        _latestTransformsHistory.resize(_genomesPerCore);
        for (auto& transforms : _latestTransformsHistory)
            transforms.reserve(50); // TODO: hardcoded

    } // generate cars

    { // generate neural networks

        _neuralNetworkTopology.init(layerInput, layerHidden, layerOutput, isUsingBias);

        _neuralNetworks.reserve(_genomesPerCore); // pre-allocate
        for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
            _neuralNetworks.emplace_back(NeuralNetwork(_neuralNetworkTopology));

    } // generate neural networks

    _messageToSend.clear();
    _messageToSend << char(Messages::FromConsumer::WebWorkerLoaded);

    _sendBackToProducer();
}

void WorkerConsumer::_resetSimulation(MessageView& receivedMsg)
{
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
        _neuralNetworks[ii].setWeights(weightsBuffer);

        _cars[ii].reset(_startTransform.position, _startTransform.quaternion);
    }
}

void WorkerConsumer::_processSimulation(unsigned int totalSteps)
{
    // update the simulation

    auto start = std::chrono::high_resolution_clock::now();

    //
    //

    for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
        _latestTransformsHistory[ii].clear();

    for (unsigned int step = 0; step < totalSteps; ++step)
    {
        _physicWorld.step();

        for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
        {
            auto& car = _cars[ii];

            if (!car.isAlive())
                continue;

            car.update(_neuralNetworks[ii]);

            {
                const auto& vehicle = car.getVehicle();

                CarData::Transforms newData;

                // transformation matrix of the car
                vehicle.getOpenGLMatrix(newData.chassis);

                // transformation matrix of the wheels
                for (unsigned int jj = 0; jj < 4; ++jj)
                    vehicle.getWheelOpenGLMatrix(jj, newData.wheels[jj]);

                _latestTransformsHistory[ii].push_back(newData);
            }
        }
    }

    unsigned int genomesAlive = 0;
    for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
        if (_cars[ii].isAlive())
            ++genomesAlive;

    //
    //

    auto finish = std::chrono::high_resolution_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    unsigned int delta = milliseconds.count() * 1000;

    // send back the result

    _messageToSend.clear();
    _messageToSend << char(Messages::FromConsumer::SimulationResult);

    _messageToSend << delta << genomesAlive;

    glm::mat4   transform;

    for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
    {
        const auto& car = _cars[ii];

        _messageToSend
            << car.isAlive()
            << car.getLife()
            << car.getFitness()
            << car.getTotalUpdates()
            << car.getGroundIndex();

        //
        //
        //

        auto& transformsHistory = _latestTransformsHistory[ii];

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

        const auto& vehicle = car.getVehicle();

        // record the transformation matrix of the car
        _messageToSend << vehicle.getOpenGLMatrix(transform);

        // record the transformation matrix of the wheels
        for (int jj = 0; jj < 4; ++jj)
            _messageToSend << vehicle.getWheelOpenGLMatrix(jj, transform);

        _messageToSend << vehicle.getVelocity();

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
