
#pragma once

#include "demo/logic/simulation/webworker/common.hpp"
#include "demo/logic/simulation/logic/CircuitBuilder.hpp"
#include "demo/logic/simulation/logic/CarData.hpp"
#include "demo/logic/simulation/AbstactSimulation.hpp"

#include "machineLearning/NeuralNetwork.hpp"

#include "framework/messaging/MessageBuffer.hpp"
#include "framework/messaging/MessageView.hpp"

#include "framework/NonCopyable.hpp"
#include "framework/asValue.hpp"

#include "demo/defines.hpp"

#include "framework/helpers/GLMath.hpp"

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

    Definition _def;
    GeneticAlgorithm& _geneticAlgorithm;
    uint32_t _coreIndex;

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
    WorkerProducer(const Definition &def, GeneticAlgorithm& geneticAlgorithm, uint32_t coreIndex);

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
