
#include "State_EndGeneration.hpp"

#include "StateManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/logic/graphic/Scene.hpp"

void State_EndGeneration::enter()
{
    Data::get().logic.state.countdown = 750; // wait 0.75 seconds
}

void State_EndGeneration::update(int deltaTime)
{
    State_AbstractSimulation::update(deltaTime);

    auto& data = Data::get();
    auto& logic = data.logic;
    auto& graphic = data.graphic;

    float elapsedTime = float(deltaTime) / 1000.0f;

    Scene::updateMatrices(elapsedTime);

    {
        graphic.particleManager.update(elapsedTime);
        graphic.flockingManager.update(elapsedTime);
    }

    logic.state.countdown -= deltaTime;
    if (logic.state.countdown <= 0)
        StateManager::get()->changeState(StateManager::States::StartGeneration);
}
