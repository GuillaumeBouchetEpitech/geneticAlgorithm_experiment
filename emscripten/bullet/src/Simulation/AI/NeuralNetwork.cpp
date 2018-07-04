

#include "NeuralNetwork.hpp"

// #include "../utils/randomiser.hpp"
#include "Utility/randomiser.hpp"


#include <stdexcept>

#include <iostream>


NeuralNetwork::NeuralNetwork(const NeuralNetworkTopology& topology)
	:	m_topology(topology)
{
	unsigned int prev_layer_num_neuron = m_topology.getInput();

	m_layerHidden.resize( m_topology.getHiddens().size() );
	for (unsigned int i = 0; i < m_layerHidden.size(); ++i)
	{
		t_layer&	layer = m_layerHidden[i];

		layer.resize( m_topology.getHiddens()[i] );
		for (t_neuron& neuron : layer)
		{
			neuron.m_weights.resize( prev_layer_num_neuron );
			for (float& weight : neuron.m_weights)
				weight = randomClamped();
		}

		prev_layer_num_neuron = m_topology.getHiddens()[i];
	}

	m_layerOutput.resize( m_topology.getOutput() );
	for (t_neuron& neuron : m_layerOutput)
	{
		neuron.m_weights.resize( prev_layer_num_neuron );
		for (float& weight : neuron.m_weights)
			weight = randomClamped();
	}
}

void NeuralNetwork::process(const std::vector<float>& input, std::vector<float>& output) const
{
	output.clear();

	//
	// process hidden layer

	std::vector<float>	hidden_input = input;
	std::vector<float>	hidden_output;

	// Cycle over all the neurons and sum their weights against the inputs.
	for (const t_layer&	current_layer : m_layerHidden)
	{
		hidden_output.clear();

		for (unsigned int i = 0; i < current_layer.size(); i++)
		{
			auto&& curr_neuron = current_layer[i];

			// We do the sizeof the weights - 1 so that we can add in the bias to the activation afterwards.

			unsigned int hidden_input_size = hidden_input.size();
			if (m_topology.isUsingBias())
				--hidden_input_size;

			// Sum the weights to the activation value.

			float activation = 0.0f;
			for (unsigned int j = 0; j < hidden_input_size; j++)
				activation += hidden_input[j] * curr_neuron.m_weights[j];

			// Add the bias, it will act as a threshold value

			if (m_topology.isUsingBias())
				activation += (-1) * curr_neuron.m_weights[ hidden_input_size ];

			// hidden_output.push_back( sigmoid(activation, 1.0f) );
			hidden_output.push_back( activation );
		}

		// hidden_input = hidden_output;
		hidden_input = std::move(hidden_output);
	}

	// process hidden layer
	//

	//
	// process output layer

	// Cycle over all the neurons and sum their weights against the inputs.
	for (unsigned int i = 0; i < m_layerOutput.size(); i++)
	{
		auto&& curr_neuron = m_layerOutput[i];

		// We do the sizeof the weights - 1 so that we can add in the bias to the activation afterwards.

		unsigned int hidden_input_size = hidden_input.size();
		if (m_topology.isUsingBias())
			--hidden_input_size;

		// Sum the weights to the activation value.

		float activation = 0.0f;
		for (unsigned int j = 0; j < hidden_input_size; j++)
			activation += hidden_input[j] * curr_neuron.m_weights[j];

		// Add the bias, it will act as a threshold value

		if (m_topology.isUsingBias())
			activation += (-1) * curr_neuron.m_weights[ hidden_input_size ];

		// output.push_back( sigmoid(activation, 1.0f) );
		output.push_back( activation );
	}

	// process output layer
	//
}

void NeuralNetwork::setWeights(const std::vector<float>& in_weights)
{
	if (in_weights.size() != m_topology.getTotalWeights())
	    throw std::invalid_argument( "received invalid number of weights" );

	unsigned int weights_inc = 0;

	for (t_layer& layer : m_layerHidden)
		for (t_neuron& neuron : layer)
			for (float& weight : neuron.m_weights)
				weight = in_weights[ weights_inc++ ];

	for (t_neuron& neuron : m_layerOutput)
		for (float& weight : neuron.m_weights)
			weight = in_weights[ weights_inc++ ];
}

void NeuralNetwork::getWeights(std::vector<float>& out_weights) const
{
	out_weights.clear();
	out_weights.reserve( m_topology.getTotalWeights() );

	for (const t_layer& layer : m_layerHidden)
		for (const t_neuron& neuron : layer)
			for (const float weight : neuron.m_weights)
				out_weights.push_back( weight );

	for (const t_neuron& neuron : m_layerOutput)
		for (const float weight : neuron.m_weights)
			out_weights.push_back( weight );
}




