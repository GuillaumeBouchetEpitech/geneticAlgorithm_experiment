

#include "StateManager.hpp"

#include "State_Main.hpp"
// #include "State_RunBest.hpp"

//
//
// singleton

StateManager*	StateManager::m_pInstance = nullptr;

StateManager::StateManager()
{
	// allocate states

	m_States[e_States::eMain] = new State_Main();
	// m_States[e_States::eRunBest] = new State_RunBest();

	m_currentState = eMain;
}

StateManager::~StateManager()
{
	// deallocate states
}

//

void	StateManager::create()
{
	if (!m_pInstance)
		m_pInstance = new StateManager();
}

void	StateManager::destroy()
{
	delete m_pInstance, m_pInstance = nullptr;
}

StateManager*	StateManager::get()
{
	return m_pInstance;
}

// singleton
//
//



void	StateManager::changeState(e_States nextState)
{
	m_currentState = nextState;
}

void	StateManager::handleEvent(const SDL_Event& event)
{
	m_States[m_currentState]->handleEvent(event);
}

void	StateManager::update(int delta)
{
	m_States[m_currentState]->update(delta);
}

void	StateManager::render(const SDL_Window& window)
{
	m_States[m_currentState]->render(window);
}

void	StateManager::resize(int width, int height)
{
	m_States[m_currentState]->resize(width, height);
}
