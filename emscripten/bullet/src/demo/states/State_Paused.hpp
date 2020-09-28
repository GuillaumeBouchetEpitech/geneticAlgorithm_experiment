
#pragma once

#include "State_AbstractSimulation.hpp"

class State_Paused
    : public State_AbstractSimulation
{
public:
    virtual void handleEvent(const SDL_Event&) override;
    virtual void update(int) override;
    virtual void visibility(bool visible) override;
};
