
#include "NeuralNetwork.hpp"

#include "demo/utilities/math/RandomNumberGenerator.hpp"
#include "demo/utilities/ErrorHandler.hpp"
#include "demo/utilities/TraceLogger.hpp"

#include <stdexcept>

#include <iostream>

#include <cmath>

// #define D_USE_SIGMOID

namespace activations
{

#ifdef D_USE_SIGMOID

    /**
     * it's a steeper sigmoid
     * => input:  [-x..x]
     * => output: [0..1]
     *
     * Notes:
     * => use "desmos.com" to visualise the curve
     * => link: https://www.desmos.com/calculator
     */
    float steeperSigmoid(float x)
    {
        return 1.0f / (1.0f + expf(-4.9f * x));
    }

#else

    /**
     * it's a simple RELU function
     * => input:  [-x..x]
     * => output: [0..x]
     *
     * Notes:
     * => faster learning curve than the sigmoid
     * => link: https://stats.stackexchange.com/questions/126238/what-are-the-advantages-of-relu-over-sigmoid-function-in-deep-neural-networks
     */
    float rectifiedLinearUnit(float x)
    {
        return std::max(0.0f, x);
    }

    /**
     * classic clamp function
     * => input:  [-value..value]
     * => output: [minVal..maxVal]
     */
    float clamp(float value, float minVal, float maxVal)
    {
        return std::min(std::max(value, minVal), maxVal);
    }

    /**
     * it's a custom RELU function
     * => input:  [-x..x]
     * => output: [0..1]
     */
    float customRectifiedLinearUnit(float x)
    {
        return clamp(x, 0.0f, 1.0f);
    }

#endif

};

NeuralNetwork::NeuralNetwork(const NeuralNetworkTopology& topology)
    : _topology(topology)
{
    // defensive check
    if (!topology.isValid())
        D_THROW(std::invalid_argument, "invalid neural network topology");

    // 1 input layer + X hidden layer(s) + 1 output layer
    _layers.resize(1 + _topology.getHiddens().size() + 1);

    //
    //
    // input layer

    auto& inputLayer = _layers.front();
    inputLayer.resize(_topology.getInput());

    //

    unsigned int previousLayerSize = _topology.getInput();

    //
    //
    // hidden layers

    for (std::size_t ii = 0; ii < _topology.getHiddens().size(); ++ii)
    {
        const unsigned int currentLayerSize = _topology.getHiddens()[ii];

        // +1 to skip the input layer
        auto& currHiddenLayer = _layers[ii + 1];

        currHiddenLayer.resize(currentLayerSize);
        for (Neuron& newNeuron : currHiddenLayer)
        {
            newNeuron.connectionsWeights.resize(previousLayerSize);
            for (float& connectionsWeight : newNeuron.connectionsWeights)
                connectionsWeight = RNG::getRangedValue(-1.0f, 1.0f);
        }

        previousLayerSize = currentLayerSize;
    }

    //
    //
    // output layer

    auto& outputLayer = _layers.back();
    outputLayer.resize(_topology.getOutput());
    for (Neuron& newNeuron : outputLayer)
    {
        newNeuron.connectionsWeights.resize(previousLayerSize);
        for (float& connectionsWeight : newNeuron.connectionsWeights)
            connectionsWeight = RNG::getRangedValue(-1.0f, 1.0f);
    }
}

void NeuralNetwork::compute(const std::vector<float>& inputValues,
                            std::vector<float>& outputValues)
{
    auto& inputLayer = _layers.front();

    if (inputValues.size() != inputLayer.size())
        D_THROW(std::invalid_argument,
                "invalid number of input"
                << ", input=" << inputValues.size()
                << ", expected=" << inputLayer.size());

    //
    //
    // update input layer

    for (std::size_t ii = 0; ii < inputLayer.size(); ++ii)
        inputLayer[ii].value = inputValues[ii];

    //
    //
    // process all layers

    for (std::size_t ii = 1; ii < _layers.size(); ++ii)
    {
        const Layer& prevLayer = _layers[ii - 1];
        Layer& currLayer = _layers[ii];

        for (auto& neuron : currLayer)
        {
            // Sum the weights to the activation value.
            float sumValues = 0.0f;
            for (std::size_t jj = 0; jj < neuron.connectionsWeights.size(); ++jj)
                sumValues += prevLayer[jj].value * neuron.connectionsWeights[jj];

            if (_topology.isUsingBias())
                sumValues += 1.0f;

#ifdef D_USE_SIGMOID
            // slower learning speed
            neuron.value = activations::steeperSigmoid(sumValues);
#else
            // faster learning speed
            // neuron.value = activations::rectifiedLinearUnit(sumValues);
            neuron.value = activations::customRectifiedLinearUnit(sumValues);
#endif
        }
    }

    //
    //
    // setup output values

    auto& outputLayer = _layers.back();

    outputValues.clear();
    outputValues.reserve(outputLayer.size());
    for (const Neuron& neuron : outputLayer)
        outputValues.push_back(neuron.value);
}

void NeuralNetwork::setWeights(const std::vector<float>& inputWeights)
{
    const unsigned int totalWeights = _topology.getTotalWeights();

    // defensive check
    if (inputWeights.size() != totalWeights)
        D_THROW(std::invalid_argument,
                "received invalid number of weights"
                << ", expected=" << totalWeights
                << ", input=" << inputWeights.size());

    unsigned int weightsIndex = 0;

    for (std::size_t ii = 1; ii < _layers.size(); ++ii)
        for (Neuron& neuron : _layers[ii])
            for (float& connectionsWeight : neuron.connectionsWeights)
                connectionsWeight = inputWeights[weightsIndex++];
}

void NeuralNetwork::getWeights(std::vector<float>& outputWeights) const
{
    outputWeights.clear();
    outputWeights.reserve(_topology.getTotalWeights()); // pre-allocate

    for (std::size_t ii = 1; ii < _layers.size(); ++ii)
        for (const Neuron& neuron : _layers[ii])
            for (const float connectionsWeight : neuron.connectionsWeights)
                outputWeights.push_back(connectionsWeight);
}

const NeuralNetworkTopology& NeuralNetwork::getTopology() const
{
    return _topology;
}

void NeuralNetwork::getNeuronsValues(std::vector<float>& neuronsOutput)
{
    neuronsOutput.clear();
    neuronsOutput.reserve(_topology.getTotalNeurons());

    for (const auto& layer : _layers)
        for (auto& neuron : layer)
            neuronsOutput.push_back(neuron.value);
}
