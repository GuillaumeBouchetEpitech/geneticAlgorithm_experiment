
#pragma once

#include "demo/defines.hpp"

#if not defined D_WEB_WEBWORKER_BUILD
#   error "exclude this file to build natively or with multi thread support"
#endif

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

    // std::vector<std::pair<glm::vec3, glm::vec3>> m_contacts;

    MessageBuffer   _message;

public:
    WorkerConsumer();

private:
    void    generateCircuit();
    void    generateCars();
    void    generateNeuralNetworks();

public:
    void    processMessage(const char* dataPointer, int dataSize);

private:
    void    send();

private:
    void    initialiseSimulation(MessageView& message);
    void    resetSimulation(MessageView& message);
    void    processSimulation();

};
