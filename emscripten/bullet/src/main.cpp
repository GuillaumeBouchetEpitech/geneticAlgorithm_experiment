
#include "demo/Demo.hpp"

#include "demo/utilities/TraceLogger.hpp"

#include <array>
#include <sstream>

#include <cstdlib> // <= EXIT_SUCCESS

int main(int argc, char** argv)
{
    Demo::Definition def;
    def.width = 800;
    def.height = 600;
    def.totalCores = 3;
    def.genomesPerCore = 90;

    { // read command line arguments

        // arra of pointers toward the demo definition arguments
        std::array<unsigned int*, 4> arguments{{
            &def.width,
            &def.height,
            &def.totalCores,
            &def.genomesPerCore,
        }};

        // start at index 1
        for (int ii = 1; ii < argc && ii - 1 < int(arguments.size()); ++ii)
        {
            std::stringstream sstr;
            sstr << argv[ii]; // range [1..argc]
            sstr >> *arguments[ii - 1]; // range [0..arguments.size()]
        }

    } // read command line arguments

    Demo myDemo(def);
    myDemo.run();

    return EXIT_SUCCESS;
}
