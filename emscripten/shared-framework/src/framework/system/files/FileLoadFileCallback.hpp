
#pragma once

#include <functional>
#include <string>

namespace FileUtils {

using LoadCallback =
  std::function<void(const std::string& filename, std::string& source)>;

LoadCallback getDefaulCallback();

} // namespace FileUtils
