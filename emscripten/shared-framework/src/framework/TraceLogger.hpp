
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <cstring> // <= strrchr()
#include <sstream> // <= std::stringstream

#include <cstdint>
#include <ostream>
#include <vector>

class TraceLogger {
public:
  static std::string getTime();

public:
  static void log(const std::string& msg);

private:
  std::stringstream _sstr;

public:
  void dump();
  std::string getData() const;

public:
  template <typename T> TraceLogger& operator<<(T data) {
    _sstr << data;
    return *this;
  }
};

template <> TraceLogger& TraceLogger::operator<<<bool>(bool data);

template <> TraceLogger& TraceLogger::operator<<<float>(float data);

template <> TraceLogger& TraceLogger::operator<<<double>(double data);

template <> TraceLogger& TraceLogger::operator<<<glm::vec2>(glm::vec2 data);
template <> TraceLogger& TraceLogger::operator<<<glm::vec3>(glm::vec3 data);
template <> TraceLogger& TraceLogger::operator<<<glm::vec4>(glm::vec4 data);
template <> TraceLogger& TraceLogger::operator<<<glm::quat>(glm::quat data);

// this will reduce the "__FILE__" macro to it's filename -> friendlier to read
#define D_MYLOG_FILENAME                                                       \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

// this is just to make the "D_MYLOG" macro source code easier to read
#define D_MYLOG_STACK D_MYLOG_FILENAME << "|" << __func__ << "|" << __LINE__

#define D_MYLOG_PREFIX                                                         \
  "MYLOG [" << TraceLogger::getTime() << "] (" << D_MYLOG_STACK << ") -> "

// one line logging macro
#define D_MYLOG(streamMsg)                                                     \
  {                                                                            \
    TraceLogger logger;                                                        \
    logger << D_MYLOG_PREFIX << streamMsg;                                     \
    logger.dump();                                                             \
  }

// TODO: std::ostream
