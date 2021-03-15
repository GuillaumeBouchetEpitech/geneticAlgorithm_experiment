
#include "Data.hpp"

#include "demo/utilities/TraceLogger.hpp"
#include "demo/utilities/ErrorHandler.hpp"

#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD
#include "demo/logic/simulation/webworker/WebWorkersSimulation.hpp"
#else
#include "demo/logic/simulation/pthread/PthreadSimulation.hpp"
#endif

#include <iomanip>
#include <sstream>

//
//
// singleton

Data *Data::_instance = nullptr;

Data::~Data()
{
}

void Data::initialise(int width, int height)
{
    initialiseShaders();
    initialiseGeometries();

    graphic.camera.viewportSize = { width, height };

    //
    //
    // initialise hud frame buffer

    auto& hud = graphic.hudComponents;

    hud.frameBuffer.initialise();
    hud.frameBuffer.bind();

    hud.colorTexture.allocateBlank({ width, height }, false, false);
    hud.colorTexture.bind();
    hud.frameBuffer.attachColorTexture(hud.colorTexture);

    hud.depthRenderBuffer.allocateDepth({ width, height });
    hud.depthRenderBuffer.bind();
    hud.frameBuffer.attachDepthRenderBuffer(hud.depthRenderBuffer);

    hud.frameBuffer.executeCheck();
    FrameBuffer::unbind();

    //
    //
    //

#if defined D_WEB_WEBWORKER_BUILD

    logic.simulation = std::make_unique<WebWorkersSimulation>();

#else

    logic.simulation = std::make_unique<PthreadSimulation>();

#endif

    initialiseSimulationCallbacks();
    initialiseCircuit();

    logic.carsTrails.allWheelsTrails.resize(logic.simulation->getTotalCars());
    for (auto &trail : logic.carsTrails.allWheelsTrails)
        for (auto &wheel : trail.wheels)
            wheel.reserve(2048); // pre-allocate

    { // compute the top left HUD text

        std::stringstream sstr;

        const GLubyte *glVersion = glGetString(GL_VERSION);

        std::string graphic;
        if (!glVersion)
            graphic = "unknown";
        else
        {
            graphic = reinterpret_cast<const char *>(glVersion);

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

    } // compute the top left HUD text

#if defined D_WEB_WEBWORKER_BUILD

    { // compute the bottom right HUD text

        std::stringstream sstr;

        sstr
            << "WebWorker as a fallback " << std::endl
            << "=> for pthread support" << std::endl
            << "=> consider Chrome Desktop";

        logic.hudText.pthreadWarning = sstr.str();

    } // compute the bottom right HUD text

#endif

    graphic.hudText.renderer.initialise();

    graphic.flockingManager.initialise();
}

//

void Data::create(int width, int height)
{
    if (_instance)
        D_THROW(std::runtime_error, "Data singleton already initialised");

    _instance = new Data();
    _instance->initialise(width, height);
}

void Data::destroy()
{
    if (!_instance)
        D_THROW(std::runtime_error, "Data singleton already destroyed");

    delete _instance, _instance = nullptr;
}

Data &Data::get()
{
    if (!_instance)
        D_THROW(std::runtime_error, "Data singleton not initialised");

    return *_instance;
}

// singleton
//
//
