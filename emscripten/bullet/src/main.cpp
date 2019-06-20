
#include "demo/Demo.hpp"

#include <cstdlib> // <= EXIT_SUCCESS

int	main()
{
	{
		Demo	myDemo(800, 600);

		myDemo.run();
	}

	return EXIT_SUCCESS;
}
