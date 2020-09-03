
#include "demo/defines.hpp"

#include "WorkerProducer.hpp"

#include "demo/utilities/ErrorHandler.hpp"
// #include "demo/utilities/TraceLogger.hpp"

WorkerProducer::WorkerProducer(const t_def& def)
{
    _workerHandle = emscripten_create_worker(D_WORKER_SCRIPT_URL);

    _flags[e_Status::eWebWorkerLoaded] = false;
    _flags[e_Status::eProcessing] = false;
    _flags[e_Status::eUpdated] = false;

    _carsData.resize(def.genomesPerCore);

    {
        _message.clear();
        _message << char(messages::client::eLoadWorker);

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

void    WorkerProducer::_onMessageCallback(char* dataPointer, int dataSize, void* arg)
{
    WorkerProducer* workerProducer = static_cast<WorkerProducer*>(arg);

    workerProducer->_processMessage(dataPointer, dataSize);
}

void    WorkerProducer::_processMessage(const char* dataPointer, int dataSize)
{
    _flags[e_Status::eProcessing] = false;

    MessageView receivedMsg(dataPointer, dataSize);

    char messageType = 0;
    receivedMsg >> messageType;

    switch (messages::server(messageType))
    {
        case messages::server::eWebWorkerLoaded:
        {
            D_MYLOG("web worker loaded");
            _flags[e_Status::eWebWorkerLoaded] = true;
            break;
        }

        case messages::server::eSimulationResult:
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

                for (auto& sensor : car.eyeSensors)
                    receivedMsg >> sensor.near >> sensor.far >> sensor.value;

                auto& gSensor = car.groundSensor;
                receivedMsg >> gSensor.near >> gSensor.far >> gSensor.value;

                auto& output = car.neuralNetworkOutput;
                receivedMsg >> output.steer >> output.speed;
            }

            _flags[e_Status::eUpdated] = true;
            break;
        }

        default:
        {
            D_THROW(std::runtime_error, "unknown worker message");
        }
    }
}

void    WorkerProducer::_send()
{
    _flags[e_Status::eProcessing] = true;

    char*           dataPointer = const_cast<char*>(_message.getData());
    unsigned int    dataSize = _message.getSize();

    em_worker_callback_func callback = WorkerProducer::_onMessageCallback;

// hacky way to force the name of the function to be from the macro "D_WORKER_MAIN"
#define WORKER_MAIN_FUNC_NAME(func_name) "_" # func_name

    emscripten_call_worker(_workerHandle, WORKER_MAIN_FUNC_NAME(D_WORKER_MAIN), dataPointer, dataSize, callback, (void*)this);
}

void    WorkerProducer::resetAndProcessSimulation(const NeuralNetwork* neuralNetworks)
{
    _message.clear();
    _message << char(messages::client::eResetAndProcessSimulation);

    std::vector<float>  weights;

    for (unsigned int ii = 0; ii < _carsData.size(); ++ii)
    {
        neuralNetworks[ii].getWeights(weights);

        _message.append(weights.data(), weights.size() * sizeof(float));
    }

    _send();
}

void    WorkerProducer::processSimulation()
{
    _message.clear();
    _message << char(messages::client::eProcessSimulation);

    _send();
}

bool    WorkerProducer::isLoaded() const
{
    return _flags[e_Status::eWebWorkerLoaded];
}

bool    WorkerProducer::isProcessing() const
{
    return _flags[e_Status::eProcessing];
}

bool    WorkerProducer::isUpdated() const
{
    return _flags[e_Status::eUpdated];
}

const t_carsData&   WorkerProducer::getCarsData() const
{
    return _carsData;
}

const AbstactSimulation::t_coreState&   WorkerProducer::getCoreState() const
{
    return _coreState;
}
