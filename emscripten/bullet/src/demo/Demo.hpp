
#pragma once

#include "demo/utilities/sdl/SDLWindowWrapper.hpp"

class Demo
    : public SDLWindowWrapper
{
public:
    Demo(int width, int height);
    virtual ~Demo();

protected:
    virtual void _onEvent(const SDL_Event& event) override;
    virtual void _onUpdate(long int deltaTime) override;
    virtual void _onRender(const SDL_Window& screen) override;
    virtual void _onResize(int width, int height) override;
    virtual void _onVisibilityChange(bool visible) override;
};
