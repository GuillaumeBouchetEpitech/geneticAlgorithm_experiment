
#pragma once

#include <sstream> // <= std::stringstream
#include <cstring> // <= strrchr()

#include <ostream>

class TraceLogger
{
public:
    static std::string getTime();

public:
    static void log(const std::string& msg);
};

// this will reduce the "__FILE__" macro to it's filename -> friendlier to read
#define D_MYLOG_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

// this is just to make the "D_MYLOG" macro source code easier to read
#define D_MYLOG_STACK D_MYLOG_FILENAME << "|" << __func__ << "|" << __LINE__

#define D_MYLOG_PREFIX "MYLOG [" << TraceLogger::getTime() << "] (" << D_MYLOG_STACK << ") -> "

// one line macro allowing flexible logs with the current time and "stacktrace"
#define D_MYLOG_MAKE_PREFIXED_STRING(resultString, streamMsg) \
{ \
    std::stringstream sstr; \
    sstr << D_MYLOG_PREFIX << streamMsg; \
    resultString = sstr.str(); \
}

// one line logging macro
#define D_MYLOG(streamMsg) \
{ \
    std::string log; \
    D_MYLOG_MAKE_PREFIXED_STRING(log, streamMsg) \
    TraceLogger::log(log); \
}
