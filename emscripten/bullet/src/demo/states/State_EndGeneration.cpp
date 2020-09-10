
#include "State_EndGeneration.hpp"

#include "StateManager.hpp"

#include "demo/logic/Data.hpp"

void State_EndGeneration::enter()
{
    Data::get().logic.state.countdown = 750; // wait 0.75 seconds
}

void State_EndGeneration::update(int deltaTime)
{
    State_AbstractSimulation::update(deltaTime);

    Data::get().logic.state.countdown -= deltaTime;
    if (Data::get().logic.state.countdown <= 0)
        StateManager::get()->changeState(StateManager::States::StartGeneration);
}
