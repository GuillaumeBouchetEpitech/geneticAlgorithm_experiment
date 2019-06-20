
#include "StateManager.hpp"

#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD
#	include "State_WebWorkersLoading.hpp"
#endif

#include "State_Running.hpp"
#include "State_Paused.hpp"

//
//
// singleton

StateManager*	StateManager::_instance = nullptr;

StateManager::StateManager()
{
	// allocate states

#if defined D_WEB_WEBWORKER_BUILD
	_states[toUnderlying(States::eWorkersLoading)] = new State_WebWorkersLoading();
#endif

	_states[toUnderlying(States::eRunning)] = new State_Running();
	_states[toUnderlying(States::ePaused)] = new State_Paused();

#if defined D_WEB_WEBWORKER_BUILD
	_currentState = States::eWorkersLoading;
#else
	_currentState = States::eRunning;
#endif
}

StateManager::~StateManager()
{
	// deallocate states

	for (auto& sate : _states)
		delete sate;
}

//

void	StateManager::create()
{
	if (!_instance)
		_instance = new StateManager();
}

void	StateManager::destroy()
{
	delete _instance, _instance = nullptr;
}

StateManager*	StateManager::get()
{
	return _instance;
}

// singleton
//
//



void	StateManager::changeState(States nextState)
{
	_states[toUnderlying(_currentState)]->leave();

	_currentState = nextState;

	_states[toUnderlying(_currentState)]->enter();
}

void	StateManager::handleEvent(const SDL_Event& event)
{
	_states[toUnderlying(_currentState)]->handleEvent(event);
}

void	StateManager::update(int delta)
{
	_states[toUnderlying(_currentState)]->update(delta);
}

void	StateManager::render(const SDL_Window& window)
{
	_states[toUnderlying(_currentState)]->render(window);
}

void	StateManager::resize(int width, int height)
{
	_states[toUnderlying(_currentState)]->resize(width, height);
}

void	StateManager::visibility(bool visible)
{
	_states[toUnderlying(_currentState)]->visibility(visible);
}