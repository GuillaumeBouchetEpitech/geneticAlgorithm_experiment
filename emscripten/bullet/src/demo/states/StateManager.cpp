
#include "StateManager.hpp"

#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD
#   include "State_WebWorkersLoading.hpp"
#endif

#include "State_Running.hpp"
#include "State_Paused.hpp"
#include "State_StartGeneration.hpp"
#include "State_EndGeneration.hpp"

#include "demo/utilities/TraceLogger.hpp"

//
//
// singleton

StateManager* StateManager::_instance = nullptr;

StateManager::StateManager()
{
    // allocate states

    _states[asValue(States::Running)]           = new State_Running();
    _states[asValue(States::Paused)]            = new State_Paused();
    _states[asValue(States::StartGeneration)]   = new State_StartGeneration();
    _states[asValue(States::EndGeneration)]     = new State_EndGeneration();

#if defined D_WEB_WEBWORKER_BUILD
    _states[asValue(States::WorkersLoading)]    = new State_WebWorkersLoading();

    _currentState = States::WorkersLoading;
#else
    _currentState = States::StartGeneration;
#endif

    _states[asValue(_currentState)]->enter();
}

StateManager::~StateManager()
{
    // deallocate states

    for (auto& state : _states)
        delete state;
}

//

void StateManager::create()
{
    if (!_instance)
        _instance = new StateManager();
}

void StateManager::destroy()
{
    delete _instance, _instance = nullptr;
}

StateManager* StateManager::get()
{
    return _instance;
}

// singleton
//
//



void StateManager::changeState(States nextState)
{
    _states[asValue(_currentState)]->leave();

    _currentState = nextState;

    _states[asValue(_currentState)]->enter();
}

StateManager::States StateManager::getState() const
{
    return _currentState;
}


void StateManager::handleEvent(const SDL_Event& event)
{
    _states[asValue(_currentState)]->handleEvent(event);
}

void StateManager::update(int delta)
{
    _states[asValue(_currentState)]->update(delta);
}

void StateManager::render(const SDL_Window& window)
{
    _states[asValue(_currentState)]->render(window);
}

void StateManager::resize(int width, int height)
{
    _states[asValue(_currentState)]->resize(width, height);
}

void StateManager::visibility(bool visible)
{
    _states[asValue(_currentState)]->visibility(visible);
}
