
#pragma once

#include "State_AbstractSimulation.hpp"

class State_Running : public State_AbstractSimulation {

public:
  virtual void enter() override;
  virtual void leave() override;

public:
  virtual void update(float) override;
};
