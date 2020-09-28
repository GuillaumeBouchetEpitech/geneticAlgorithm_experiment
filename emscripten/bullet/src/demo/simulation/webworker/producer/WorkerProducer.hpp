
#pragma once

#include "demo/defines.hpp"

#include "demo/simulation/machineLearning/NeuralNetwork.hpp"
#include "demo/simulation/webworker/common.hpp"
#include "demo/simulation/logic/CircuitBuilder.hpp"

#include "../message/MessageBuffer.hpp"
#include "../message/MessageView.hpp"

#include "demo/simulation/logic/CarData.hpp"

#include "demo/simulation/AbstactSimulation.hpp"

#include "thirdparty/GLMath.hpp"

#include <bitset>

#include <emscripten/emscripten.h> // <= worker_handle

class WorkerProducer
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
    worker_handle   _workerHandle;

    enum Status
    {
        WebWorkerLoaded = 0,
        Processing,
        Updated,
        Count
    };

    std::bitset<Status::Count> _flags;

    AbstactSimulation::CoreState _coreState;

    CarDatas _carsData;

    MessageBuffer _message;

public:
    WorkerProducer(const Definition& def);

private:
    static void _onMessageCallback(char* pData, int size, void* arg);

private:
    void _processMessage(const char* pData, int dataLength);

private:
    void _send();

public:
    void resetAndProcessSimulation(const NeuralNetwork* pNeuralNetworks);
    void processSimulation();

public:
    bool isLoaded() const;
    bool isProcessing() const;
    bool isUpdated() const;
    const CarDatas& getCarsData() const;

    const AbstactSimulation::CoreState& getCoreState() const;

};
