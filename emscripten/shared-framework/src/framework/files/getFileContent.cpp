
#include <fstream>
#include <sstream>

namespace FileUtils {
bool getFileContent(const std::string& filename, std::string& source) {
  std::ifstream istr(filename);

  if (istr.fail())
    return false;

  std::stringstream sstr;
  sstr << istr.rdbuf();

  source.clear();

  source += sstr.str();

  return true;
}
} // namespace FileUtils
