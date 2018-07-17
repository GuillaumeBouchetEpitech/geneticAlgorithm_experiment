
#ifndef EMSCRIPTEN
#	error "=> native currently unsupported, the webworker feature must be replicated first (with multithreading)"
#endif

#include "Experiment.hpp"

int main ()
{
	{
		Experiment	myExperiment;
		myExperiment.run();
	}

	return 0;
}
