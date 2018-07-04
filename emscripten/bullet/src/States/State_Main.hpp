

#pragma once


#include "IState.hpp"


class State_Main : public IState
{
public:
	State_Main();
	~State_Main();

public:
	virtual void	handleEvent(const SDL_Event&) override;
	virtual void	update(int) override;
	virtual void	render(const SDL_Window&) override;
	virtual void	resize(int width, int height) override;
};
