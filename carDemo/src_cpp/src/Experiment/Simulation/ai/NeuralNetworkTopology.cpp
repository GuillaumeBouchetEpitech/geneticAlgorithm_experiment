

#include "NeuralNetworkTopology.hpp"


#include <exception>
#include <stdexcept>



NeuralNetworkTopology::NeuralNetworkTopology()
	:	m_input(0),
		m_output(0),
		m_totalWeights(0)
{
}

void	NeuralNetworkTopology::init(unsigned int input, const t_hidden_layers& hiddens, unsigned int output)
{
	if (!input)
		throw std::invalid_argument( "received invalid number of inputs" );

	// if (hiddens.empty())
	// 	throw std::invalid_argument( "received invalid number of hidden layer" );

	for (unsigned int val : hiddens)
		if (!val)
			throw std::invalid_argument( "received invalid number of hidden neurons" );

	if (!output)
		throw std::invalid_argument( "received invalid number of outputs" );

	//

	m_input = input;
	m_output = output;
	m_hiddens = hiddens;

	//

	unsigned int prev_layer_num_neuron = input;
	for (unsigned int num_neuron : hiddens)
	{
		m_totalWeights += prev_layer_num_neuron * num_neuron;
		prev_layer_num_neuron = num_neuron;
	}
	m_totalWeights += prev_layer_num_neuron * output;
}

bool NeuralNetworkTopology::isValid() const
{
	bool valid = (m_input > 0 && m_output > 0);

	if (valid)
		for (auto& elem : m_hiddens)
			if (elem == 0)
				return false;

	return valid;
}






