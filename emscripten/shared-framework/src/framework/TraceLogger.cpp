

#include "TraceLogger.hpp"

#include "framework/ErrorHandler.hpp"

#include <iostream>
// #include <sstream>
#include <iomanip>
#include <memory>

#include <ctime>

// C++20
// C++20
// C++20

// #include <format>

// C++20
// C++20
// C++20

std::string TraceLogger::getTime() {
  std::time_t rawtime = std::time(0);
  std::tm* now = std::localtime(&rawtime);

  std::stringstream sstr;
  sstr << std::setfill('0') << std::setw(2) // <= left pad (size=2)
       << now->tm_hour << ":" << std::setfill('0')
       << std::setw(2) // <= left pad (size=2)
       << now->tm_min << ":" << std::setfill('0')
       << std::setw(2) // <= left pad (size=2)
       << now->tm_sec;

  return sstr.str();

  // C++20
  // C++20
  // C++20

  // return std::format("{:0>2}:{:0>2}:{:0>2}", now->tm_hour, now->tm_min,
  // now->tm_sec);

  // C++20
  // C++20
  // C++20
}

void TraceLogger::log(const std::string& msg) {
  std::cout << msg << std::endl;
  // std::cerr << msg << std::endl;
}

void TraceLogger::dump() { std::cout << _sstr.str() << std::endl; }

std::string TraceLogger::getData() const { return _sstr.str(); }

template <> TraceLogger& TraceLogger::operator<<<bool>(bool data) {
  _sstr << std::boolalpha << data;
  return *this;
}

template <> TraceLogger& TraceLogger::operator<<<float>(float data) {
  _sstr << std::fixed << std::setprecision(2) << data;
  return *this;
}

template <> TraceLogger& TraceLogger::operator<<<double>(double data) {
  _sstr << std::fixed << std::setprecision(2) << data;
  return *this;
}

template <> TraceLogger& TraceLogger::operator<<<glm::vec2>(glm::vec2 data) {
  _sstr << "[";
  (*this) << data.x;
  _sstr << " / ";
  (*this) << data.y;
  _sstr << "]";

  return *this;
}

template <> TraceLogger& TraceLogger::operator<<<glm::vec3>(glm::vec3 data) {
  _sstr << "[";
  (*this) << data.x;
  _sstr << " / ";
  (*this) << data.y;
  _sstr << " / ";
  (*this) << data.z;
  _sstr << "]";

  return *this;
}

template <> TraceLogger& TraceLogger::operator<<<glm::vec4>(glm::vec4 data) {
  _sstr << "[";
  (*this) << data.x;
  _sstr << " / ";
  (*this) << data.y;
  _sstr << " / ";
  (*this) << data.z;
  _sstr << " / ";
  (*this) << data.w;
  _sstr << "]";

  return *this;
}

template <> TraceLogger& TraceLogger::operator<<<glm::quat>(glm::quat data) {
  _sstr << "[";
  (*this) << data.x;
  _sstr << " / ";
  (*this) << data.y;
  _sstr << " / ";
  (*this) << data.z;
  _sstr << " / ";
  (*this) << data.w;
  _sstr << "]";

  return *this;
}
