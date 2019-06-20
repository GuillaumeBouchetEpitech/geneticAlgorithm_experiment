
#pragma once

#include "TraceLogger.hpp"

#define	D_THROW(exceptionType, exceptionMsg) \
{ \
	std::string	log; \
	D_MYLOG_MAKE_PREFIXED_STRING(log, "[EXCEPTION] type: "#exceptionType << ", msg: " << exceptionMsg) \
	D_MYLOG_MAKE_LOG(log) \
	throw exceptionType(log); \
}
