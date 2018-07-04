

#include "NeuralNetworkTopology.hpp"


#include <exception>
#include <stdexcept>


void	NeuralNetworkTopology::init(unsigned int input, const t_hidden_layers& hiddens, unsigned int output, bool useBias /*= true*/)
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
	m_useBias = useBias;

	//

	if (m_useBias)
	{
 		// bias neuron on the input
 		++m_input;

 		// bias neuron on the hidden
		for (unsigned int& num_neuron : m_hiddens)
			++num_neuron;
	}

	//

	unsigned int prev_layer_num_neuron = m_input;
	for (unsigned int num_neuron : m_hiddens)
	{
		m_totalWeights += prev_layer_num_neuron * num_neuron;
		prev_layer_num_neuron = num_neuron;
	}
	m_totalWeights += prev_layer_num_neuron * m_output;
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






