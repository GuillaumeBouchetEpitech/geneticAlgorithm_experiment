
#pragma once

#include "NeuralNetworkTopology.hpp"

class NeuralNetwork
{
private: // internal structures
    struct t_neuron
    {
    public:
        std::vector<float>  weights;
    };

    typedef std::vector<t_neuron>   t_layer;

private: // attributs

    std::vector<t_layer>    _layerHidden;
    t_layer                 _layerOutput;

    const NeuralNetworkTopology&    _topology;

public: // ctor/dtor
    NeuralNetwork(const NeuralNetworkTopology& topology);

public: // methods
    void process(const std::vector<float>& input, std::vector<float>& output) const;

private:
    void _processLayer(const t_layer& layer,
                       const std::vector<float>& input,
                       std::vector<float>& output) const;

public: // setter
    void setWeights(const std::vector<float>& in_weights);

public: // getter
    void getWeights(std::vector<float>& out_weights) const;
    const NeuralNetworkTopology&    getTopology() const;

};
