
#pragma once

#include "framework/helpers/SDL.hpp" // SDL_Event + SDL_Window
#include "framework/system/NonCopyable.hpp"

class IState : public NonCopyable {
public:
  virtual ~IState(){};

public:
  virtual void enter() = 0;
  virtual void leave() = 0;

public:
  virtual void handleEvent(const SDL_Event&) = 0;
  virtual void update(float) = 0;
  virtual void render(const SDL_Window&) = 0;
  virtual void resize(int, int) = 0;
  virtual void visibility(bool) = 0;
};
