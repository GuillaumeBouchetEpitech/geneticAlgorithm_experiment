


#ifndef D_TRACELOGGER_HPP
#define D_TRACELOGGER_HPP



#include <sstream>
#include <cstring> // -> __FILENAME__




#include <iostream>

std::string getTime();



#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define D_STACK "(" << __FILENAME__ << "|" << __func__ << "|" << __LINE__ << ")"

#define D_MYLOG(msg)\
{\
	std::stringstream sstr;\
	sstr << "MYLOG [" << getTime() << "] " << D_STACK << " -> " << msg;\
	std::cout << sstr.str() << std::endl;\
}

// #define D_MYLOG(msg)


#endif // D_TRACELOGGER_HPP


