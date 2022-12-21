
#pragma once

#include "framework/graphic/sdl/SDLWindowWrapper.hpp"

#include <cstdint>

class Demo : public SDLWindowWrapper {
public:
  struct Definition {
    uint32_t totalCores = 0;
    uint32_t genomesPerCore = 0;
    uint32_t width = 0;
    uint32_t height = 0;
  };

public:
  Demo(const Definition& def);
  virtual ~Demo();

protected:
  virtual void _onEvent(const SDL_Event& event) override;
  virtual void _onUpdate(uint32_t deltaTime) override;
  virtual void _onRender(const SDL_Window& screen) override;
  virtual void _onResize(uint32_t width, uint32_t height) override;
  virtual void _onVisibilityChange(bool visible) override;
};
