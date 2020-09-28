
#pragma once

#include "NeuralNetworkTopology.hpp"

class NeuralNetwork
{
private: // internal structures
    struct Neuron
    {
        std::vector<float>  weights;
    };

    using Layer = std::vector<Neuron>;
    using Layers = std::vector<Layer>;

private: // attributs
    Layers _layerHidden;
    Layer  _layerOutput;

    const NeuralNetworkTopology&    _topology;

public: // ctor/dtor
    NeuralNetwork(const NeuralNetworkTopology& topology);

public: // methods
    void process(const std::vector<float>& input,
                 std::vector<float>& output) const;

private:
    void _processLayer(const Layer& layer,
                       const std::vector<float>& input,
                       std::vector<float>& output) const;

public: // setter
    void setWeights(const std::vector<float>& inputWeights);

public: // getter
    void getWeights(std::vector<float>& outputWeights) const;
    const NeuralNetworkTopology& getTopology() const;

};
