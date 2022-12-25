
#include "SDLWindowWrapper.hpp"

#include "framework/graphic/GlContext.hpp"

#include "framework/system/ErrorHandler.hpp"
#include "framework/system/asValue.hpp"

#include <stdexcept>

#if defined __EMSCRIPTEN__
#include <emscripten.h>
#endif

SDLWindowWrapper::SDLWindowWrapper(const char* name, uint32_t width,
                                   uint32_t height, uint32_t framesPerSecond,
                                   OpenGlEsVersion openGlEsVersion,
                                   bool enableResize /*= true*/) {
  _framesPerSecond = framesPerSecond;

  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    D_THROW(std::runtime_error,
            "Could not initialise SDL, error: " << SDL_GetError());

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, asValue(openGlEsVersion));
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

  // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  // SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);

  const int posX = SDL_WINDOWPOS_UNDEFINED;
  const int posY = SDL_WINDOWPOS_UNDEFINED;
  uint32_t flags = SDL_WINDOW_OPENGL;
  if (enableResize)
    flags |= SDL_WINDOW_RESIZABLE;

  _window = SDL_CreateWindow(name, posX, posY, int(width), int(height), flags);

  if (!_window)
    D_THROW(std::runtime_error,
            "Could not create the window: " << SDL_GetError());

  try {
    _glContextId = SDL_GL_CreateContext(_window);
    if (!_glContextId)
      D_THROW(std::runtime_error,
              "Failed to create GL context: " << SDL_GetError());

    try {
      if (SDL_GL_MakeCurrent(_window, _glContextId) < 0)
        D_THROW(std::runtime_error,
                "Failed to make GL context current: " << SDL_GetError());
    } catch (const std::exception& error) {
      SDL_GL_DeleteContext(_glContextId);

      throw error;
    }
  } catch (const std::exception& error) {
    SDL_DestroyWindow(_window);
    SDL_Quit();

    throw error;
  }

  _startTime = SDL_GetTicks();

  D_MYLOG("GL_VERSION: " << GlContext::getVersion());
  GlContext::setViewport(0, 0, width, height);
  GlContext::setDepthFunc(GlContext::DepthFuncs::less);
}

SDLWindowWrapper::~SDLWindowWrapper() {
  stop();

  if (_glContextId)
    SDL_GL_DeleteContext(_glContextId);

  if (_window)
    SDL_DestroyWindow(_window);

  SDL_Quit();
}

//

#if defined __EMSCRIPTEN__

void SDLWindowWrapper::_webStep(void* pData) {
  SDLWindowWrapper* self = static_cast<SDLWindowWrapper*>(pData);

  const uint32_t currentTime = SDL_GetTicks(); // in millisecond
  const uint32_t delta = currentTime - self->_startTime;

  self->process(delta);

  self->_startTime = currentTime;
}

#endif

void SDLWindowWrapper::run() {
  if (_running)
    return;
  _running = true;

#if defined __EMSCRIPTEN__

  // emscripten_set_main_loop_arg(SDLWindowWrapper::_webStep, (void*)this, 0,
  // true); emscripten_set_main_loop_arg(SDLWindowWrapper::_webStep,
  // (void*)this, 60, true);
  emscripten_set_main_loop_arg(SDLWindowWrapper::_webStep, (void*)this,
                               int32_t(_framesPerSecond), true);

  // unreachable <= "emscripten_set_main_loop_arg" does that

#else

  const int32_t frameDelay = 1000 / int32_t(_framesPerSecond);

  while (_running) {
    const uint32_t startFrameTime = SDL_GetTicks(); // in millisecond
    const uint32_t delta = startFrameTime - _startTime;

    process(delta);

    _startTime = startFrameTime;

    const uint32_t stopFrameTime = SDL_GetTicks(); // in millisecond
    const uint32_t elapsedFrameTime = stopFrameTime - startFrameTime;
    const int32_t mustWait = frameDelay - int32_t(elapsedFrameTime);

    if (mustWait > 0)
      SDL_Delay(uint32_t(mustWait));
  }

#endif
}

void SDLWindowWrapper::stop() {
  if (!_running)
    return;
  _running = false;

#if defined __EMSCRIPTEN__

  emscripten_cancel_main_loop();

#endif
}

//

void SDLWindowWrapper::process(uint32_t deltaTime) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {

    switch (event.type) {

#if not defined __EMSCRIPTEN__

    case SDL_QUIT: {
      return stop();
    }
    case SDL_KEYUP: {
      int symbol = event.key.keysym.sym;
      if (symbol == SDLK_ESCAPE)
        return stop();
      break;
    }

#endif

    case SDL_WINDOWEVENT: {
      switch (event.window.event) {
      case SDL_WINDOWEVENT_SIZE_CHANGED: {
        int width = 0;
        int height = 0;
        SDL_GL_GetDrawableSize(_window, &width, &height);
        if (width == 0 || height == 0)
          SDL_GetWindowSize(_window, &width, &height);

        if (width <= 0)
          width = 1;
        if (height <= 0)
          height = 1;

        _onResize(uint32_t(width), uint32_t(height));
        break;
      }
      case SDL_WINDOWEVENT_SHOWN: {
        _onVisibilityChange(_visible = true);
        break;
      }
      case SDL_WINDOWEVENT_HIDDEN: {
        _onVisibilityChange(_visible = false);
        break;
      }
      }
    }
    }

    _onEvent(event);
  }

  _onUpdate(deltaTime);

  if (!_visible)
    return;

  _onRender(*_window);

  SDL_GL_SwapWindow(_window);
}
