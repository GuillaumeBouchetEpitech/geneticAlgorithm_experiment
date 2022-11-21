
#include "State_EndGeneration.hpp"

#include "StateManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/logic/graphic/Scene.hpp"

void State_EndGeneration::enter()
{
    _countdown = 750; // wait 0.75 seconds
}

void State_EndGeneration::update(int deltaTime)
{
    State_AbstractSimulation::update(deltaTime);

    auto& data = Data::get();
    auto& graphic = data.graphic;

    float elapsedTime = float(deltaTime) / 1000.0f;

    Scene::updateMatrices(elapsedTime);

    {
        graphic.particleManager.update(elapsedTime);
        graphic.backGroundCylindersRenderer.update(elapsedTime);
        graphic.flockingManager.update();
    }

    _countdown -= deltaTime;
    if (_countdown <= 0)
        StateManager::get()->changeState(StateManager::States::StartGeneration);
}
