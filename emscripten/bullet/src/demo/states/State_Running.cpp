
#include "demo/defines.hpp"

#include "StateManager.hpp"
#include "State_Running.hpp"

#include "demo/logic/Context.hpp"
#include "demo/logic/graphic/Scene.hpp"

#include "framework/helpers/GLMath.hpp"

#include <cmath>  // std::ceil
#include <limits> // std::numeric_limits<T>::max();

void
State_Running::enter() {
  auto& graphic = Context::get().graphic;
  graphic.hud.topologyRenderer.fadeIn(0.5f, 0.5f);
  graphic.hud.thirdPersonCamera.fadeIn(0.6f, 0.5f);
  graphic.hud.coreUsageRenderer.fadeIn(0.7f, 0.5f);
  graphic.hud.fitnessDataRenderer.fadeIn(0.8f, 0.5f);
  graphic.hud.informationTextRenderer.fadeIn(0.9f, 0.5f);
}

void
State_Running::leave() {
  auto& graphic = Context::get().graphic;
  graphic.hud.topologyRenderer.fadeOut(0.0f, 0.5f);
  graphic.hud.thirdPersonCamera.fadeOut(0.1f, 0.5f);
  graphic.hud.coreUsageRenderer.fadeOut(0.2f, 0.5f);
  graphic.hud.fitnessDataRenderer.fadeOut(0.3f, 0.5f);
  graphic.hud.informationTextRenderer.fadeOut(0.3f, 0.5f);
}

void
State_Running::update(float elapsedTime) {
  State_AbstractSimulation::update(elapsedTime);

  auto& context = Context::get();
  auto& logic = context.logic;
  auto& simulation = *logic.simulation;
  auto& graphic = context.graphic;

  { // simulation update

    if (!simulation.isGenerationComplete()) {
      constexpr float fakeElapsedTime = 1.0f / 30.0f; // TODO: hardcoded
      const unsigned int totalSteps = (logic.isAccelerated ? 50 : 1);

      simulation.update(fakeElapsedTime, totalSteps);
    } else {
      simulation.breed();
    }

  } // simulation update

  // done to avoid a spurious change of camera
  // -> true when changing states: Running -> EndGeneration
  if (StateManager::get()->getState() == StateManager::States::Running) {

    graphic.scene.modelsRenderer.update(elapsedTime);

    _updateCameraTracking(elapsedTime);
    _updateCommonLogic(elapsedTime);
    graphic.scene.animatedCircuitRenderer.update(elapsedTime);
  }
}
