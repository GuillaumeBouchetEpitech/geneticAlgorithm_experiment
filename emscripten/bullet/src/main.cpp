
#include "demo/Demo.hpp"

#include <cstdlib> // <= EXIT_SUCCESS

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
