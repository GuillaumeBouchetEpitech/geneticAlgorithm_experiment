

#include "TraceLogger.hpp"

#include <iostream>
#include <iomanip>

#include <ctime>

std::string TraceLogger::getTime()
{
    time_t rawtime;
    struct tm* timeinfo;
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

void TraceLogger::log(const std::string& msg)
{
    std::cout << msg << std::endl;
    // std::cerr << msg << std::endl;
}
