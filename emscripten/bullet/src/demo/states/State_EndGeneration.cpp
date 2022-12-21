
#include "State_EndGeneration.hpp"

#include "StateManager.hpp"

#include "demo/logic/Context.hpp"
#include "demo/logic/graphic/Scene.hpp"

void State_EndGeneration::enter() { _timer.start(0.75f); }

void State_EndGeneration::update(int deltaTime) {
  State_AbstractSimulation::update(deltaTime);

  const float elapsedTime = float(deltaTime) / 1000.0f;

  _updateCommonLogic(elapsedTime);

  _timer.update(elapsedTime);
  if (_timer.isDone()) {
    StateManager::get()->changeState(StateManager::States::StartGeneration);
  }
}
