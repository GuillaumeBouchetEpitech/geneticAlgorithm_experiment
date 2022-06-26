
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

        for (int ii = 0; ii < argc && ii < int(arguments.size()); ++ii)
        {
            std::stringstream sstr;
            sstr << argv[ii + 1]; // range [1..argc]
            sstr >> *arguments[ii]; // range [0..arguments.size()]
        }

    } // read command line arguments

    Demo myDemo(def);
    myDemo.run();

    return EXIT_SUCCESS;
}
