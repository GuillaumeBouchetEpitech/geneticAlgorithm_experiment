
#include "Context.hpp"

#include "demo/defines.hpp"
#if defined D_WEB_WEBWORKER_BUILD
#include "demo/logic/simulation/webworker/WebWorkersSimulation.hpp"
#else
#include "demo/logic/simulation/pthread/PthreadSimulation.hpp"
#endif

#include "framework/ErrorHandler.hpp"
#include "framework/TraceLogger.hpp"
#include "framework/graphic/GlContext.hpp"

#include <iomanip>
#include <sstream>

//
//
// singleton

Context* Context::_instance = nullptr;

Context::~Context() {}

void Context::initialise(unsigned int width, unsigned int height,
                         unsigned int totalCores, unsigned int genomesPerCore) {
  {
    graphic.camera.viewportSize = {width, height};

    graphic.camera.main.scene.setPerspective(70.0f, 0.1f, 1500.0f);

    graphic.camera.main.hud.setOrthographic(0.0f, float(width), 0.0f,
                                            float(height), -10.0f, +10.0f);
    graphic.camera.main.hud.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0),
                                   glm::vec3(0, 1, 0));
    graphic.camera.main.hud.computeMatrices();

    graphic.camera.thirdPerson.scene.setPerspective(70.0f, 0.1f, 1500.0f);
  }

  initialiseGraphicResource();

  graphic.hud.stackRenderers.wireframes.initialise(
    ShaderIds::stackRendererHud, GeometryIds::stackRendererWireframesHud);
  graphic.hud.stackRenderers.triangles.initialise(
    ShaderIds::stackRendererHud, GeometryIds::stackRendererTrianglesHud);
  graphic.scene.stackRenderers.wireframes.initialise(
    ShaderIds::stackRendererScene, GeometryIds::stackRendererWireframesScene);
  graphic.scene.stackRenderers.triangles.initialise(
    ShaderIds::stackRendererScene, GeometryIds::stackRendererTrianglesScene);
  graphic.scene.particleManager.initialise();
  graphic.hud.topologyRenderer.initialise();
  graphic.hud.textRenderer.initialise();
  graphic.hud.thirdPersonCamera.initialise();
  graphic.scene.modelsRenderer.initialise();
  graphic.scene.flockingManager.initialise();
  graphic.scene.carTailsRenderer.initialise();

  graphic.hud.postProcess.initialise({width, height});

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

#if defined D_WEB_PTHREAD_BUILD

    sstr << "Mode: pthread";

#else

    sstr << "Mode: webworker (as a fallback)";

#endif

#else

    sstr << "Type: C++ (Native Build)" << std::endl;
    sstr << "Mode: pthread";

#endif

    logic.hudText.header = sstr.str();

  } // compute the top left HUD text
}

//

void Context::create(unsigned int width, unsigned int height,
                     unsigned int totalCores, unsigned int genomesPerCore) {
  if (_instance)
    D_THROW(std::runtime_error, "Context singleton already initialised");

  _instance = new Context();
  _instance->initialise(width, height, totalCores, genomesPerCore);
}

void Context::destroy() {
  if (!_instance)
    D_THROW(std::runtime_error, "Context singleton already destroyed");

  delete _instance, _instance = nullptr;
}

Context& Context::get() {
  if (!_instance)
    D_THROW(std::runtime_error, "Context singleton not initialised");

  return *_instance;
}

// singleton
//
//
