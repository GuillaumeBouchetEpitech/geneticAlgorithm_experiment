
#include "demo/Demo.hpp"
#include "demo/defines.hpp"

#include "framework/system/ErrorHandler.hpp"
#include "framework/system/TraceLogger.hpp"

#include <array>
#include <sstream>

#include <cstdint> // <= EXIT_SUCCESS
#include <cstdlib> // <= EXIT_SUCCESS

#if defined D_WEB_BUILD
#include <emscripten/emscripten.h> // <= EMSCRIPTEN_KEEPALIVE
#endif

namespace {

void validateInputs(uint32_t inWidth, uint32_t inHeight, uint32_t inTotalCores,
                    uint32_t inGenomesPerCore) {

  if (::isinf(inWidth) || ::isnan(inWidth))
    D_THROW(std::runtime_error, "argument 0 (width) is not a valid number");
  if (::isinf(inHeight) || ::isnan(inHeight))
    D_THROW(std::runtime_error, "argument 1 (height) is not a valid number");
  if (::isinf(inTotalCores) || ::isnan(inTotalCores))
    D_THROW(std::runtime_error,
            "argument 2 (totalCores) is not a valid number");
  if (::isinf(inGenomesPerCore) || ::isnan(inGenomesPerCore))
    D_THROW(std::runtime_error,
            "argument 3 (genomesPerCore) is not a valid number");

  if (inWidth < 100)
    D_THROW(std::runtime_error, "argument 0 (width) cannot be < 100");
  if (inHeight < 100)
    D_THROW(std::runtime_error, "argument 1 (height) cannot be < 100");
}

} // namespace

#if defined D_NATIVE_PTHREAD_BUILD

namespace {

void processCommandLineArgs(Demo::Definition& def, int argc, char** argv) {
  // array of pointers toward the demo definition arguments
  std::array<uint32_t*, 4> arguments{{
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
  for (std::size_t ii = 0; ii < safeSize; ++ii) {
    std::stringstream sstr;
    sstr << safeArgs.at(ii);

    uint32_t argValue;
    sstr >> argValue;

    *arguments.at(ii) = argValue;
  }
}

} // namespace

int main(int argc, char** argv) {
  Demo::Definition def;
  def.width = 800;
  def.height = 600;
  def.totalCores = 3;
  def.genomesPerCore = 90;

  processCommandLineArgs(def, argc, argv);

  validateInputs(def.width, def.height, def.totalCores, def.genomesPerCore);

  Demo myDemo(def);
  myDemo.run();

  return EXIT_SUCCESS;
}

#else

namespace /* anonymous */
{
Demo* myDemo = nullptr;
};

extern "C" {

EMSCRIPTEN_KEEPALIVE
void startDemo(uint32_t inWidth, uint32_t inHeight, uint32_t inTotalCores,
               uint32_t inGenomesPerCore) {
  if (myDemo)
    return;

  Demo::Definition def;
  def.width = inWidth;
  def.height = inHeight;
  def.totalCores = inTotalCores;
  def.genomesPerCore = inGenomesPerCore;

  validateInputs(def.width, def.height, def.totalCores, def.genomesPerCore);

  myDemo = new Demo(def);
}

EMSCRIPTEN_KEEPALIVE
void updateDemo(uint32_t inDelta) {
  if (!myDemo)
    return;

  myDemo->process(inDelta);
}
}

#endif
