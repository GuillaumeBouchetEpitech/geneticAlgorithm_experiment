
#include "demo/defines.hpp"

#if not defined D_WEB_WEBWORKER_BUILD
#	error "exclude this file to build natively or with multi thread support"
#endif

#include "WorkerConsumer.hpp"

#include "demo/simulation/logic/CircuitBuilder.hpp"
#include "demo/simulation/logic/Car.hpp"

#include "demo/logic/physic/PhysicVehicle.hpp"

#include "demo/utilities/ErrorHandler.hpp"
// #include "demo/utilities/TraceLogger.hpp"

#include "demo/constants.hpp"

#include <memory> // std::unique_ptr

#include <emscripten/emscripten.h> // <= emscripten_worker_respond()

WorkerConsumer::WorkerConsumer()
{

    // m_contacts.reserve(32);
    // _physicWorld.setOnContact([this](const glm::vec3& position, const glm::vec3& normal) {

    // 	m_contacts.push_back(std::make_pair(position, normal));
    // });

}

void	WorkerConsumer::processMessage(const char* dataPointer, int dataSize)
{
    MessageView	message(dataPointer, dataSize);

    char messageType = 0;
    message >> messageType;

    switch (messages::client(messageType))
    {
        case messages::client::eLoadWorker:
        {
            initialiseSimulation(message);
            break;
        }

        case messages::client::eResetAndProcessSimulation:
        {
            resetSimulation(message);
            processSimulation();
            break;
        }

        case messages::client::eProcessSimulation:
        {
            processSimulation();
            break;
        }

        default:
        {
            D_THROW(std::runtime_error, "unknown worker message");
        }
    }
}

void	WorkerConsumer::send()
{
    emscripten_worker_respond(const_cast<char*>(_message.getData()), _message.getSize());
}

void	WorkerConsumer::initialiseSimulation(MessageView& message)
{
    std::string circuitFilename;
    message >> circuitFilename;

    message >> _genomesPerCore;

    // extract neural network topology

    bool						isUsingBias = true;
    unsigned int				layerInput = 0;
    unsigned int				totalHidden = 0;
    std::vector<unsigned int> 	layerHidden;
    unsigned int				layerOutput = 0;

    message >> isUsingBias;

    message >> layerInput;

    if (isUsingBias)
        --layerInput;

    message >> totalHidden;
    for (unsigned int ii = 0; ii < totalHidden; ++ii)
    {
        unsigned int layerValue = 0;
        message >> layerValue;

        if (isUsingBias)
            --layerValue;

        layerHidden.push_back(layerValue);
    }
    message >> layerOutput;

    {
        int physicIndex = 0;

        auto onNewGroundPatch = [&](const CircuitBuilder::t_vertices& vertices,
                                    const CircuitBuilder::t_colors& colors,
                                    const CircuitBuilder::t_normals& normals,
                                    const CircuitBuilder::t_indices& indices) -> void {

            static_cast<void>(colors); // <= unused
            static_cast<void>(normals); // <= unused

            _physicWorld.createGround(vertices, indices, physicIndex++);
        };

        auto onNewWallPatch = [&](const CircuitBuilder::t_vertices& vertices,
                                const CircuitBuilder::t_colors& colors,
                                const CircuitBuilder::t_normals& normals,
                                const CircuitBuilder::t_indices& indices) -> void {

            static_cast<void>(colors); // <= unused
            static_cast<void>(normals); // <= unused

            _physicWorld.createWall(vertices, indices);
        };

        CircuitBuilder circuitBuilder;
        circuitBuilder.load(circuitFilename);
        circuitBuilder.generate(onNewGroundPatch, onNewWallPatch);
    }

    {
        _cars.reserve(_genomesPerCore);
        for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
            _cars.push_back(Car(_physicWorld));
    }

    {
        _neuralNetworkTopology.init(layerInput, layerHidden, layerOutput, isUsingBias);

        _neuralNetworks.reserve(_genomesPerCore);
        for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
            _neuralNetworks.push_back(NeuralNetwork(_neuralNetworkTopology));
    }

    _message.clear();
    _message << char(messages::server::eWebWorkerLoaded);

    send();
}

void	WorkerConsumer::resetSimulation(MessageView& message)
{
    const unsigned int	floatWeightsSize = _neuralNetworkTopology.getTotalWeights();
    const unsigned int	byteWeightsSize = floatWeightsSize * sizeof(float);

    auto newWeights = std::make_unique<float[]>(floatWeightsSize);
    float* newWeightsRaw = newWeights.get();

    std::vector<float>	weightsBuffer(floatWeightsSize);
    float*	weightsBufferRaw = weightsBuffer.data();

    for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
    {
        message.read(newWeightsRaw, byteWeightsSize);

        memcpy(weightsBufferRaw, newWeightsRaw, byteWeightsSize);
        _neuralNetworks[ii].setWeights(weightsBuffer);

        _cars[ii].reset();
    }

    // m_contacts.clear();
}

void	WorkerConsumer::processSimulation()
{
    // update the simulation

    _physicWorld.step();

    for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
    {
        auto& car = _cars[ii];

        if (!car.isAlive())
            continue;

        car.update(_neuralNetworks[ii]);
    }

    // send back the result

    _message.clear();
    _message << char(messages::server::eSimulationResult);

    glm::mat4	transform;

    const auto& vehicles = _physicWorld.getVehicles();

    for (unsigned int ii = 0; ii < _genomesPerCore; ++ii)
    {
        const auto& car = _cars[ii];

        _message << car.isAlive() << car.getFitness() << car.getGroundIndex();

        if (!car.isAlive())
            continue;

        auto* vehicle = vehicles.at(ii);

        // record the transformation matrix of the car
        _message << vehicle->getOpenGLMatrix(transform);

        // record the transformation matrix of the wheels
        for (int jj = 0; jj < 4; ++jj)
            _message << vehicle->getWheelOpenGLMatrix(jj, transform);

        const auto& eyeSensors = car.getEyeSensors();
        for (const auto& sensor : eyeSensors)
            _message << sensor.near << sensor.far << sensor.value;

        const auto& gSensor = car.getGroundSensor();
        _message << gSensor.near << gSensor.far << gSensor.value;

        const auto&	output = car.getNeuralNetworkOutput();
        _message << output.steer << output.speed;
    }

    // _message << m_contacts.size();
    // for (const auto& contact : m_contacts)
    // 	_message << contact.first << contact.second;
    // m_contacts.clear();

    send();
}