
#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD

#include "State_WebWorkersLoading.hpp"

#include "StateManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/logic/graphic/Scene.hpp"

void State_WebWorkersLoading::enter()
{
    Data::get().logic.state.countdown = 0;
}

void State_WebWorkersLoading::handleEvent(const SDL_Event& event)
{
    static_cast<void>(event); // <= unused
}

void State_WebWorkersLoading::update(int deltaTime)
{
    auto& logic = Data::get().logic;

    if (logic.state.countdown == 0)
    {
        // only update to load the webworkers
        logic.simulation->update();
    }
    else
    {
        // to ensure the message is visible, what the user is waiting for
        logic.state.countdown -= deltaTime;
        if (logic.state.countdown <= 0)
            StateManager::get()->changeState(StateManager::States::StartGeneration);
    }
}

void State_WebWorkersLoading::render(const SDL_Window& window)
{
    static_cast<void>(window); // <= unused

    Scene::renderSimple();
}

#endif
