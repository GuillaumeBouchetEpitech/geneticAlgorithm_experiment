

#include "TraceLogger.hpp"

#include <ctime>


std::string getTime()
{
	time_t * rawtime = new time_t;
	struct tm * timeinfo;
	time(rawtime);
	timeinfo = localtime(rawtime);

               // int tm_sec;         /* seconds */
               // int tm_min;         /* minutes */
               // int tm_hour;        /* hours */

	std::stringstream sstr;
	// sstr << asctime(timeinfo);
	sstr
		<< timeinfo->tm_hour << ":"
		<< timeinfo->tm_min << ":"
		<< timeinfo->tm_sec;

	return sstr.str();
}
