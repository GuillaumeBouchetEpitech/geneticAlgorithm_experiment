
#pragma once

#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD

#include "State_AbstractSimulation.hpp"

#include "framework/system/Timer.hpp"

class State_WebWorkersLoading : public State_AbstractSimulation {
private:
  bool _awaiting = true;
  Timer _timer;

public:
  virtual void enter() override;

public:
  virtual void handleEvent(const SDL_Event&) override;
  virtual void update(float) override;
  virtual void render(const SDL_Window&) override;

  virtual void visibility(bool visible) override;
};

#endif
