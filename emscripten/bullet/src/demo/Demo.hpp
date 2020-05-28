
#pragma once

#include "demo/utilities/sdl/SDLWindowWrapper.hpp"

class Demo
    : public SDLWindowWrapper
{
public:
    Demo(int width, int height);
    virtual ~Demo();

protected:
    virtual void onEvent(const SDL_Event& event) override;
    virtual void onUpdate(long int deltaTime) override;
    virtual void onRender(const SDL_Window& screen) override;
    virtual void onResize(int width, int height) override;
    virtual void onVisibilityChange(bool visible) override;
};
