

#include "NeuralNetwork.hpp"

#include "../utils/randomiser.hpp"


#include <stdexcept>


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
			float activation = 0.0f;

			// Sum the weights to the activation value.
			// We do the sizeof the weights - 1 so that we can add in the bias to the 
			// activation afterwards.
			for (unsigned int j = 0; j < hidden_input.size(); j++)
				activation += hidden_input[j] * current_layer[i].m_weights[j];

			// // Add the bias, it will act as a threshold value
			// activation += (-1) * current_layer[i].m_weights[ (hidden_input.size()-1) ];

			// hidden_output.push_back( sigmoid(activation, 1.0f) );
			hidden_output.push_back( activation );
		}

		hidden_input = hidden_output;
	}

	// process hidden layer
	//

	//
	// process output layer

	// Cycle over all the neurons and sum their weights against the inputs.
	for (unsigned int i = 0; i < m_layerOutput.size(); i++)
	{
		float activation = 0.0f;

		// Sum the weights to the activation value.
		// We do the sizeof the weights - 1 so that we can add in the bias to the 
		// activation afterwards.
		for (unsigned int j = 0; j < hidden_input.size(); j++)
			activation += hidden_input[j] * m_layerOutput[i].m_weights[j];

		// // Add the bias, it will act as a threshold value
		// activation += (-1) * m_layerOutput[i].m_weights[ (hidden_output.size() - 1) ];

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
	out_weights.reserve( m_topology.getTotalWeights() );

	for (const t_layer& layer : m_layerHidden)
		for (const t_neuron& neuron : layer)
			for (const float weight : neuron.m_weights)
				out_weights.push_back( weight );

	for (const t_neuron& neuron : m_layerOutput)
		for (const float weight : neuron.m_weights)
			out_weights.push_back( weight );
}


