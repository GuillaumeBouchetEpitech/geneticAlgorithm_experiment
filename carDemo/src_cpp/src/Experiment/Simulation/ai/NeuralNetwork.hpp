

#ifndef D_NEURALNETWORK_HPP
#define D_NEURALNETWORK_HPP


#include "NeuralNetworkTopology.hpp"


class NeuralNetwork
{
private: // internal structures
	struct t_neuron
	{
	public:
		std::vector<float>	m_weights;
	};

	typedef std::vector<t_neuron>	t_layer;

private: // attributs

	std::vector<t_layer>	m_layerHidden;
	t_layer					m_layerOutput;

	const NeuralNetworkTopology&	m_topology;

public: // ctor/dtor
	NeuralNetwork(const NeuralNetworkTopology& topology);

public: // methods
	void process(const std::vector<float>& input, std::vector<float>& output) const;

public: // setter/getter
	void setWeights(const std::vector<float>& in_weights);
	void getWeights(std::vector<float>& out_weights) const;

};


#endif	// D_NEURALNETWORK_HPP

