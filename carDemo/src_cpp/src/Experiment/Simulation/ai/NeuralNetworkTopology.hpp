

#ifndef	D_NEURALNETWORKTOPOLOGY_HPP
#define	D_NEURALNETWORKTOPOLOGY_HPP


#include <vector>


class NeuralNetworkTopology
{
private:
	typedef std::vector<unsigned int>	t_hidden_layers;

private: // attributs
	unsigned int		m_input;
	t_hidden_layers		m_hiddens;
	unsigned int		m_output;

	unsigned int				m_totalWeights;

public:
	NeuralNetworkTopology();

	void	init(unsigned int input, const t_hidden_layers& hiddens, unsigned int output);

public:
	bool isValid() const;

public:
	inline unsigned int	getInput() const { return m_input; }
	inline unsigned int	getOutput() const { return m_output; }
	inline const t_hidden_layers&	getHiddens() const { return m_hiddens; }

	inline unsigned int	getTotalWeights() const { return m_totalWeights; }
};


#endif	// D_NEURALNETWORKTOPOLOGY_HPP

