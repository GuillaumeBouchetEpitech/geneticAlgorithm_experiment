
#include "RandomNumberGenerator.hpp"

#include <chrono>

std::mt19937 RandomNumberGenerator::_engine;

void RandomNumberGenerator::setSeed(int seed)
{
    _engine.seed(seed);
}

void RandomNumberGenerator::ensureRandomSeed()
{
    auto currTime = std::chrono::high_resolution_clock::now();
    auto seed = currTime.time_since_epoch().count();
    _engine.seed(seed);
}

float RandomNumberGenerator::getNormalisedValue()
{
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(_engine);
}

int RandomNumberGenerator::getRangedValue(int min, int max)
{
    return int(getRangedValue(float(min), float(max)));
}

float RandomNumberGenerator::getRangedValue(float min, float max)
{
    std::uniform_real_distribution<float> dist(min, max);
    return dist(_engine);
}
