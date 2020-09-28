
#include "State_Paused.hpp"

#include "StateManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/logic/graphic/wrappers/Shader.hpp"
#include "demo/logic/graphic/Scene.hpp"

// #include "demo/utilities/TraceLogger.hpp"

#include "thirdparty/GLMath.hpp"

void State_Paused::handleEvent(const SDL_Event& event)
{
    switch (event.type)
    {
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            StateManager::get()->changeState(Data::get().logic.state.previousState);
            break;
        }
    }
}

void State_Paused::update(int delta)
{
    static_cast<void>(delta); // <= unused
}

void State_Paused::visibility(bool visible)
{
    static_cast<void>(visible); // <= unused
}
