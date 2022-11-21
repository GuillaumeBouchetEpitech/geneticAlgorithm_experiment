
#pragma once

#include "State_AbstractSimulation.hpp"

class State_EndGeneration
    : public State_AbstractSimulation
{
private:
    int _countdown = 0;

public:
    virtual void enter() override;

public:
    virtual void update(int) override;
};
