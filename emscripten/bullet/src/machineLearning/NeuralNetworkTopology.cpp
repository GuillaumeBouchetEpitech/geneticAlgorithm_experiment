
#include "NeuralNetworkTopology.hpp"

#include "framework/system/ErrorHandler.hpp"
#include "framework/system/TraceLogger.hpp"

#include <exception>
#include <stdexcept>

void
NeuralNetworkTopology::init(
  unsigned int input, const HiddenLayers& hiddens, unsigned int output,
  bool useBias /*= true*/) {
  if (input == 0)
    D_THROW(
      std::invalid_argument,
      "received invalid number of inputs, input=" << input);

  for (unsigned int value : hiddens)
    if (value == 0)
      D_THROW(
        std::invalid_argument,
        "received invalid number of hidden neurons, value=" << value);

  if (output == 0)
    D_THROW(
      std::invalid_argument,
      "received invalid number of outputs, output=" << output);

  //

  _input = input;
  _output = output;
  _hiddens = hiddens;
  _useBias = useBias;

  //

  _computeTotalWeights();
  _computeTotalNeurons();
}

void
NeuralNetworkTopology::init(
  const std::initializer_list<unsigned int>& list, bool useBias /*= true*/) {
  if (list.size() < 2)
    D_THROW(
      std::invalid_argument,
      "received invalid number of layers, list=" << list.size());

  for (unsigned int value : list)
    if (value == 0)
      D_THROW(
        std::invalid_argument,
        "received invalid number of neurons, value=" << value);

  //

  auto it = list.begin();
  if (list.size() > 2)
    _hiddens.reserve(list.size() - 2);
  for (std::size_t ii = 0; ii < list.size(); ++ii, ++it) {
    if (ii == 0)
      _input = *it;
    else if (ii + 1 == list.size())
      _output = *it;
    else
      _hiddens.push_back(*it);
  }

  _useBias = useBias;

  //

  _computeTotalWeights();
  _computeTotalNeurons();
}

void
NeuralNetworkTopology::_computeTotalWeights() {
  unsigned int prev_layer_num_neuron = _input;
  for (unsigned int num_neuron : _hiddens) {
    _totalWeights += prev_layer_num_neuron * num_neuron;
    prev_layer_num_neuron = num_neuron;
  }
  _totalWeights += prev_layer_num_neuron * _output;
}

void
NeuralNetworkTopology::_computeTotalNeurons() {
  _totalNeurons = _input;
  for (int size : _hiddens)
    _totalNeurons += size;
  _totalNeurons += _output;
}

bool
NeuralNetworkTopology::isValid() const {
  if (_input == 0 && _output == 0)
    return false;

  for (unsigned int hiddenValue : _hiddens)
    if (hiddenValue == 0)
      return false;

  return true;
}

unsigned int
NeuralNetworkTopology::getInput() const {
  return _input;
}

unsigned int
NeuralNetworkTopology::getOutput() const {
  return _output;
}

const NeuralNetworkTopology::HiddenLayers&
NeuralNetworkTopology::getHiddens() const {
  return _hiddens;
}

bool
NeuralNetworkTopology::isUsingBias() const {
  return _useBias;
}

unsigned int
NeuralNetworkTopology::getTotalWeights() const {
  return _totalWeights;
}

unsigned int
NeuralNetworkTopology::getTotalNeurons() const {
  return _totalNeurons;
}
