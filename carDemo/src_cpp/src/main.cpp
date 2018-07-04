


#include "Experiment/Experiment.hpp"

#include "Experiment/Simulation/utils/TraceLogger.hpp"


#include <typeinfo> // <- for the __cxa_exception_type()->name()


int	main()
{
	try
	{
		Experiment	my_experiment;
		my_experiment.run();
	}
	catch (...)
	{
        std::exception_ptr p = std::current_exception();
        D_MYLOG("/!\\ Exception, name=" << (p ? p.__cxa_exception_type()->name() : "null"));
	}
}


