
#pragma once

#include "framework/system/containers/static_array.hpp"

class FitnessStats {
private:
  std::size_t _index = 0;
  static_array<float, 10> _allFitnesses;
  float _maxFitness = 0.0f;

public:
  FitnessStats();
  ~FitnessStats() = default;

public:
  void update(float latestFitness);

public:
  float get(int index) const;
  std::size_t size() const;
  float max() const;
};
