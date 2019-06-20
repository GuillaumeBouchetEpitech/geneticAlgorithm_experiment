
#include "State_Paused.hpp"

#include "StateManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/logic/graphic/wrappers/Shader.hpp"
#include "demo/logic/graphic/Scene.hpp"

// #include "demo/utilities/TraceLogger.hpp"

#include "thirdparty/GLMath.hpp"

void	State_Paused::enter()
{
	// D_MYLOG("step");

	Data::get()->logic.isPaused = true;
}

void	State_Paused::leave()
{
	// D_MYLOG("step");

	Data::get()->logic.isPaused = false;
}

//

void	State_Paused::handleEvent(const SDL_Event& event)
{
	switch (event.type)
	{
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		{
			StateManager::get()->changeState(StateManager::States::eRunning);
			break;
		}
	}
}

void	State_Paused::update(int delta)
{
	static_cast<void>(delta); // <= unused
}


void	State_Paused::render(const SDL_Window& window)
{
	static_cast<void>(window); // <= unused

	Scene::renderAll();
}

void	State_Paused::resize(int width, int height)
{
	Data::get()->graphic.camera.viewportSize = { width, height };
}

void	State_Paused::visibility(bool visible)
{
	static_cast<void>(visible); // <= unused
}
