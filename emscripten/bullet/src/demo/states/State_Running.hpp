
#pragma once

#include "State_AbstractSimulation.hpp"

class State_Running : public State_AbstractSimulation {

public:
  virtual void update(int) override;
};
