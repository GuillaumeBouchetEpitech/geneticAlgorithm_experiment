
#include "State_EndGeneration.hpp"

#include "StateManager.hpp"

#include "demo/logic/Context.hpp"
#include "demo/logic/graphic/Scene.hpp"
#include "demo/logic/helpers/AnimationTransition.hpp"

void State_EndGeneration::enter() {
  _countdownUntilNextState = 750; // wait 0.75 seconds

  // auto& context = Context::get();

  // context.graphic.hud.animationManager.push(0.0f, 0.5f, [&context](float
  // coef)
  // {
  //   static_cast<void>(coef);

  //   auto& graphic = context.graphic;
  //   auto& vSize = graphic.camera.viewportSize;

  //   {
  //     const float valueAlpha = AnimationTransition()
  //       .push(0.0f, 0.0f)
  //       .push(0.1f, 1.0f)
  //       .push(0.9f, 1.0f)
  //       .push(1.0f, 0.0f).get(coef);

  //     graphic.hud.stackRenderers.triangles.pushQuad(
  //       glm::vec3(vSize * 0.5f, -1.0f),
  //       vSize, glm::vec4(0, 0, 0, valueAlpha * 0.75f));
  //   }
  // });
}

void State_EndGeneration::update(int deltaTime) {
  State_AbstractSimulation::update(deltaTime);

  auto& graphic = Context::get().graphic;

  float elapsedTime = float(deltaTime) / 1000.0f;

  Scene::updateMatrices(elapsedTime);

  {
    graphic.scene.particleManager.update(elapsedTime);
    graphic.scene.backGroundTorusRenderer.update(elapsedTime);
    graphic.scene.flockingManager.update();
    graphic.hud.animationManager.update(elapsedTime);
  }

  _countdownUntilNextState -= deltaTime;
  if (_countdownUntilNextState <= 0)
    StateManager::get()->changeState(StateManager::States::StartGeneration);
}
