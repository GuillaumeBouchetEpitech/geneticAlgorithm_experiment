
#include "RandomNumberGenerator.hpp"

#include <chrono>

std::mt19937 RandomNumberGenerator::_engine;

void RandomNumberGenerator::setSeed(uint32_t seed) { _engine.seed(seed); }

void RandomNumberGenerator::ensureRandomSeed() {
  auto currTime = std::chrono::high_resolution_clock::now();
  auto seed = currTime.time_since_epoch().count();
  _engine.seed(uint32_t(seed));
}

float RandomNumberGenerator::getNormalisedValue() {
  std::uniform_real_distribution<float> dist(0.0f, 1.0f);
  return dist(_engine);
}

template <>
double RandomNumberGenerator::getRangedValue<double>(double min, double max) {
  std::uniform_real_distribution<double> dist(min, max);
  return dist(_engine);
}
