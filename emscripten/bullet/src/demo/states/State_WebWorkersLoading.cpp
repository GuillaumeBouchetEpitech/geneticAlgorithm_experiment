
#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD

#include "State_WebWorkersLoading.hpp"

#include "StateManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/logic/graphic/wrappers/Shader.hpp"
#include "demo/logic/graphic/Scene.hpp"

// #include "demo/utilities/TraceLogger.hpp"

#include "thirdparty/GLMath.hpp"

void	State_WebWorkersLoading::enter()
{
	// D_MYLOG("step");
}

void	State_WebWorkersLoading::leave()
{
	// D_MYLOG("step");
}

//

void	State_WebWorkersLoading::handleEvent(const SDL_Event& event)
{
	static_cast<void>(event); // <= unused
}

void	State_WebWorkersLoading::update(int delta)
{
	static_cast<void>(delta); // <= unused

	auto&	simulation = Data::get()->logic.simulation;

	simulation->update();
}


void	State_WebWorkersLoading::render(const SDL_Window& window)
{
	static_cast<void>(window); // <= unused

	Scene::renderSimple();
}

void	State_WebWorkersLoading::resize(int width, int height)
{
	Data::get()->graphic.camera.viewportSize = { width, height };
}

void	State_WebWorkersLoading::visibility(bool visible)
{
	static_cast<void>(visible); // <= unused
}

#endif
