
#include "DeterministicRng.hpp"

// constexpr int64_t DeterministicRng::k_randMax = 2147483648;

void DeterministicRng::setSeed(int32_t seed) { _seed = uint64_t(seed); }

int DeterministicRng::getValue() {
  if (_seed == 0)
    _seed = 123459876;

  const int64_t hi = _seed / 127773;
  const int64_t lo = _seed % 127773;
  int64_t x = 16807 * lo - 2836 * hi;

  if (x < 0)
    x += 0x7fffffff;

  return int((_seed = uint64_t(x)) % (k_randMax + 1));
}

float DeterministicRng::getNormalisedValue() {
  return float(getValue()) / float(k_randMax);
}

int DeterministicRng::getRangedValue(int minVal, int maxVal) {
  return int(getRangedValue(float(minVal), float(maxVal)));
}

float DeterministicRng::getRangedValue(float minVal, float maxVal) {
  return minVal + getNormalisedValue() * (maxVal - minVal);
}
