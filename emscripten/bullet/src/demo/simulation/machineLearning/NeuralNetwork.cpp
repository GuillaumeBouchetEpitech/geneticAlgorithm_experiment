
#include "NeuralNetwork.hpp"

#include "demo/utilities/math/RandomNumberGenerator.hpp"
#include "demo/utilities/ErrorHandler.hpp"
#include "demo/utilities/TraceLogger.hpp"

#include <stdexcept>

#include <iostream>

#include <cmath>

namespace /*anonymous*/
{

/**
 * it's a steeper sigmoid
 * => from [-1, 1] to [0, 1]
 */

float customSigmoid(float x)
{
	return 1.0f / ( 1.0f + expf( -4.9f * x ) );
}

};

NeuralNetwork::NeuralNetwork(const NeuralNetworkTopology& topology)
	: _topology(topology)
{
	unsigned int previousLayerSize = _topology.getInput();

	_layerHidden.resize(_topology.getHiddens().size());
	for (unsigned int ii = 0; ii < _layerHidden.size(); ++ii)
	{
		t_layer&	newLayer = _layerHidden[ii];

		unsigned int	currentLayerSize = _topology.getHiddens()[ii];

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

void NeuralNetwork::process(const std::vector<float>& input, std::vector<float>& output) const
{
	output.clear();
	output.reserve(_layerOutput.size());

	// process hidden layer

	std::vector<float>	hiddenInput = input;
	std::vector<float>	hiddenOutput;

	// Cycle over all the neurons and sum their weights against the inputs.
	for (const t_layer&	currentLayer : _layerHidden)
	{
		hiddenOutput.clear();

		processLayer(currentLayer, hiddenInput, hiddenOutput);

		hiddenInput = std::move(hiddenOutput);
	}

	// process output layer

	processLayer(_layerOutput, hiddenInput, output);
}

void NeuralNetwork::processLayer(const t_layer& layer,
								 const std::vector<float>& input,
								 std::vector<float>& output) const
{
	// Cycle over all the neurons and sum their weights against the inputs.
	for (unsigned int ii = 0; ii < layer.size(); ++ii)
	{
		const auto& neuron = layer[ii];

		// if (input.size() != (neuron.weights.size() + (_topology.isUsingBias() ? -1 : 0)))
		// {
		// 	D_THROW(std::invalid_argument,
		// 			"invalid number of weights and input"
		// 			<< ", input=" << inputSize
		// 			<< ", weights=" << neuron.weights.size());
		// }

		// We do the sizeof the weights - 1 so that we can add in the bias to the activation afterwards.

		unsigned int inputSize = input.size();
		if (_topology.isUsingBias())
			--inputSize;

		// Sum the weights to the activation value.

		float activation = 0.0f;
		for (unsigned int jj = 0; jj < inputSize; ++jj)
			activation += input[jj] * neuron.weights[jj];

		// Add the bias, it will act as a threshold value

		const float biasInput = -1.0f;
		if (_topology.isUsingBias())
			activation += biasInput * neuron.weights[ inputSize ];

		// output.push_back(activation);
		output.push_back(customSigmoid(activation));
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

const NeuralNetworkTopology&	NeuralNetwork::getTopology() const
{
	return _topology;
}
