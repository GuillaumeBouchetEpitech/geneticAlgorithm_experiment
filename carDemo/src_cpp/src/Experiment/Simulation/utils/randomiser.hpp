

#ifndef	D_RANDOMISER_HPP
#define	D_RANDOMISER_HPP


// #define	D_RAND_MAX	2147483647 // int max positive value : 2,147,483,647
#define	D_RAND_MAX	2147483648

int		my_rand();
void	my_srand(int seed);

inline int		my_rangeRand(int min, int max)
{
	return  ( min + ( my_rand() / D_RAND_MAX * (max - min) ) );
}

inline float	my_rangeRand(float min, float max)
{
	return  ( min + ( (float)my_rand() / D_RAND_MAX * (max - min) ) );
}

inline float randomFloat()
{
	// return ((float)(rand())) / ((float)(RAND_MAX));
	return my_rangeRand(0.0f, 1.0f);
}

inline float	randomClamped()
{
	// return (randomFloat() - randomFloat());
	return my_rangeRand(-1.0f, 1.0f);
}


#endif	// D_RANDOMISER_HPP

