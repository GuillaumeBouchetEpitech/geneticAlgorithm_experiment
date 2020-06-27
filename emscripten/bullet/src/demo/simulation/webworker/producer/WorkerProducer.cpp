
#include "demo/defines.hpp"

#if not defined D_WEB_WEBWORKER_BUILD
#   error "exclude this file to build natively or with multi thread support"
#endif

#include "WorkerProducer.hpp"

#include "demo/utilities/ErrorHandler.hpp"
// #include "demo/utilities/TraceLogger.hpp"

WorkerProducer::WorkerProducer(const t_def& def)
{
    _workerHandle = emscripten_create_worker(D_WORKER_SCRIPT_URL);

    _flags[e_Status::eWebWorkerLoaded] = false;
    _flags[e_Status::eProcessing] = false;
    _flags[e_Status::eUpdated] = false;

    // _contacts.reserve(64);

    _carsData.resize(def.genomesPerCore);

    {
        _message.clear();
        _message << char(messages::client::eLoadWorker);
        _message << def.filename;
        _message << def.genomesPerCore;

        const auto& topology = def.neuralNetworkTopology;
        const auto& hiddenLayers = topology.getHiddens();

        _message << topology.isUsingBias();
        _message << topology.getInput();
        _message << int(hiddenLayers.size());
        for (unsigned int layerValue : hiddenLayers)
            _message << layerValue;
        _message << topology.getOutput();

        send();
    }
}

void    WorkerProducer::onMessageCallback(char* dataPointer, int dataSize, void* arg)
{
    WorkerProducer* workerProducer = static_cast<WorkerProducer*>(arg);

    workerProducer->processMessage(dataPointer, dataSize);
}

void    WorkerProducer::processMessage(const char* dataPointer, int dataSize)
{
    _flags[e_Status::eProcessing] = false;

    MessageView message(dataPointer, dataSize);

    char messageType = 0;
    message >> messageType;

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
            message >> _coreState.delta >> _coreState.genomesAlive;

            for (auto& car : _carsData)
            {
                message
                    >> car.isAlive
                    >> car.life
                    >> car.fitness
                    >> car.totalUpdates
                    >> car.groundIndex;

                if (!car.isAlive)
                    continue;

                message >> car.transform;
                for (auto& transform : car.wheelsTransform)
                    message >> transform;

                for (auto& sensor : car.eyeSensors)
                    message >> sensor.near >> sensor.far >> sensor.value;

                auto& gSensor = car.groundSensor;
                message >> gSensor.near >> gSensor.far >> gSensor.value;

                auto& output = car.neuralNetworkOutput;
                message >> output.steer >> output.speed;
            }

            {
                // unsigned int contactNumber = 0;
                // message >> contactNumber;

                // glm::vec3    position, normal;
                // for (unsigned int ii = 0; ii < contactNumber; ++ii)
                // {
                //  message >> position >> normal;
                //  _contacts.push_back({ position, normal });
                // }
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

void    WorkerProducer::send()
{
    _flags[e_Status::eProcessing] = true;

    char*           dataPointer = const_cast<char*>(_message.getData());
    unsigned int    dataSize = _message.getSize();

    em_worker_callback_func     callback = WorkerProducer::onMessageCallback;

    emscripten_call_worker(_workerHandle, D_WORKER_MAIN, dataPointer, dataSize, callback, (void*)this);
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

    send();
}

void    WorkerProducer::processSimulation()
{
    _message.clear();
    _message << char(messages::client::eProcessSimulation);

    send();
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

// const WorkerProducer::t_contacts&    WorkerProducer::getContactsData() const
// {
//  return _contacts;
// }

// void WorkerProducer::clearContactsData()
// {
//  _contacts.clear();
// }

