
#include "NeuralNetworkTopology.hpp"

#include "demo/utilities/ErrorHandler.hpp"
#include "demo/utilities/TraceLogger.hpp"

#include <exception>
#include <stdexcept>

void	NeuralNetworkTopology::init(unsigned int input, const t_hiddenLayers& hiddens, unsigned int output, bool useBias /*= true*/)
{
	if (!input)
    	D_THROW(std::invalid_argument, "received invalid number of inputs, input=" << input);

	for (unsigned int value : hiddens)
		if (!value)
	    	D_THROW(std::invalid_argument, "received invalid number of hidden neurons, value=" << value);

	if (!output)
    	D_THROW(std::invalid_argument, "received invalid number of outputs, output=" << output);

	//

	_input = input;
	_output = output;
	_hiddens = hiddens;
	_useBias = useBias;

	//

	if (_useBias)
	{
 		// bias neuron on the input
 		++_input;

 		// bias neuron on the hidden
		for (unsigned int& num_neuron : _hiddens)
			++num_neuron;
	}

	//

	unsigned int prev_layer_num_neuron = _input;
	for (unsigned int num_neuron : _hiddens)
	{
		_totalWeights += prev_layer_num_neuron * num_neuron;
		prev_layer_num_neuron = num_neuron;
	}
	_totalWeights += prev_layer_num_neuron * _output;
}

bool			NeuralNetworkTopology::isValid() const
{
	if (_input == 0 && _output == 0)
		return false;

	for (unsigned int hiddenValue : _hiddens)
		if (hiddenValue == 0)
			return false;

	return true;
}

unsigned int	NeuralNetworkTopology::getInput() const
{
    return _input;
}

unsigned int	NeuralNetworkTopology::getOutput() const
{
    return _output;
}

const			NeuralNetworkTopology::t_hiddenLayers&	NeuralNetworkTopology::getHiddens() const
{
    return _hiddens;
}

bool			NeuralNetworkTopology::isUsingBias() const
{
    return _useBias;
}

unsigned int	NeuralNetworkTopology::getTotalWeights() const
{
    return _totalWeights;
}
