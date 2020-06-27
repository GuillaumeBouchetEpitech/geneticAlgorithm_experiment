
#include "Demo.hpp"

#include "states/StateManager.hpp"

#include "demo/logic/Data.hpp"

#include "demo/utilities/TraceLogger.hpp"

#include <chrono>

#ifdef __EMSCRIPTEN__
#    include <emscripten.h>
#endif

Demo::Demo(int width, int height)
    : SDLWindowWrapper(width, height)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDisable(GL_CULL_FACE);
    // glEnable(GL_CULL_FACE);

    Data::create();
    Data::get()->graphic.camera.viewportSize = {width, height};

    StateManager::create();

    // hacky?
    Data::get()->logic.state.previousState = StateManager::get()->getState();
}

Demo::~Demo()
{
    StateManager::destroy();
    Data::destroy();
}

//

void Demo::onEvent(const SDL_Event& event)
{
    StateManager::get()->handleEvent(event);
}

void Demo::onUpdate(long int deltaTime)
{
    auto startTime = std::chrono::high_resolution_clock::now();

    StateManager::get()->update(deltaTime);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    Data::get()->logic.metrics.updateTime = microseconds.count();
}

void Demo::onRender(const SDL_Window& screen)
{
    auto startTime = std::chrono::high_resolution_clock::now();

    StateManager::get()->render(screen);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    Data::get()->logic.metrics.renderTime = microseconds.count();
}

void Demo::onResize(int width, int height)
{
    StateManager::get()->resize(width, height);
}

void Demo::onVisibilityChange(bool visible)
{
    StateManager::get()->visibility(visible);
}

