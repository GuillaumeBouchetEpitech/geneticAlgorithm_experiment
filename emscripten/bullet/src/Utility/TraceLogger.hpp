

#pragma once


#include <sstream>
#include <cstring> // -> __FILENAME__


#include <iostream>
#include <iomanip>

std::string getTime();



#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define D_STACK "(" << __FILENAME__ << "|" << __func__ << "|" << __LINE__ << ")"

#define D_MYLOG(msg)\
{\
	std::stringstream sstr;\
	sstr << "MYLOG [" << getTime() << "] " << D_STACK << " -> " << msg;\
	std::cout << sstr.str() << std::endl;\
}


