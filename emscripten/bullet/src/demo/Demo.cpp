
#include "Demo.hpp"

#include "demo/defines.hpp"
#include "demo/logic/Context.hpp"
#include "demo/logic/graphic/Scene.hpp"
#include "demo/states/StateManager.hpp"

#include "framework/system/TraceLogger.hpp"

#include <chrono>

namespace {

#if defined D_NATIVE_PTHREAD_BUILD
constexpr bool k_canResize = true;
#else
constexpr bool k_canResize = false;
#endif
} // namespace

Demo::Demo(const Definition& def)
  : SDLWindowWrapper("AI", def.width, def.height, 30,
                     SDLWindowWrapper::OpenGlEsVersion::v3, k_canResize) {
  Context::create(def.width, def.height, def.totalCores, def.genomesPerCore);
  StateManager::create();
  Scene::initialise();
}

Demo::~Demo() {
  StateManager::destroy();
  Context::destroy();
}

//

void Demo::_onEvent(const SDL_Event& event) {
  StateManager::get()->handleEvent(event);
}

void Demo::_onUpdate(uint32_t deltaTime) {
  auto startTime = std::chrono::high_resolution_clock::now();

  const float elapsedTime = float(deltaTime) / 1000.0f;

  StateManager::get()->update(elapsedTime);

  auto endTime = std::chrono::high_resolution_clock::now();
  auto microseconds =
    std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
  Context::get().logic.metrics.updateTime = microseconds.count();
}

void Demo::_onRender(const SDL_Window& screen) {
  auto startTime = std::chrono::high_resolution_clock::now();

  StateManager::get()->render(screen);

  auto endTime = std::chrono::high_resolution_clock::now();
  auto microseconds =
    std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
  Context::get().logic.metrics.renderTime = microseconds.count();
}

void Demo::_onResize(uint32_t width, uint32_t height) {
  StateManager::get()->resize(width, height);
}

void Demo::_onVisibilityChange(bool visible) {
  StateManager::get()->visibility(visible);
}
