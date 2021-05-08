
#pragma once

#include "demo/utilities/sdl/SDLWindowWrapper.hpp"

class Demo
    : public SDLWindowWrapper
{
public:
    struct Definition
    {
        unsigned int totalCores;
        unsigned int genomesPerCore;
        unsigned int width;
        unsigned int height;
    };

public:
    Demo(const Definition& def);
    virtual ~Demo();

protected:
    virtual void _onEvent(const SDL_Event& event) override;
    virtual void _onUpdate(long int deltaTime) override;
    virtual void _onRender(const SDL_Window& screen) override;
    virtual void _onResize(int width, int height) override;
    virtual void _onVisibilityChange(bool visible) override;
};
