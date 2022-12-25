
#pragma once

#include "State_AbstractSimulation.hpp"

#include "framework/system/Timer.hpp"

class State_StartGeneration : public State_AbstractSimulation {
private:
  Timer _timer;

public:
  virtual void enter() override;
  virtual void leave() override;

public:
  virtual void update(float) override;
};
