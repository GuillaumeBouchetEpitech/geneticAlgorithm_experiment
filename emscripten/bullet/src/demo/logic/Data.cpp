
#include "Data.hpp"

#include "graphic/wrappers/Shader.hpp"

#include "demo/utilities/TraceLogger.hpp"
#include "demo/utilities/ErrorHandler.hpp"

#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD
#    include "demo/simulation/webworker/WebWorkersSimulation.hpp"
#else
#    include "demo/simulation/pthread/PthreadSimulation.hpp"
#endif

#include <iomanip>
#include <sstream>

//
//
// singleton

Data* Data::_instance = nullptr;

Data::~Data()
{
    delete logic.simulation;
    delete graphic.shaders.stackRenderer;
    delete graphic.shaders.wireframes;
    delete graphic.shaders.animatedCircuit;
    delete graphic.shaders.hudText;
    delete graphic.shaders.particles;
    delete graphic.shaders.model;
}

void Data::initialise()
{
    initialiseShaders();
    initialiseGeometries();
    initialiseSounds();

#if defined D_WEB_WEBWORKER_BUILD

    logic.simulation = new WebWorkersSimulation();

#else

    logic.simulation = new PthreadSimulation();

#endif

    // logic.simulation = new SimulationFacade();

    initialiseCircuit();
    initialiseSimulation();

    // logic.carsTrails.allTrails.resize(logic.simulation->getTotalCars());
    logic.carsTrails.allWheelsTrails.resize(logic.simulation->getTotalCars());

    {
        std::stringstream sstr;

        const GLubyte* glVersion = glGetString(GL_VERSION);

        std::string graphic;
        if (!glVersion)
            graphic = "unknown";
        else
        {
            graphic = reinterpret_cast<const char*>(glVersion);

            const int messageMaxSize = 30;
            if (graphic.size() > messageMaxSize)
                graphic = graphic.substr(0, messageMaxSize) + "[...]";
        }

        sstr << "Graphic: " << graphic << std::endl;

#if defined D_WEB_PTHREAD_BUILD

        sstr
            << "Type: C++ (WebAssembly Build)" << std::endl
            << "Mode: pthread";

#elif defined D_WEB_WEBWORKER_BUILD

        sstr
            << "Type: C++ (WebAssembly Build)" << std::endl
            << "Mode: webworker (as a fallback)";

#elif defined D_NATIVE_PTHREAD_BUILD

        sstr
            << "Type: C++ (Native Build)" << std::endl
            << "Mode: pthread";

#endif

        logic.hudText.header = sstr.str();
    }

#if defined D_WEB_WEBWORKER_BUILD

    {
        std::stringstream sstr;

        // sstr
        //     << "WebWorker fallback " << std::endl
        //     << "for pthread support" << std::endl
        //     << "try Chrome Desktop ";
        sstr
            << "WebWorker as a fallback " << std::endl
            << "=> for pthread support" << std::endl
            << "=> consider Chrome Desktop";

        logic.hudText.pthreadWarning = sstr.str();
    }

#endif

    graphic.hudText.renderer.initialise();
}

//

void Data::create()
{
    if (_instance)
        return;

    _instance = new Data();
    _instance->initialise();
}

void Data::destroy()
{
    delete _instance, _instance = nullptr;
}

Data& Data::get()
{
    if (!_instance)
        D_THROW(std::runtime_error, "Data singleton not initialised");

    return *_instance;
}

// singleton
//
//
