
#include "Demo.hpp"

#include "states/StateManager.hpp"

#include "demo/logic/Data.hpp"

#include "demo/utilities/TraceLogger.hpp"

#include "demo/logic/graphic/Scene.hpp"

#include <chrono>

Demo::Demo(const Definition& def)
    : SDLWindowWrapper(def.width, def.height)
{
    Data::create(def.width, def.height, def.totalCores, def.genomesPerCore);

    StateManager::create();

    // hacky?
    Data::get().logic.state.previousState = StateManager::get()->getState();

    Scene::initialise();
}

Demo::~Demo()
{
    StateManager::destroy();
    Data::destroy();
}

//

void Demo::_onEvent(const SDL_Event& event)
{
    StateManager::get()->handleEvent(event);
}

void Demo::_onUpdate(long int deltaTime)
{
    auto startTime = std::chrono::high_resolution_clock::now();

    StateManager::get()->update(deltaTime);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    Data::get().logic.metrics.updateTime = microseconds.count();
}

void Demo::_onRender(const SDL_Window& screen)
{
    auto startTime = std::chrono::high_resolution_clock::now();

    StateManager::get()->render(screen);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    Data::get().logic.metrics.renderTime = microseconds.count();
}

void Demo::_onResize(int width, int height)
{
    StateManager::get()->resize(width, height);
}

void Demo::_onVisibilityChange(bool visible)
{
    StateManager::get()->visibility(visible);
}

