

#include "RandomNumberGenerator.hpp"


unsigned long	RandomNumberGenerator::m_seed = 1;

int	RandomNumberGenerator::getValue()
{
	long	hi, lo, x;

	if (m_seed == 0)
		m_seed = 123459876;

	hi = m_seed / 127773;
	lo = m_seed % 127773;
	x = 16807 * lo - 2836 * hi;

	if (x < 0)
		x += 0x7fffffff;

	return ( (m_seed = x) % ((unsigned long)D_RAND_MAX + 1));
}
