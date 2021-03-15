
#pragma once

#include "demo/defines.hpp"

#include "demo/logic/simulation/machineLearning/NeuralNetwork.hpp"
#include "demo/logic/simulation/logic/Car.hpp"
#include "demo/logic/simulation/logic/CircuitBuilder.hpp"
#include "demo/logic/simulation/logic/physic/PhysicWorld.hpp"
#include "demo/logic/simulation/webworker/common.hpp"
#include "demo/utilities/NonCopyable.hpp"

#include "../message/MessageBuffer.hpp"
#include "../message/MessageView.hpp"

#include "thirdparty/GLMath.hpp"

#include <vector>

class WorkerConsumer
    : public NonCopyable
{
private:
    unsigned int _genomesPerCore = 0;

    PhysicWorld _physicWorld;

    std::vector<Car> _cars;

    NeuralNetworkTopology _neuralNetworkTopology;
    std::vector<NeuralNetwork> _neuralNetworks;

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
    void _processSimulation();
};
