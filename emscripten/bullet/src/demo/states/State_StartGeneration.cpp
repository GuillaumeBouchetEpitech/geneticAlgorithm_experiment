
#include "State_StartGeneration.hpp"

#include "StateManager.hpp"

#include "demo/logic/Context.hpp"
#include "demo/logic/graphic/Scene.hpp"
#include "demo/logic/helpers/AnimationTransition.hpp"

#include "framework/helpers/GLMath.hpp"
// #include "framework/math/easingFunctions.hpp"

#include <array>
#include <iomanip>

void State_StartGeneration::enter() {
  auto& context = Context::get();

  const float lastFitness = context.logic.fitnessStats.get(-1);
  if (lastFitness > 0.0f) {
    _countdownUntilNextState = 2500; // wait 2.5 second(s)
  } else {
    _countdownUntilNextState = 1500; // wait 1.5 second(s)
  }

  context.graphic.hud.animationManager.push(
    0.0f, float(_countdownUntilNextState + 500) / 1000.0f,
    [&context](float coef) {
      static_cast<void>(coef);

      auto& graphic = context.graphic;
      auto& vSize = graphic.camera.viewportSize;

      {
        const float valueAlpha = AnimationTransition()
                                   .push(0.0f, 0.0f)
                                   .push(0.1f, 1.0f)
                                   .push(0.9f, 1.0f)
                                   .push(1.0f, 0.0f)
                                   .get(coef);

        graphic.hud.stackRenderers.triangles.pushQuad(
          glm::vec3(vSize * 0.5f, -1.0f), vSize,
          glm::vec4(0, 0, 0, valueAlpha * 0.75f));
      }
    });

  context.graphic.hud.animationManager.push(
    0.25f, float(_countdownUntilNextState + 250) / 1000.0f,
    [&context](float coef) {
      static_cast<void>(coef);

      auto& graphic = context.graphic;
      auto& textRenderer = graphic.hud.textRenderer;
      auto& vSize = graphic.camera.viewportSize;

      {
        const float valueAlpha = AnimationTransition()
                                   .push(0.0f, 0.0f)
                                   .push(0.2f, 1.0f)
                                   .push(0.9f, 1.0f)
                                   .push(1.0f, 0.0f)
                                   .get(coef);

        std::stringstream sstr;
        sstr << "Generation: "
             << context.logic.simulation->getGenerationNumber();
        const std::string message = sstr.str();

        constexpr float scale = 3.0f;

        glm::vec2 textPos;
        textPos.x = vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale;
        textPos.y = vSize.y * 0.5f + 16 * scale;

        textRenderer.push(textPos, message, glm::vec4(1, 1, 1, valueAlpha),
                          scale);
      }

      {
        const float scale = 2.0f;

        const float prevFitness = context.logic.fitnessStats.get(-2);
        const float currFitness = context.logic.fitnessStats.get(-1);

        if (currFitness > 0.0f) {
          {
            const float valueAlpha = AnimationTransition()
                                       .push(0.0f, 0.0f)
                                       .push(0.3f, 1.0f)
                                       .push(0.9f, 1.0f)
                                       .push(1.0f, 0.0f)
                                       .get(coef);

            std::stringstream sstr;
            sstr << "Fitness: " << std::fixed << std::setprecision(1)
                 << currFitness;
            std::string message = sstr.str();

            glm::vec2 textPos;
            textPos.x = vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale;
            textPos.y = vSize.y * 0.5f + 0 * scale;

            textRenderer.push(textPos, message, glm::vec4(1, 1, 1, valueAlpha),
                              scale);
          }

          if (currFitness != prevFitness) {

            const float valueAlpha = AnimationTransition()
                                       .push(0.0f, 0.0f)
                                       .push(0.4f, 1.0f)
                                       .push(0.9f, 1.0f)
                                       .push(1.0f, 0.0f)
                                       .get(coef);

            std::stringstream sstr;

            if (currFitness > prevFitness)
              sstr << "Smarter result (+" << std::fixed << std::setprecision(1)
                   << (currFitness - prevFitness) << ")";
            else if (currFitness < prevFitness)
              sstr << "Worse result (" << std::fixed << std::setprecision(1)
                   << (currFitness - prevFitness) << ")";

            const std::string message = sstr.str();

            glm::vec2 textPos;
            textPos.x = vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale;
            textPos.y = vSize.y * 0.5f - 16 * scale;

            textRenderer.push(textPos, message, glm::vec4(1, 1, 1, valueAlpha),
                              scale);
          }
        }
      }
    });
}

void State_StartGeneration::update(int deltaTime) {
  State_AbstractSimulation::update(deltaTime);

  auto& context = Context::get();
  auto& logic = context.logic;
  auto& graphic = context.graphic;

  float elapsedTime = float(deltaTime) / 1000.0f;

  Scene::updateMatrices(elapsedTime);

  {
    graphic.scene.particleManager.update(elapsedTime);
    graphic.scene.backGroundTorusRenderer.update(elapsedTime);
    graphic.scene.animatedCircuitRenderer.update(elapsedTime);
    graphic.scene.flockingManager.update();
    graphic.hud.animationManager.update(elapsedTime);
  }

  { // camera tracking

    auto& simulation = *logic.simulation;
    auto& camera = graphic.camera;

    glm::vec3 cameraNextCenter = camera.main.center;
    float cameraNextDistance = 200.0f;

    if (logic.isAccelerated) {
      cameraNextCenter = logic.circuitDimension.center;
    } else {
      cameraNextDistance = 40.0f;
      cameraNextCenter = simulation.getStartPosition();
    }

    //
    //

    const float lerpRatio = 0.1f;

    camera.main.center += (cameraNextCenter - camera.main.center) * lerpRatio;
    camera.main.distance +=
      (cameraNextDistance - camera.main.distance) * lerpRatio;

  } // camera tracking

  _countdownUntilNextState -= deltaTime;
  if (_countdownUntilNextState <= 0) {
    StateManager::get()->changeState(StateManager::States::Running);
  }
}
