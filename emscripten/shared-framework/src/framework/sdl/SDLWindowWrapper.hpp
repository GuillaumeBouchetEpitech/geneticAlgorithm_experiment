
#pragma once

#include "framework/NonCopyable.hpp"
#include "framework/helpers/SDL.hpp"

#include <cstdint>
#include <unordered_map>

class SDLWindowWrapper
  : public NonCopyable
{
public:
  enum class OpenGlEsVersion
  {
    v2 = 2, // OpenGl ES2 -> WebGL1
    v3 = 3, // OpenGl ES3 -> WebGL2
  };

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
    uint32_t framesPerSecond,
    OpenGlEsVersion openGlEsVersion);
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
  virtual void _onEvent(const SDL_Event& event) = 0; // TODO: convert event?
  virtual void _onUpdate(uint32_t deltaTime) = 0;
  virtual void _onRender(const SDL_Window& screen) = 0; // TODO: remove window?
  virtual void _onResize(uint32_t width, uint32_t height) = 0;
  virtual void _onVisibilityChange(bool visible) = 0;

//   // TODO
//   // TODO
//   // TODO

// public:
//   struct Input
//   {
//     struct Keyboard
//     {
//       std::unordered_map<int, bool> keys;
//     }
//     keyboard;

//     struct Mouse
//     {
//       glm::vec2 position = { 0, 0 };
//       glm::vec2 delta = { 0, 0 };

//       struct Buttons
//       {
//         bool left = false;
//         bool middle = false;
//         bool right = false;
//       }
//       buttons;

//       // std::unordered_map<int, bool> buttons;
//       // std::unordered_map<int, bool> prevButtons;
//     }
//     mouse;
//   }
//   _inputs;


//   struct MouseEvent
//   {
//   };

// protected:
//   virtual void _onMouseEvent(const SDL_Event& event) = 0;
};

