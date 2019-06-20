
#include "Data.hpp"

#include "graphic/wrappers/Shader.hpp"

#include "demo/utilities/TraceLogger.hpp"

#include "demo/constants.hpp"


#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD
#	include "demo/simulation/webworker/WebWorkersSimulation.hpp"
#else
#	include "demo/simulation/pthread/PthreadSimulation.hpp"
#endif

#include <iomanip>
#include <sstream>

//
//
// singleton

Data* Data::_instance = nullptr;

Data::Data()
{
	initialiseShaders();
	initialiseGeometries();

#if defined D_WEB_WEBWORKER_BUILD

	logic.simulation = new WebWorkersSimulation();

#else

	logic.simulation = new PthreadSimulation();

#endif
}

Data::~Data()
{
	delete logic.simulation;
	delete graphic.shaders.basic;
	delete graphic.shaders.instanced;
	delete graphic.shaders.monoColor;
	delete graphic.shaders.animatedCircuit;
	delete graphic.shaders.hudText;
}

void	Data::initialise()
{
	initialiseCircuit();
	initialiseStates();

	logic.carsTrails.allData.resize(logic.simulation->getTotalCars());

	{
		std::stringstream sstr;

		const GLubyte* glVersion = glGetString(GL_VERSION);

		std::string graphic;
		if (!glVersion)
			graphic = "unknown";
		else
		{
			graphic = reinterpret_cast<const char*>(glVersion);
			if (graphic.size() > 30)
				graphic = graphic.substr(0, 30) + "[...]";
		}

		sstr << "Graphic: " << graphic << std::endl;

#if defined D_WEB_PTHREAD_BUILD

		sstr
			<< "Type: C++ (asm.js)" << std::endl
			<< "Mode: pthread";

#elif defined D_WEB_WEBWORKER_BUILD

		sstr
			<< "Type: C++ (asm.js)" << std::endl
			<< "Mode: webworker (as a fallback)";

#elif defined D_NATIVE_PTHREAD_BUILD

		sstr
			<< "Type: C++ (native)" << std::endl
			<< "Mode: pthread";

#endif

		logic.hudText.header = sstr.str();
	}

#if defined D_WEB_WEBWORKER_BUILD

	{
		std::stringstream sstr;

		sstr
			<< "WebWorker fallback " << std::endl
			<< "for pthread support" << std::endl
			<< "try Chrome Desktop ";

		logic.hudText.pthreadWarning = sstr.str();
	}

#endif

}

//

void	Data::create()
{
	if (_instance)
		return;

	_instance = new Data();
	_instance->initialise();
}

void	Data::destroy()
{
	delete _instance, _instance = nullptr;
}

Data*	Data::get()
{
	return _instance;
}

// singleton
//
//
