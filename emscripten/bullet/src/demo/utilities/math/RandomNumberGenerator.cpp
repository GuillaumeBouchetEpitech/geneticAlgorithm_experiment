
#include "RandomNumberGenerator.hpp"

#include <limits>
#include <chrono>

unsigned long RandomNumberGenerator::_seed = 1;
const int RandomNumberGenerator::_max = std::numeric_limits<int>::max();

void RandomNumberGenerator::setSeed(int seed)
{
    _seed = seed;
}

void RandomNumberGenerator::ensureRandomSeed()
{
    auto currTime = std::chrono::high_resolution_clock::now();
    auto seed = currTime.time_since_epoch().count();
    _seed = static_cast<unsigned int>(seed);
}

int RandomNumberGenerator::getValue()
{
    if (_seed == 0)
        _seed = 123459876;

    long hi = _seed / 127773;
    long lo = _seed % 127773;
    long x = 16807 * lo - 2836 * hi;

    if (x < 0)
        x += 0x7fffffff;

    return ((_seed = x) % ((unsigned long)_max + 1));
}

float RandomNumberGenerator::getNormalisedValue()
{
    return float(RandomNumberGenerator::getValue()) / float(_max);
}

int RandomNumberGenerator::getRangedValue(int min, int max)
{
    return  (min + (RandomNumberGenerator::getNormalisedValue() * (max - min)));
}

float RandomNumberGenerator::getRangedValue(float min, float max)
{
    return  (min + (RandomNumberGenerator::getNormalisedValue() * (max - min)));
}
