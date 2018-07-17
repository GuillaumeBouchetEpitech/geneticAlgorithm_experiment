

#pragma once


#include <vector>


class NeuralNetworkTopology
{
private:
	typedef std::vector<unsigned int>	t_hidden_layers;

private: // attributs
	unsigned int		m_input = 0;
	t_hidden_layers		m_hiddens;
	unsigned int		m_output = 0;

	bool				m_useBias = false;

	unsigned int		m_totalWeights = 0;

public:
	NeuralNetworkTopology() = default;

	void	init(unsigned int input, const t_hidden_layers& hiddens, unsigned int output, bool useBias = true);

public:
	bool isValid() const;

public:
	inline unsigned int	getInput() const { return m_input; }
	inline unsigned int	getOutput() const { return m_output; }
	inline const t_hidden_layers&	getHiddens() const { return m_hiddens; }

	inline bool			isUsingBias() const { return m_useBias; }

	inline unsigned int	getTotalWeights() const { return m_totalWeights; }
};
