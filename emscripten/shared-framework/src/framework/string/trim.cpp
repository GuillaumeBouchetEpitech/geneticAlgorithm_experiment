
#include "trim.hpp"

#include <algorithm>

namespace StringUtils {

const std::string& preTrim(std::string& str) {
  auto isNotSpace = [](int c) { return !std::isspace(c); };
  str.erase(str.begin(), std::find_if(str.begin(), str.end(), isNotSpace));
  return str;
}

const std::string& postTrim(std::string& str) {
  auto isNotSpace = [](int c) { return !std::isspace(c); };
  str.erase(std::find_if(str.rbegin(), str.rend(), isNotSpace).base(),
            str.end());
  return str;
}

const std::string& trim(std::string& str) {
  preTrim(str);
  postTrim(str);
  return str;
}

} // namespace StringUtils
