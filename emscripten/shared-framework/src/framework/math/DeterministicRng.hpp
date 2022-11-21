
#pragma once


#include <cstdint>

class DeterministicRng
{
public:
  DeterministicRng() = default;

public:
  int getValue();
  void setSeed(int32_t seed);

private:
  uint64_t	_seed = 1;
};
