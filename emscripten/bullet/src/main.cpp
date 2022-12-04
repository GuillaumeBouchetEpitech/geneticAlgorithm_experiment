
#include "demo/Demo.hpp"

#include "framework/TraceLogger.hpp"
#include "framework/ErrorHandler.hpp"

#include <array>
#include <sstream>

#include <cstdint> // <= EXIT_SUCCESS
#include <cstdlib> // <= EXIT_SUCCESS

namespace
{

void processCommandLineArgs(Demo::Definition& def, int argc, char** argv)
{
  // array of pointers toward the demo definition arguments
  std::array<uint32_t*, 4> arguments
  {{
    &def.width,
    &def.height,
    &def.totalCores,
    &def.genomesPerCore,
  }};

  std::vector<std::string_view> safeArgs;
  safeArgs.reserve(argc - 1);
  for (int ii = 1; ii < argc; ++ii)
    safeArgs.push_back(argv[ii]);

  const std::size_t safeSize = std::min(safeArgs.size(), arguments.size());
  for (std::size_t ii = 0; ii < safeSize; ++ii)
  {
    std::stringstream sstr;
    sstr << safeArgs.at(ii);

    uint32_t argValue;
    sstr >> argValue;

    if (::isinf(argValue) || ::isnan(argValue))
      D_THROW(std::runtime_error, "argument " << ii << "is not a valid number");

    *arguments.at(ii) = argValue;
  }
}

}

int main(int argc, char** argv)
{
  Demo::Definition def;
  def.width = 800;
  def.height = 600;
  def.totalCores = 3;
  def.genomesPerCore = 90;

  processCommandLineArgs(def, argc, argv);

  Demo myDemo(def);
  myDemo.run();

  return EXIT_SUCCESS;
}
