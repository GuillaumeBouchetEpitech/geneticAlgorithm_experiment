
#include "NeuralNetwork.hpp"

#include "demo/utilities/math/RandomNumberGenerator.hpp"
#include "demo/utilities/ErrorHandler.hpp"
#include "demo/utilities/TraceLogger.hpp"

#include <stdexcept>

#include <iostream>

#include <cmath>

namespace activations
{

// /**
//  * it's a steeper sigmoid
//  * => input:  [-x..x]
//  * => output: [0..1]
//  *
//  * Notes:
//  * => use "desmos.com" to visualise the curve
//  * => link: https://www.desmos.com/calculator
//  */
// float steeperSigmoid(float x)
// {
//     return 1.0f / ( 1.0f + expf( -4.9f * x ) );
// }

/**
 * it's a simple RELU function
 * => input:  [-x, x]
 * => output: [0, x]
 *
 * Notes:
 * => faster learning curve than the sigmoid
 * => link: https://stats.stackexchange.com/questions/126238/what-are-the-advantages-of-relu-over-sigmoid-function-in-deep-neural-networks
 */
float rectifiedLinearUnit(float x)
{
    return std::max(0.0f, x);
}

};

NeuralNetwork::NeuralNetwork(const NeuralNetworkTopology& topology)
    : _topology(topology)
{
    if (!topology.isValid())
        D_THROW(std::invalid_argument, "invalid neural network topology");

    unsigned int previousLayerSize = _topology.getInput();

    _layerHidden.resize(_topology.getHiddens().size());
    for (unsigned int ii = 0; ii < _layerHidden.size(); ++ii)
    {
        t_layer& newLayer = _layerHidden[ii];

        unsigned int currentLayerSize = _topology.getHiddens()[ii];

        newLayer.resize(currentLayerSize);
        for (t_neuron& newNeuron : newLayer)
        {
            newNeuron.weights.resize(previousLayerSize);
            for (float& newWeight : newNeuron.weights)
                newWeight = t_RNG::getRangedValue(-1.0f, 1.0f);
        }

        previousLayerSize = currentLayerSize;
    }

    _layerOutput.resize(_topology.getOutput());
    for (t_neuron& newNeuron : _layerOutput)
    {
        newNeuron.weights.resize(previousLayerSize);
        for (float& newWeight : newNeuron.weights)
            newWeight = t_RNG::getRangedValue(-1.0f, 1.0f);
    }
}

void NeuralNetwork::process(const std::vector<float>& input,
                            std::vector<float>& output) const
{
    unsigned int requiredInputs = _topology.getInput();

    if (input.size() != requiredInputs)
        D_THROW(std::invalid_argument,
                "invalid number of input"
                << ", input=" << input.size()
                << ", weights=" << requiredInputs);

    // process hidden layer

    std::vector<float>  hiddenInput = input; // raw copy
    std::vector<float>  hiddenOutput;

    // Cycle over all the neurons and sum their weights against the inputs.
    for (const t_layer& currentLayer : _layerHidden)
    {
        _processLayer(currentLayer, hiddenInput, hiddenOutput);

        hiddenInput = std::move(hiddenOutput); // move
    }

    // process output layer

    _processLayer(_layerOutput, hiddenInput, output);
}

void NeuralNetwork::_processLayer(const t_layer& layer,
                                  const std::vector<float>& input,
                                  std::vector<float>& output) const
{
    output.clear();
    output.reserve(layer.size());

    // Cycle over all the neurons and sum their weights against the inputs.
    for (unsigned int ii = 0; ii < layer.size(); ++ii)
    {
        const auto& neuron = layer[ii];

        // Sum the weights to the activation value.

        float sumValues = 0.0f;
        for (unsigned int jj = 0; jj < input.size(); ++jj)
            sumValues += input[jj] * neuron.weights[jj];

        // Add the bias, it will act as a threshold value

        if (_topology.isUsingBias())
            sumValues += 1.0f;

        // output.push_back(activations::steeperSigmoid(sumValues)); // slower
        output.push_back(activations::rectifiedLinearUnit(sumValues)); // fast
    }

}


void NeuralNetwork::setWeights(const std::vector<float>& inputWeights)
{
    const unsigned int totalWeights = _topology.getTotalWeights();

    if (inputWeights.size() != totalWeights)
    {
        D_THROW(std::invalid_argument,
                "received invalid number of weights"
                << ", expected=" << totalWeights
                << ", input=" << inputWeights.size());
    }

    unsigned int weights_inc = 0;

    for (t_layer& layer : _layerHidden)
        for (t_neuron& neuron : layer)
            for (float& weight : neuron.weights)
                weight = inputWeights[weights_inc++];

    for (t_neuron& neuron : _layerOutput)
        for (float& weight : neuron.weights)
            weight = inputWeights[weights_inc++];
}

void NeuralNetwork::getWeights(std::vector<float>& outputWeights) const
{
    outputWeights.clear();
    outputWeights.reserve(_topology.getTotalWeights());

    for (const t_layer& layer : _layerHidden)
        for (const t_neuron& neuron : layer)
            for (const float weight : neuron.weights)
                outputWeights.push_back( weight );

    for (const t_neuron& neuron : _layerOutput)
        for (const float weight : neuron.weights)
            outputWeights.push_back( weight );
}

const NeuralNetworkTopology&    NeuralNetwork::getTopology() const
{
    return _topology;
}
