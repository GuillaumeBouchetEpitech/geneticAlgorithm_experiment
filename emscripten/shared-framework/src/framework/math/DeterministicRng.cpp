
#include "DeterministicRng.hpp"

#define	D_RAND_MAX	2147483648

int DeterministicRng::getValue()
{
	if (_seed == 0)
		_seed = 123459876;

	const int64_t hi = _seed / 127773;
	const int64_t lo = _seed % 127773;
	int64_t x = 16807 * lo - 2836 * hi;

	if (x < 0)
		x += 0x7fffffff;

	return int((_seed = uint64_t(x)) % (int64_t(D_RAND_MAX) + 1));
}

void DeterministicRng::setSeed(int32_t seed)
{
  _seed = uint64_t(seed);
}
