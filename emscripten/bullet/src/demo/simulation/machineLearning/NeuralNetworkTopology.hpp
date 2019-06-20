
#pragma once

#include <vector>

class NeuralNetworkTopology
{
public:
	typedef std::vector<unsigned int>	t_hiddenLayers;

private: // attributs
	unsigned int		_input = 0;
	t_hiddenLayers		_hiddens;
	unsigned int		_output = 0;

	bool				_useBias = false;

	unsigned int		_totalWeights = 0;

public:
	NeuralNetworkTopology() = default;

	void	init(unsigned int input, const t_hiddenLayers& hiddens, unsigned int output, bool useBias = true);

public:
	bool isValid() const;

public:
	unsigned int			getInput() const;
	unsigned int			getOutput() const;
	const t_hiddenLayers&	getHiddens() const;

	bool					isUsingBias() const;

	unsigned int			getTotalWeights() const;
};
