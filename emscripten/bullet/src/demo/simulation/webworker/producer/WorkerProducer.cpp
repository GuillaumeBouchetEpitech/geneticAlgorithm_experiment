
#include "demo/defines.hpp"

#include "WorkerProducer.hpp"

#include "../preprocessing.hpp"

#include "demo/utilities/ErrorHandler.hpp"
// #include "demo/utilities/TraceLogger.hpp"

WorkerProducer::WorkerProducer(const Definition& def)
{
    _workerHandle = emscripten_create_worker(D_WORKER_SCRIPT_URL);

    _flags[Status::WebWorkerLoaded] = false;
    _flags[Status::Processing] = false;
    _flags[Status::Updated] = false;

    _carsData.resize(def.genomesPerCore);

    {
        _message.clear();
        _message << char(Messages::Client::LoadWorker);

        _message << def.startTransform.position;
        _message << def.startTransform.quaternion;

        _message << int(def.knots.size());

        for (const auto& knot : def.knots)
            _message << knot.left << knot.right << knot.minDistance << knot.color;

        _message << def.genomesPerCore;

        const auto& topology = def.neuralNetworkTopology;
        const auto& hiddenLayers = topology.getHiddens();

        _message << topology.isUsingBias();
        _message << topology.getInput();
        _message << int(hiddenLayers.size());
        for (unsigned int layerValue : hiddenLayers)
            _message << layerValue;
        _message << topology.getOutput();

        _send();
    }
}

void WorkerProducer::_onMessageCallback(char* dataPointer, int dataSize, void* arg)
{
    WorkerProducer* workerProducer = static_cast<WorkerProducer*>(arg);

    workerProducer->_processMessage(dataPointer, dataSize);
}

void WorkerProducer::_processMessage(const char* dataPointer, int dataSize)
{
    _flags[Status::Processing] = false;

    MessageView receivedMsg(dataPointer, dataSize);

    char messageType = 0;
    receivedMsg >> messageType;

    switch (Messages::Server(messageType))
    {
        case Messages::Server::WebWorkerLoaded:
        {
            D_MYLOG("web worker loaded");
            _flags[Status::WebWorkerLoaded] = true;
            break;
        }

        case Messages::Server::SimulationResult:
        {
            receivedMsg >> _coreState.delta >> _coreState.genomesAlive;

            for (auto& car : _carsData)
            {
                receivedMsg
                    >> car.isAlive
                    >> car.life
                    >> car.fitness
                    >> car.totalUpdates
                    >> car.groundIndex;

                if (!car.isAlive)
                    continue;

                receivedMsg >> car.transform;
                for (auto& transform : car.wheelsTransform)
                    receivedMsg >> transform;

                receivedMsg >> car.velocity;

                for (auto& sensor : car.eyeSensors)
                    receivedMsg >> sensor.near >> sensor.far >> sensor.value;

                auto& gSensor = car.groundSensor;
                receivedMsg >> gSensor.near >> gSensor.far >> gSensor.value;

                auto& output = car.neuralNetworkOutput;
                receivedMsg >> output.steer >> output.speed;
            }

            _flags[Status::Updated] = true;
            break;
        }

        default:
        {
            D_THROW(std::runtime_error, "unknown worker message");
        }
    }
}

void WorkerProducer::_send()
{
    _flags[Status::Processing] = true;

    char*           dataPointer = const_cast<char*>(_message.getData());
    unsigned int    dataSize = _message.getSize();

    em_worker_callback_func callback = WorkerProducer::_onMessageCallback;

    emscripten_call_worker(_workerHandle, WORKER_MAIN_STR, dataPointer, dataSize, callback, (void*)this);
}

void WorkerProducer::resetAndProcessSimulation(const NeuralNetwork* neuralNetworks)
{
    _message.clear();
    _message << char(Messages::Client::ResetAndProcessSimulation);

    std::vector<float>  weights;

    for (unsigned int ii = 0; ii < _carsData.size(); ++ii)
    {
        neuralNetworks[ii].getWeights(weights);

        _message.append(weights.data(), weights.size() * sizeof(float));
    }

    _send();
}

void WorkerProducer::processSimulation()
{
    _message.clear();
    _message << char(Messages::Client::ProcessSimulation);

    _send();
}

bool WorkerProducer::isLoaded() const
{
    return _flags[Status::WebWorkerLoaded];
}

bool WorkerProducer::isProcessing() const
{
    return _flags[Status::Processing];
}

bool WorkerProducer::isUpdated() const
{
    return _flags[Status::Updated];
}

const CarDatas& WorkerProducer::getCarsData() const
{
    return _carsData;
}

const AbstactSimulation::CoreState& WorkerProducer::getCoreState() const
{
    return _coreState;
}
