

#include "TraceLogger.hpp"

#include <iostream>
#include <iomanip>

#include <ctime>

// C++20
// C++20
// C++20

// #include <format>

// C++20
// C++20
// C++20

std::string TraceLogger::getTime()
{
  std::time_t rawtime = std::time(0);
  std::tm* now = std::localtime(&rawtime);

  std::stringstream sstr;
  sstr
    << std::setfill('0') << std::setw(2) // <= left pad (size=2)
    << now->tm_hour << ":"
    << std::setfill('0') << std::setw(2) // <= left pad (size=2)
    << now->tm_min << ":"
    << std::setfill('0') << std::setw(2) // <= left pad (size=2)
    << now->tm_sec;

  return sstr.str();

  // C++20
  // C++20
  // C++20

  // return std::format("{:0>2}:{:0>2}:{:0>2}", now->tm_hour, now->tm_min, now->tm_sec);

  // C++20
  // C++20
  // C++20
}

void TraceLogger::log(const std::string& msg)
{
  std::cout << msg << std::endl;
  // std::cerr << msg << std::endl;
}
