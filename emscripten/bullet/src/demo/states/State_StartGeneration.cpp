
#include "State_StartGeneration.hpp"

#include "StateManager.hpp"

#include "demo/logic/Context.hpp"
#include "demo/logic/graphic/Scene.hpp"

#include "framework/helpers/GLMath.hpp"

#include <array>
#include <iomanip>

void State_StartGeneration::enter() {
  auto& context = Context::get();

  const float lastFitness = context.logic.fitnessStats.get(-1);
  if (lastFitness > 0.0f) {
    _timer.start(2.5f);
  } else {
    _timer.start(1.5f);
  }
  context.graphic.hud.screenTitles.fadeIn(0.0f, 0.75f);
}

void State_StartGeneration::leave() {
  auto& context = Context::get();
  context.graphic.hud.screenTitles.fadeOut(0.0f, 0.5f);
}

void State_StartGeneration::update(int deltaTime) {
  State_AbstractSimulation::update(deltaTime);

  const float elapsedTime = float(deltaTime) / 1000.0f;

  _updateCommonLogic(elapsedTime);
  _updateCameraTracking(elapsedTime);

  _timer.update(elapsedTime);
  if (_timer.isDone()) {
    StateManager::get()->changeState(StateManager::States::Running);
  }
}
