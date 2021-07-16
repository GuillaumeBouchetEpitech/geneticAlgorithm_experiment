
#pragma once

#include "NeuralNetworkTopology.hpp"

#include "demo/utilities/NonCopyable.hpp"

#include <vector>
#include <memory>

class NeuralNetwork
    : public NonCopyable
{
private: // internal structures
    struct Neuron
    {
        std::vector<float> weights;
    };

    using Layer = std::vector<Neuron>;
    using Layers = std::vector<Layer>;

private: // attributs
    Layers _layerHidden;
    Layer _layerOutput;

    const NeuralNetworkTopology& _topology;

public: // ctor/dtor
    NeuralNetwork(const NeuralNetworkTopology& topology);

public: // methods
    void compute(const std::vector<float>& inputValues,
                 std::vector<float>& outputValues) const;

private:
    void _processLayer(const Layer& layer,
                       const std::vector<float>& inputValues,
                       std::vector<float>& outputValues) const;

public: // setter
    void setWeights(const std::vector<float>& inputWeights);

public: // getter
    void getWeights(std::vector<float>& outputWeights) const;
    const NeuralNetworkTopology& getTopology() const;

    void getNeuronsOutput(const std::vector<float>& inputValues,
                          std::vector<float>& neuronsOutput) const;

};

using NeuralNetworks = std::vector<std::shared_ptr<NeuralNetwork>>;
