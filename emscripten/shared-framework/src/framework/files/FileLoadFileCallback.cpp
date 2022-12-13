
#include "FileLoadFileCallback.hpp"

#include "getFileContent.hpp"

#include "framework/ErrorHandler.hpp"

namespace FileUtils {

LoadCallback getDefaulCallback() {
  return [](const std::string& filename, std::string& outFileContent) {
    outFileContent.clear();
    if (!FileUtils::getFileContent(filename, outFileContent))
      D_THROW(std::runtime_error,
              "ERROR: Could not load file: \"" << filename << "\"");
  };
}

} // namespace FileUtils
