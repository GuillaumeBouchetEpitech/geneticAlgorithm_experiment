
#include "demo/defines.hpp"

#include "WorkerConsumer.hpp"

#include "demo/utilities/ErrorHandler.hpp"
// #include "demo/utilities/TraceLogger.hpp"

#include <memory> // std::unique_ptr
#include <chrono>

#include <emscripten/emscripten.h> // <= emscripten_worker_respond()

WorkerConsumer::WorkerConsumer()
{

    // m_contacts.reserve(32);
    // _physicWorld.setOnContact([this](const glm::vec3& position, const glm::vec3& normal) {

    //  m_contacts.push_back(std::make_pair(position, normal));
    // });

}

void    WorkerConsumer::processMessage(const char* dataPointer, int dataSize)
{
    MessageView receivedMsg(dataPointer, dataSize);

    char messageType = 0;
    receivedMsg >> messageType;

    switch (messages::client(messageType))
    {
        case messages::client::eLoadWorker:
        {
            _initialiseSimulation(receivedMsg);
            break;
        }

        case messages::client::eResetAndProcessSimulation:
        {
            _resetSimulation(receivedMsg);
            _processSimulation();
            break;
        }

        case messages::client::eProcessSimulation:
        {
            _processSimulation();
            break;
        }

        default:
        {
            D_THROW(std::runtime_error, "unknown worker message");
        }
    }
}

void    WorkerConsumer::_send()
{
    emscripten_worker_respond(const_cast<char*>(_message.getData()), _message.getSize());
}

void    WorkerConsumer::_initialiseSimulation(MessageView& receivedMsg)
{
    CircuitBuilder::t_startTransform startTransform;
    CircuitBuilder::t_knots circuitKnots;

    bool                        isUsingBias = true;
    unsigned int                layerInput = 0;
    unsigned int                totalHidden = 0;
    std::vector<unsigned int>   layerHidden;
    unsigned int                layerOutput = 0;

    { // read initialisation packet
        receivedMsg >> startTransform.position;
        receivedMsg >> startTransform.quaternion;

        int knotsLength = 0;
        receivedMsg >> knotsLength;

        circuitKnots.reserve(knotsLength); // <= pre-allocate

        for (int ii = 0; ii < knotsLength; ++ii)
        {
            CircuitBuilder::t_knot knot;

            receivedMsg >> knot.left >> knot.right >> knot.minDistance >> knot.color;

            circuitKnots.push_back(knot);
        }

        //
        //

        receivedMsg >> _genomesPerCore;

        // extract neural network topology
        receivedMsg >> isUsingBias;
        receivedMsg >> layerInput;
        receivedMsg >> totalHidden;
        for (unsigned int ii = 0; ii < totalHidden; ++ii)
        {
            unsigned int layerValue = 0;
            receivedMsg >> layerValue;

            layerHidden.push_back(layerValue);
        }
        receivedMsg >> layerOutput;

    } // read initialisation packet

    { // generate circuit

        int physicIndex = 0;

        auto onNewGroundPatch = [&](const CircuitBuilder::t_vec3Array& vertices,
                                    const CircuitBuilder::t_vec3Array& colors,
                                    const CircuitBuilder::t_vec3Array& normals,
                                    const CircuitBuilder::t_indices& indices) -> void {

            static_cast<void>(colors); // <= unused
            static_cast<void>(normals); // <= unused

            _physicWorld.createGround(vertices, indices, physicIndex++);
        };

        auto onNewWallPatch = [&](const CircuitBuilder::t_vec3Array& vertices,
                                const CircuitBuilder::t_vec3Array& colors,
                                const CircuitBuilder::t_vec3Array& normals,
                                const CircuitBuilder::t_indices& indices) -> void {

            static_cast<void>(colors); // <= unused
            static_cast<void>(normals); // <= unused

            _physicWorld.createWall(vertices, indices);
        };

        CircuitBuilder circuitBuilder;
        circuitBuilder.load(startTransform, circuitKnots);
        circuitBuilder.generate(onNewGroundPatch, onNewWallPatch);

        _startTransform = circuitBuilder.getStartTransform();

    } // generate circuit

    { // generate cars

        _cars.reserve(_genomesPerCore); // pre-allocate
        for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
            _cars.push_back(Car(_physicWorld,
                                _startTransform.position,
                                _startTransform.quaternion));

    } // generate cars

    { // generate neural networks

        _neuralNetworkTopology.init(layerInput, layerHidden, layerOutput, isUsingBias);

        _neuralNetworks.reserve(_genomesPerCore); // pre-allocate
        for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
            _neuralNetworks.push_back(NeuralNetwork(_neuralNetworkTopology));

    } // generate neural networks

    _message.clear();
    _message << char(messages::server::eWebWorkerLoaded);

    _send();
}

void    WorkerConsumer::_resetSimulation(MessageView& receivedMsg)
{
    const unsigned int floatWeightsSize = _neuralNetworkTopology.getTotalWeights();
    const unsigned int byteWeightsSize = floatWeightsSize * sizeof(float);

    auto newWeights = std::make_unique<float[]>(floatWeightsSize);
    float* newWeightsRaw = newWeights.get();

    std::vector<float>  weightsBuffer(floatWeightsSize);
    float* weightsBufferRaw = weightsBuffer.data();

    for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
    {
        receivedMsg.read(newWeightsRaw, byteWeightsSize);

        memcpy(weightsBufferRaw, newWeightsRaw, byteWeightsSize);
        _neuralNetworks[ii].setWeights(weightsBuffer);

        _cars[ii].reset(_startTransform.position, _startTransform.quaternion);
    }

    // m_contacts.clear();
}

void    WorkerConsumer::_processSimulation()
{
    // update the simulation

    auto start = std::chrono::high_resolution_clock::now();

    //
    //

    _physicWorld.step();

    unsigned int genomesAlive = 0;
    for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
    {
        auto& car = _cars[ii];

        if (!car.isAlive())
            continue;

        car.update(_neuralNetworks[ii]);

        if (_cars[ii].isAlive())
            ++genomesAlive;
    }

    //
    //

    auto finish = std::chrono::high_resolution_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    unsigned int delta = milliseconds.count();

    // send back the result

    _message.clear();
    _message << char(messages::server::eSimulationResult);

    _message << delta << genomesAlive;

    glm::mat4   transform;

    for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
    {
        const auto& car = _cars[ii];

        _message
            << car.isAlive()
            << car.getLife()
            << car.getFitness()
            << car.getTotalUpdates()
            << car.getGroundIndex();

        if (!car.isAlive())
            continue;

        const auto& vehicle = car.getVehicle();

        // record the transformation matrix of the car
        _message << vehicle.getOpenGLMatrix(transform);

        // record the transformation matrix of the wheels
        for (int jj = 0; jj < 4; ++jj)
            _message << vehicle.getWheelOpenGLMatrix(jj, transform);

        const auto& eyeSensors = car.getEyeSensors();
        for (const auto& sensor : eyeSensors)
            _message << sensor.near << sensor.far << sensor.value;

        const auto& gSensor = car.getGroundSensor();
        _message << gSensor.near << gSensor.far << gSensor.value;

        const auto& output = car.getNeuralNetworkOutput();
        _message << output.steer << output.speed;
    }

    _send();
}
