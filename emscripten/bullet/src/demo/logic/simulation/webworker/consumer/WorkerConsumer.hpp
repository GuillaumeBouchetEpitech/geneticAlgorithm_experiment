
#pragma once

#include "demo/logic/simulation/logic/CarAgent.hpp"
#include "demo/logic/simulation/logic/CarData.hpp"
#include "demo/logic/simulation/logic/CircuitBuilder.hpp"
// #include "demo/logic/simulation/logic/physic/PhysicWorld.hpp"
#include "demo/logic/simulation/webworker/common.hpp"

#include "machineLearning/NeuralNetwork.hpp"

#include "framework/helpers/GLMath.hpp"
#include "framework/physic/PhysicWorld.hpp"
#include "framework/system/NonCopyable.hpp"
#include "framework/system/messaging/MessageBuffer.hpp"
#include "framework/system/messaging/MessageView.hpp"

#include "demo/defines.hpp"

#include <memory>
#include <vector>

class WorkerConsumer : public NonCopyable {
private:
  unsigned int _genomesPerCore = 0;

  std::unique_ptr<PhysicWorld> _physicWorld;

  CarAgents _carAgents;

  std::vector<std::vector<CarData::Transforms>> _latestTransformsHistory;

  NeuralNetworkTopology _neuralNetworkTopology;
  std::vector<std::shared_ptr<NeuralNetwork>> _neuralNetworks;

  CircuitBuilder::StartTransform _startTransform;
  // CircuitBuilder::Knots _circuitKnots;
  CircuitBuilder _circuitBuilder;

  MessageBuffer _messageToSend;

public:
  WorkerConsumer() = default;

public:
  void processMessage(const char* dataPointer, int dataSize);

private:
  void _sendBackToProducer();

private:
  void _initialiseSimulation(MessageView& receivedMsg);
  void _resetSimulation(MessageView& receivedMsg);
  void _processSimulation(float elapsedTime, unsigned int totalSteps);

private:
  void _resetPhysic();
};
