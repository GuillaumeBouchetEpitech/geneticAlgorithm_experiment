
#pragma once

#include "demo/logic/simulation/machineLearning/NeuralNetwork.hpp"
#include "demo/logic/simulation/logic/CarAgent.hpp"
#include "demo/logic/simulation/logic/CarData.hpp"
#include "demo/logic/simulation/logic/CircuitBuilder.hpp"
#include "demo/logic/simulation/logic/physic/PhysicWorld.hpp"
#include "demo/logic/simulation/webworker/common.hpp"
#include "demo/logic/simulation/webworker/messaging/MessageBuffer.hpp"
#include "demo/logic/simulation/webworker/messaging/MessageView.hpp"

#include "demo/utilities/NonCopyable.hpp"

#include "demo/defines.hpp"

#include "helpers/GLMath.hpp"

#include <vector>

class WorkerConsumer
    : public NonCopyable
{
private:
    unsigned int _genomesPerCore = 0;

    PhysicWorld _physicWorld;

    CarAgents _carAgents;

    std::vector<std::vector<CarData::Transforms>> _latestTransformsHistory;

    NeuralNetworkTopology _neuralNetworkTopology;
    std::vector<std::shared_ptr<NeuralNetwork>> _neuralNetworks;

    CircuitBuilder::StartTransform _startTransform;

    MessageBuffer _messageToSend;

public:
    WorkerConsumer() = default;

public:
    void processMessage(const char *dataPointer, int dataSize);

private:
    void _sendBackToProducer();

private:
    void _initialiseSimulation(MessageView &receivedMsg);
    void _resetSimulation(MessageView &receivedMsg);
    void _processSimulation(float elapsedTime, unsigned int totalSteps);
};
