
#pragma once

// // #define	D_RAND_MAX	2147483647 // int max positive value : 2,147,483,647
// #define	D_RAND_MAX	2147483648

class RandomNumberGenerator
{
private:
	static unsigned long	_seed;
	static const int		_max;

public:
	static void		setSeed(int seed);

public:
	static int		getValue();

public:
	static float	getNormalisedValue();
	static int		getRangedValue(int min, int max);
	static float	getRangedValue(float min, float max);
};

typedef RandomNumberGenerator t_RNG;
