
#pragma once

#include "demo/logic/simulation/machineLearning/NeuralNetwork.hpp"
#include "demo/logic/simulation/webworker/common.hpp"
#include "demo/logic/simulation/logic/CircuitBuilder.hpp"
#include "demo/logic/simulation/logic/CarData.hpp"
#include "demo/logic/simulation/AbstactSimulation.hpp"

#include "demo/logic/simulation/webworker/messaging/MessageBuffer.hpp"
#include "demo/logic/simulation/webworker/messaging/MessageView.hpp"

#include "demo/utilities/NonCopyable.hpp"
#include "demo/utilities/types.hpp"

#include "demo/defines.hpp"

#include "helpers/GLMath.hpp"

#include <bitset>

#include <emscripten/emscripten.h> // <= worker_handle

class WorkerProducer
    : public NonCopyable
{
public:
    struct Definition
    {
        CircuitBuilder::StartTransform startTransform;
        CircuitBuilder::Knots knots;
        unsigned int genomesPerCore = 0;
        NeuralNetworkTopology neuralNetworkTopology;

        Definition() = default;
    };

private:
    worker_handle _workerHandle;

    enum class Status : unsigned int
    {
        WebWorkerLoaded = 0,
        Processing,
        Updated,
        Count
    };

    std::bitset<asValue(Status::Count)> _flags;

    AbstactSimulation::CoreState _coreState;

    CarDatas _carsData;

    MessageBuffer _message;

public:
    WorkerProducer(const Definition &def);

private:
    static void _onMessageCallback(char* dataPointer, int dataSize, void* arg);

private:
    void _processMessage(const char* pData, int dataLength);

private:
    void _sendToConsumer();

public:
    void resetAndProcessSimulation(float elapsedTime, unsigned int totalSteps, const NeuralNetworks& neuralNetworks);
    void processSimulation(float elapsedTime, unsigned int totalSteps);

public:
    bool isLoaded() const;
    bool isProcessing() const;
    bool isUpdated() const;
    const CarDatas &getCarsData() const;

    const AbstactSimulation::CoreState &getCoreState() const;
};
