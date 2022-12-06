
#pragma once

#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD

#include "State_AbstractSimulation.hpp"

class State_WebWorkersLoading : public State_AbstractSimulation {
private:
  int _countdownUntilNextState = 0;

public:
  virtual void enter() override;

public:
  virtual void handleEvent(const SDL_Event&) override;
  virtual void update(int) override;
  virtual void render(const SDL_Window&) override;

  virtual void visibility(bool visible) override;
};

#endif
