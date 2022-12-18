
#include "demo/defines.hpp"

#include "StateManager.hpp"
#include "State_Running.hpp"

#include "demo/logic/Context.hpp"
#include "demo/logic/graphic/Scene.hpp"

#include "framework/helpers/GLMath.hpp"

#include <cmath>  // std::ceil
#include <limits> // std::numeric_limits<T>::max();

void State_Running::update(int deltaTime) {
  State_AbstractSimulation::update(deltaTime);

  float elapsedTime = float(deltaTime) / 1000.0f;

  auto& context = Context::get();
  auto& logic = context.logic;
  auto& simulation = *logic.simulation;
  auto& graphic = context.graphic;
  auto& camera = graphic.camera;

  { // simulation update

    if (!simulation.isGenerationComplete()) {
      constexpr float fakeElapsedTime = 1.0f / 30.0f; // TODO: hardcoded
      const unsigned int totalSteps = (logic.isAccelerated ? 50 : 1);

      simulation.update(fakeElapsedTime, totalSteps);
    } else {
      simulation.breed();
    }

  } // simulation update

  Scene::updateMatrices(elapsedTime);

  // done to avoid a spurious change of camera
  // -> true when changing states: Running -> EndGeneration
  if (StateManager::get()->getState() == StateManager::States::Running) {

    { // camera tracking

      glm::vec3 cameraNextCenter = logic.circuitDimension.center;
      float cameraNextDistance = 200.0f;

      //
      //

      auto& leaderCar = logic.leaderCar;

      if (logic.isAccelerated) {
        leaderCar.reset();
      } else {
        cameraNextDistance = 40.0f;

        leaderCar.update(elapsedTime);

        if (auto leaderPos = leaderCar.leaderPosition())
          cameraNextCenter = *leaderPos;
      }

      //
      //

      {
        const float lerpRatio = 6.0f * elapsedTime;

        camera.main.center +=
          (cameraNextCenter - camera.main.center) * lerpRatio;
        camera.main.distance +=
          (cameraNextDistance - camera.main.distance) * lerpRatio;
      }

    } // camera tracking

    {
      graphic.scene.particleManager.update(elapsedTime);
      graphic.scene.backGroundTorusRenderer.update(elapsedTime);
      graphic.scene.animatedCircuitRenderer.update(elapsedTime);
      graphic.scene.flockingManager.update();
      graphic.hud.topologyRenderer.update(elapsedTime);
      graphic.hud.animationManager.update(elapsedTime);
    }
  }
}
