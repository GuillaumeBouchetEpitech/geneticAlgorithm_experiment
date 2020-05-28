
#pragma once


#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD

#include "IState.hpp"

class State_WebWorkersLoading
    : public IState
{
public:
    virtual void enter() override;
    virtual void leave() override;

public:
    virtual void handleEvent(const SDL_Event&) override;
    virtual void update(int) override;
    virtual void render(const SDL_Window&) override;
    virtual void resize(int width, int height) override;
    virtual void visibility(bool visible) override;
};

#endif
