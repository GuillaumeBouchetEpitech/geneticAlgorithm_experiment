

#include "TraceLogger.hpp"

#include <iomanip>

#include <ctime>


std::string getTime()
{
	time_t	rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	std::stringstream sstr;
	sstr
		<< std::setfill('0') << std::setw(2) // <= left pad (size=2)
		<< timeinfo->tm_hour << ":"
		<< std::setfill('0') << std::setw(2) // <= left pad (size=2)
		<< timeinfo->tm_min << ":"
		<< std::setfill('0') << std::setw(2) // <= left pad (size=2)
		<< timeinfo->tm_sec;

	return sstr.str();
}
