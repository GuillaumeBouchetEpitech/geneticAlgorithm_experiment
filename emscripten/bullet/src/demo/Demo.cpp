
#include "Demo.hpp"

#include "states/StateManager.hpp"

#include "demo/logic/Data.hpp"

#include "demo/utilities/TraceLogger.hpp"

#include <chrono>

#ifdef __EMSCRIPTEN__
#	include <emscripten.h>
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

	Data::create();
	Data::get()->graphic.camera.viewportSize = {width, height};

	StateManager::create();
}

Demo::~Demo()
{
	StateManager::destroy();
	Data::destroy();
}

//

void	Demo::onEvent(const SDL_Event& event)
{
	StateManager::get()->handleEvent(event);
}

void	Demo::onUpdate(long int deltaTime)
{
	auto startTime = std::chrono::high_resolution_clock::now();

// #ifdef __EMSCRIPTEN__
	// EM_ASM(myFpsmeterUpdate.tickStart(););
// #endif

	StateManager::get()->update(deltaTime);

// #ifdef __EMSCRIPTEN__
	// EM_ASM(myFpsmeterUpdate.tick(););
// #endif

	auto endTime = std::chrono::high_resolution_clock::now();
	auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
	Data::get()->logic.metrics.updateTime = microseconds.count();
}

void	Demo::onRender(const SDL_Window& screen)
{
	auto startTime = std::chrono::high_resolution_clock::now();

// #ifdef __EMSCRIPTEN__
	// EM_ASM(myFpsmeterRender.tickStart(););
// #endif

	StateManager::get()->render(screen);

// #ifdef __EMSCRIPTEN__
	// EM_ASM(myFpsmeterRender.tick(););
// #endif

	auto endTime = std::chrono::high_resolution_clock::now();
	auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
	Data::get()->logic.metrics.renderTime = microseconds.count();
}

void	Demo::onResize(int width, int height)
{
	// D_MYLOG("resize=" << width << "/" << height);
	// if (width < 800)
	// 	width = 800;
	// if (height < 600)
	// 	height = 600;
	StateManager::get()->resize(width, height);
}

void	Demo::onVisibilityChange(bool visible)
{
	StateManager::get()->visibility(visible);
}

