
#pragma once

#include "State_AbstractSimulation.hpp"

class State_EndGeneration : public State_AbstractSimulation {
private:
  int _countdownUntilNextState = 0;

public:
  virtual void enter() override;

public:
  virtual void update(int) override;
};
