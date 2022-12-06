
#pragma once

#include "NeuralNetworkTopology.hpp"

#include "framework/NonCopyable.hpp"

#include <memory>
#include <vector>

class NeuralNetwork : public NonCopyable {
private: // internal structures
  struct Neuron {
    std::vector<float> connectionsWeights;
    float value;
  };

  using Layer = std::vector<Neuron>;
  using Layers = std::vector<Layer>;

private: // attributes
  Layers _layers;

  const NeuralNetworkTopology& _topology;

public: // ctor/dtor
  NeuralNetwork(const NeuralNetworkTopology& topology);

public: // methods
  void compute(const std::vector<float>& inputValues,
               std::vector<float>& outputValues);

public: // setter
  void setWeights(const std::vector<float>& inputWeights);

public: // getter
  void getWeights(std::vector<float>& outputWeights) const;
  const NeuralNetworkTopology& getTopology() const;

  void getNeuronsValues(std::vector<float>& neuronsOutput);
  void setNeuronsValues(const std::vector<float>& neuronsvalues);
};

using NeuralNetworks = std::vector<std::shared_ptr<NeuralNetwork>>;
