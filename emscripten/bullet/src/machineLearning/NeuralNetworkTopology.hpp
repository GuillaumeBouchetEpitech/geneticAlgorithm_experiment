
#pragma once

#include <vector>

class NeuralNetworkTopology {
public:
  using HiddenLayers = std::vector<unsigned int>;

private: // attributes
  unsigned int _input = 0;
  HiddenLayers _hiddens;
  unsigned int _output = 0;

  bool _useBias = false;

  unsigned int _totalWeights = 0;
  unsigned int _totalNeurons = 0;

public:
  NeuralNetworkTopology() = default;

  void init(
    unsigned int input, const HiddenLayers& hiddens, unsigned int output,
    bool useBias = true);

  void
  init(const std::initializer_list<unsigned int>& list, bool useBias = true);

private:
  void _computeTotalWeights();
  void _computeTotalNeurons();
  ;

public:
  bool isValid() const;

public:
  unsigned int getInput() const;
  unsigned int getOutput() const;
  const HiddenLayers& getHiddens() const;

  bool isUsingBias() const;

  unsigned int getTotalWeights() const;
  unsigned int getTotalNeurons() const;
};
