
#pragma once

class RandomNumberGenerator
{
private:
    static unsigned long _seed;
    static const int _max;

public:
    static void setSeed(int seed);
    static void ensureRandomSeed();

public:
    static int getValue();

public:
    static float getNormalisedValue();
    static int getRangedValue(int min, int max);
    static float getRangedValue(float min, float max);
};

using RNG = RandomNumberGenerator;
