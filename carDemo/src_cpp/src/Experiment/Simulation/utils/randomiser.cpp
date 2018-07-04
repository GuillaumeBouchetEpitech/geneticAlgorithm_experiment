

#include "randomiser.hpp"


namespace
{
	unsigned long	s_seed = 1;
}


int	my_rand()
{
	long	hi, lo, x;

	if (s_seed == 0)
		s_seed = 123459876;

	hi = s_seed / 127773;
	lo = s_seed % 127773;
	x = 16807 * lo - 2836 * hi;

	if (x < 0)
		x += 0x7fffffff;

	return ( (s_seed = x) % ((unsigned long)D_RAND_MAX + 1));
}

void	my_srand(int seed)
{
	s_seed = seed;
}