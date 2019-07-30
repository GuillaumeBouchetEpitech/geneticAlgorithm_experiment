
#include "demo/Demo.hpp"

#include <cstdlib> // <= EXIT_SUCCESS

#include "demo/utilities/TraceLogger.hpp"

// #ifndef __EMSCRIPTEN__

int main()
{
	{
		int width = 800;
		int height = 600;

		Demo myDemo(width, height);
		myDemo.run();
	}

	return EXIT_SUCCESS;
}

// #else

// #include <emscripten.h>

// namespace /*anonymous*/
// {
// 	Demo* myDemo = nullptr;

// 	void my_create_demo()
// 	{
// 		int width = 800;
// 		int height = 600;

// 		myDemo = new Demo(width, height);
// 	}

// 	void my_update_demo(unsigned int deltaTime)
// 	{
// 		if (myDemo)
// 			myDemo->process(deltaTime);
// 	}
// };

// extern "C"
// {
// 	void create_demo()
// 	{
// 		my_create_demo();
// 	}

// 	void update_demo(unsigned int deltaTime)
// 	{
// 		my_update_demo(deltaTime);
// 	}
// };

// #endif
