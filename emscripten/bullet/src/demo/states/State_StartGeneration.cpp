
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
  auto& graphic = Context::get().graphic;
  graphic.hud.screenTitles.fadeOut(0.0f, 0.5f);
  graphic.scene.modelsRenderer.fadeIn(0.25f, 1.0f);
}

void State_StartGeneration::update(float elapsedTime) {
  State_AbstractSimulation::update(elapsedTime);

  _updateCommonLogic(elapsedTime);
  _updateCameraTracking(elapsedTime);

  _timer.update(elapsedTime);
  if (_timer.isDone()) {
    StateManager::get()->changeState(StateManager::States::Running);
  }
}
