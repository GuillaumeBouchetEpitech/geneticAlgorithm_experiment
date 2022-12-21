
#pragma once

#include "TraceLogger.hpp"

// one line exception macro
#define D_THROW(exceptionType, exceptionMsg)                                   \
  {                                                                            \
    TraceLogger logger;                                                        \
    logger << D_MYLOG_PREFIX << "[EXCEPTION] type: " #exceptionType            \
           << ", msg: " << exceptionMsg;                                       \
    logger.dump();                                                             \
    throw exceptionType(logger.getData());                                     \
  }
