

#pragma once


#include "OpenGLES.hpp"


class IState
{
public:
	virtual void	handleEvent(const SDL_Event&) = 0;
	virtual void	update(int) = 0;
	virtual void	render(const SDL_Window&) = 0;
	virtual void	resize(int, int) = 0;
};
