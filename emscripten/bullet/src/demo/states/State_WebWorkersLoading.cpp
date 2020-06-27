
#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD

#include "State_WebWorkersLoading.hpp"

#include "StateManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/logic/graphic/wrappers/Shader.hpp"
#include "demo/logic/graphic/Scene.hpp"

#include "demo/utilities/TraceLogger.hpp"

#include "thirdparty/GLMath.hpp"

void State_WebWorkersLoading::enter()
{
}

void State_WebWorkersLoading::leave()
{
}

//

void State_WebWorkersLoading::handleEvent(const SDL_Event& event)
{
    static_cast<void>(event); // <= unused
}

void State_WebWorkersLoading::update(int deltaTime)
{
    auto& logic = Data::get()->logic;

    if (logic.state.countdown == 0)
    {
        // only update to load the webworkers
        logic.simulation->update();
    }
    else
    {
        // to ensure the message is visible, what the user is wait for
        logic.state.countdown -= deltaTime;
        if (logic.state.countdown <= 0)
            StateManager::get()->changeState(StateManager::States::eStartGeneration);
    }
}


void State_WebWorkersLoading::render(const SDL_Window& window)
{
    static_cast<void>(window); // <= unused

    Scene::renderSimple();
}

void State_WebWorkersLoading::resize(int width, int height)
{
    Data::get()->graphic.camera.viewportSize = { width, height };
}

void State_WebWorkersLoading::visibility(bool visible)
{
    if (!visible) {
        Data::get()->logic.state.previousState = StateManager::States::eWorkersLoading;
        StateManager::get()->changeState(StateManager::States::ePaused);
    }
}

#endif
