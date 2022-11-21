
#include "demo/Demo.hpp"

#include "framework/TraceLogger.hpp"
#include "framework/ErrorHandler.hpp"

#include <array>
#include <sstream>

#include <cstdint> // <= EXIT_SUCCESS
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
    std::array<uint32_t*, 4> arguments
    {{
      &def.width,
      &def.height,
      &def.totalCores,
      &def.genomesPerCore,
    }};

    for (uint32_t ii = 0; ii < uint32_t(argc) && ii < uint32_t(arguments.size()); ++ii)
    {
      std::stringstream sstr;
      sstr << argv[ii + 1]; // range [1..argc]
      sstr >> *arguments[ii]; // range [0..arguments.size()]

      if (::isinf(*arguments[ii]) || ::isnan(*arguments[ii]))
        D_THROW(std::runtime_error, "argument " << ii << "is not a valid number");
    }

  } // read command line arguments

  Demo myDemo(def);
  myDemo.run();

  return EXIT_SUCCESS;
}
