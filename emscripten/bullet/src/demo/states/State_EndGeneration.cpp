
#include "State_EndGeneration.hpp"

#include "StateManager.hpp"

#include "demo/logic/Context.hpp"
#include "demo/logic/graphic/Scene.hpp"

void State_EndGeneration::enter()
{
    _countdownUntilNextState = 750; // wait 0.75 seconds
}

void State_EndGeneration::update(int deltaTime)
{
    State_AbstractSimulation::update(deltaTime);

    auto& graphic = Context::get().graphic;

    float elapsedTime = float(deltaTime) / 1000.0f;

    Scene::updateMatrices(elapsedTime);

    {
        graphic.particleManager.update(elapsedTime);
        graphic.backGroundCylindersRenderer.update(elapsedTime);
        graphic.flockingManager.update();
    }

    _countdownUntilNextState -= deltaTime;
    if (_countdownUntilNextState <= 0)
        StateManager::get()->changeState(StateManager::States::StartGeneration);
}
