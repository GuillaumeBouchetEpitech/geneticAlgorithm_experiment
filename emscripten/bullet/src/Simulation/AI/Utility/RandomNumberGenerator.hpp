

#pragma once


// #define	D_RAND_MAX	2147483647 // int max positive value : 2,147,483,647
#define	D_RAND_MAX	2147483648

class RandomNumberGenerator
{
private:
	static unsigned long	m_seed;

public:
	static inline void	setSeed(int seed) { m_seed = seed; }

public:
	static int	getValue();

public:
	static inline float 	getNormalisedValue()
	{
		return float(RandomNumberGenerator::getValue()) / D_RAND_MAX;
	}

	static inline int		getRangedValue(int min, int max)
	{
		return  (min + (RandomNumberGenerator::getNormalisedValue() * (max - min)));
	}

	static inline float	getRangedValue(float min, float max)
	{
		return  (min + (RandomNumberGenerator::getNormalisedValue() * (max - min)));
	}

};

typedef RandomNumberGenerator t_RNG;
