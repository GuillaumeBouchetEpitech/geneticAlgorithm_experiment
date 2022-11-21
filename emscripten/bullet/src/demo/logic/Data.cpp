
#include "Data.hpp"

#include "framework/TraceLogger.hpp"
#include "framework/ErrorHandler.hpp"

#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD
#include "demo/logic/simulation/webworker/WebWorkersSimulation.hpp"
#else
#include "demo/logic/simulation/pthread/PthreadSimulation.hpp"
#endif

#include "framework/graphic/ResourceManager.hpp"

#include "framework/graphic/GlContext.hpp"

#include <iomanip>
#include <sstream>

//
//
// singleton

Data* Data::_instance = nullptr;

Data::~Data()
{
    ResourceManager::destroy();
}

void Data::initialise(unsigned int width, unsigned int height, unsigned int totalCores, unsigned int genomesPerCore)
{
    ResourceManager::create();

    graphic.camera.viewportSize = { width, height };

    initialiseShaders();
    initialiseTextures();
    initialiseGeometries();

    graphic.stackRenderer.initialise();
    graphic.particleManager.initialise();
    graphic.textRenderer.initialise();
    graphic.modelsRenderer.initialise();
    graphic.flockingManager.initialise();
    graphic.carTailsRenderer.initialise();

    //
    //
    // initialise hud frame buffer

    auto& hud = graphic.hudComponents;

    hud.frameBuffer.initialise();
    hud.frameBuffer.bind();

    hud.colorTexture.allocateBlank({ width, height }, true, false);
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
    initialiseSimulation(totalCores, genomesPerCore);

    { // compute the top left HUD text

        std::string glVersion = GlContext::getVersion();
        if (glVersion.empty())
            glVersion = "unknown";

        std::stringstream sstr;
        sstr << "Graphic: " << glVersion << std::endl;

#if defined D_WEB_BUILD

        sstr << "Type: C++ (WebAssembly Build)" << std::endl;

#  if defined D_WEB_PTHREAD_BUILD

        sstr << "Mode: pthread";

#  else

        sstr << "Mode: webworker (as a fallback)";

#  endif

#else

        sstr << "Type: C++ (Native Build)" << std::endl;
        sstr << "Mode: pthread";

#endif

        logic.hudText.header = sstr.str();

    } // compute the top left HUD text
}

//

void Data::create(unsigned int width, unsigned int height, unsigned int totalCores, unsigned int genomesPerCore)
{
    if (_instance)
        D_THROW(std::runtime_error, "Data singleton already initialised");

    _instance = new Data();
    _instance->initialise(width, height, totalCores, genomesPerCore);
}

void Data::destroy()
{
    if (!_instance)
        D_THROW(std::runtime_error, "Data singleton already destroyed");

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
