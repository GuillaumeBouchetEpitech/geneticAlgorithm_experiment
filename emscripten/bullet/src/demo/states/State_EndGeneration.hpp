
#pragma once

#include "State_AbstractSimulation.hpp"

#include "framework/system/Timer.hpp"

class State_EndGeneration : public State_AbstractSimulation {
private:
  Timer _timer;

public:
  virtual void enter() override;

public:
  virtual void update(int) override;
};
