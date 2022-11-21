
#pragma once

#include "framework/NonCopyable.hpp"
#include "framework/helpers/SDL.hpp"

#include <cstdint>

class SDLWindowWrapper
  : public NonCopyable
{
private:
  SDL_Window* _window = nullptr;
  SDL_GLContext _glContextId = 0;

  uint32_t _framesPerSecond = 0;

  bool _running = false;
  uint32_t _startTime = 0;
  bool _visible = false;

public:
  SDLWindowWrapper(
    const char* name,
    uint32_t width,
    uint32_t height,
    uint32_t framesPerSecond);
  virtual ~SDLWindowWrapper();

#ifdef __EMSCRIPTEN__

private:
  static void _webStep(void* data);

#endif

public:
  void run();
  void stop();

public:
  void process(uint32_t deltaTime);

protected:
  virtual void _onEvent(const SDL_Event& event) = 0;
  virtual void _onUpdate(uint32_t deltaTime) = 0;
  virtual void _onRender(const SDL_Window& screen) = 0;
  virtual void _onResize(uint32_t width, uint32_t height) = 0;
  virtual void _onVisibilityChange(bool visible) = 0;
};

