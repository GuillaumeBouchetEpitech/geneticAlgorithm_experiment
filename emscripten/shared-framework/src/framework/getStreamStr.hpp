
#pragma once

#include <sstream>

std::string getStreamStr(const std::ostream& stream);

#define D_SSTR(stream) getStreamStr(std::ostringstream() << stream)
