
#pragma once

#include <random>

class RandomNumberGenerator
{
private:
  static std::mt19937 _engine;

public:
  static void setSeed(uint32_t seed);
  static void ensureRandomSeed();

public:
  static int getValue();

public:
  static float getNormalisedValue();
  static int getRangedValue(int min, int max);
  static float getRangedValue(float min, float max);
};

using RNG = RandomNumberGenerator;
