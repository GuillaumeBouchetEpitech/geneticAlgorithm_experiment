
#include <iomanip>
#include <sstream>
#include <string>

std::string
writeTime(unsigned int time) {
  std::stringstream sstr;
  sstr << std::setw(5) << std::fixed << std::setprecision(1);

  // if (time < 1000)
  // {
  //     sstr << time << "us";
  // }
  // else
  if (time < 1000000) {
    sstr << (float(time) / 1000) << "ms";
  } else {
    sstr << (float(time) / 1000000) << "s";
  }

  return sstr.str();
}
