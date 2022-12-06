
#include "FitnessStats.hpp"

#include "framework/TraceLogger.hpp"

FitnessStats::FitnessStats() {
  _index = 0;
  for (float& fitness : _allFitnesses)
    fitness = 0.0f;
}

void FitnessStats::update(float latestFitness) {
  const float lastFitness = get(-1);
  const float secondLastFitness = get(-2);
  const bool lastFitnessWasSmarter = lastFitness > secondLastFitness;
  if (lastFitnessWasSmarter)
    _index = (_index + 1) % _allFitnesses.size();

  _allFitnesses[int(_index) - 1] = latestFitness;
  _maxFitness = std::max(_maxFitness, latestFitness);
}

float FitnessStats::get(int index) const {
  return _allFitnesses[int(_index) + index];
}

std::size_t FitnessStats::size() const { return _allFitnesses.size(); }

float FitnessStats::max() const { return _maxFitness; }
