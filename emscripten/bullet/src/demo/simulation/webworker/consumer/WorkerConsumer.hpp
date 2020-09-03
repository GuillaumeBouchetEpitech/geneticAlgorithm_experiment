
#pragma once

#include "demo/defines.hpp"

#include "demo/simulation/machineLearning/NeuralNetwork.hpp"

#include "demo/simulation/logic/Car.hpp"
#include "demo/simulation/logic/CircuitBuilder.hpp"
#include "demo/simulation/logic/physic/PhysicWorld.hpp"

#include "demo/simulation/webworker/common.hpp"

#include "../message/MessageBuffer.hpp"
#include "../message/MessageView.hpp"

#include "thirdparty/GLMath.hpp"

#include <vector>

class WorkerConsumer
{
private:
    unsigned int    _genomesPerCore = 0;

    PhysicWorld     _physicWorld;

    std::vector<Car>    _cars;

    NeuralNetworkTopology       _neuralNetworkTopology;
    std::vector<NeuralNetwork>  _neuralNetworks;

    CircuitBuilder::t_startTransform    _startTransform;

    MessageBuffer   _message;

public:
    WorkerConsumer();

public:
    void    processMessage(const char* dataPointer, int dataSize);

private:
    void    _send();

private:
    void    _initialiseSimulation(MessageView& receivedMsg);
    void    _resetSimulation(MessageView& receivedMsg);
    void    _processSimulation();

};
