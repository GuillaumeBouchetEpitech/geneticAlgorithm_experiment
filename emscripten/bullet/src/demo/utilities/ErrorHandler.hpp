
#pragma once

#include "TraceLogger.hpp"

// one line macro allowing flexible logs before throwing any exception type
#define D_THROW(exceptionType, exceptionMsg) \
{ \
    std::string log; \
    D_MYLOG_MAKE_PREFIXED_STRING(log, "[EXCEPTION] type: "#exceptionType << ", msg: " << exceptionMsg) \
    TraceLogger::log(log); \
    throw exceptionType(log); \
}
