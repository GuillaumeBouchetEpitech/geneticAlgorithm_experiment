
#pragma once

#include "../NonCopyable.hpp"

#include "thirdparty/OpenGLES.hpp"

class SDLWindowWrapper
	: public NonCopyable
{
private:
	SDL_Window*		_window = nullptr;
	SDL_GLContext	_glContextId = 0;

	bool			_running = false;
	unsigned int	_startTime = 0;
	bool			_visible = false;

public:
	SDLWindowWrapper(int width, int height);
	virtual ~SDLWindowWrapper();

#ifdef __EMSCRIPTEN__

private:
	static void step(void* data);

#endif

public:
	void	run();
	void	stop();

public:
	void	process(unsigned int deltaTime);

protected:
	virtual void	onEvent(const SDL_Event& event) = 0;
	virtual void	onUpdate(long int deltaTime) = 0;
	virtual void	onRender(const SDL_Window& screen) = 0;
	virtual void	onResize(int width, int height) = 0;
	virtual void	onVisibilityChange(bool visible) = 0;
};

