
#include "Scene.hpp"

#include "common.hpp"

#include "demo/states/StateManager.hpp"

#include "demo/logic/graphicIds.hpp"

#include "framework/asValue.hpp"

#include "framework/TraceLogger.hpp"
#include "framework/math/clamp.hpp"

#include "renderers/hud/CoreUsageRenderer.hpp"
#include "renderers/hud/NewLeaderRenderer.hpp"
#include "renderers/scene/renderLeaderEye.hpp"

#include "./helpers/writeTime.hpp"

#include "framework/graphic/GlContext.hpp"
#include "framework/graphic/ResourceManager.hpp"

#include <iomanip>
#include <sstream>

void Scene::_renderHUD_ortho() {
  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& logic = context.logic;

  NewLeaderRenderer newLeaderRndr;
  newLeaderRndr.compute();

  CoreUsageRenderer coreUsageRndr;
  coreUsageRndr.position = {8, 4 * 16 + 7};
  coreUsageRndr.size = {150, 100};

  { // texts

    auto& textRenderer = graphic.textRenderer;
    const auto& simulation = *logic.simulation;
    const auto& hudText = logic.hudText;

    textRenderer.clear();

    const auto& vSize = graphic.camera.viewportSize;

    { // top-left header text

      textRenderer.push({8, vSize.y - 16 - 8}, hudText.header, glm::vec3(1),
                        1.0f);

    } // top-left header text

    { // top-left performance stats

      std::stringstream sstr;

      sstr << "Update: " << writeTime(logic.metrics.updateTime) << std::endl
           << "Render: " << writeTime(logic.metrics.renderTime);

      const std::string str = sstr.str();

      textRenderer.push({8, vSize.y - 5 * 16 - 8}, str, glm::vec3(1), 1.0f);

    } // top-left performance stats

    { // bottom-left text

      const unsigned int totalCars = logic.cores.totalCars;
      unsigned int carsLeft = 0;
      float localBestFitness = 0.0f;

      for (unsigned int ii = 0; ii < totalCars; ++ii) {
        const auto& carData = simulation.getCarResult(ii);

        if (carData.isAlive)
          ++carsLeft;

        if (localBestFitness < carData.fitness)
          localBestFitness = carData.fitness;
      }

      {
        std::stringstream sstr;

        sstr << "Generation: " << simulation.getGenerationNumber();

        const std::string str = sstr.str();

        textRenderer.push({8, 8 + 2 * 16}, str, glm::vec3(1), 1.0f);
      }

      {
        std::stringstream sstr;

        sstr << std::fixed << std::setprecision(1)
             << "Fitness: " << localBestFitness << "/"
             << simulation.getBestGenome().fitness;

        const std::string str = sstr.str();

        const float bestFitness = simulation.getBestGenome().fitness;
        const float coef =
          bestFitness == 0.0f
            ? 0.0f
            : clamp(localBestFitness / bestFitness, 0.0f, 1.0f);

        const glm::vec3 textColor =
          glm::mix(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), coef);

        textRenderer.push({8, 8 + 1 * 16}, str, textColor, 1.0f);
      }

      {
        std::stringstream sstr;

        sstr << std::fixed << std::setprecision(1) << "Cars: " << carsLeft
             << "/" << totalCars;

        const std::string str = sstr.str();

        const float coef =
          1.0f - clamp(float(carsLeft) / totalCars, 0.0f, 1.0f);

        const glm::vec3 textColor =
          glm::mix(glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), coef);

        textRenderer.push({8, 8 + 0 * 16}, str, textColor, 1.0f);
      }

    } // bottom-left text

    newLeaderRndr.renderHudText();
    coreUsageRndr.renderHudText();

    { // big titles

      StateManager::States currentState = StateManager::get()->getState();

#if defined D_WEB_WEBWORKER_BUILD
      if (currentState == StateManager::States::WorkersLoading) {
        float scale = 2.0f;

        std::stringstream sstr;
        sstr << "WEB WORKERS" << std::endl << "  LOADING  " << std::endl;
        std::string message = sstr.str();

        textRenderer.push(
          {vSize.x * 0.5f - 5 * 16 * scale, vSize.y * 0.5f - 8 * scale},
          message, glm::vec3(1), scale);
      }
#endif

      if (currentState == StateManager::States::Paused) {
        float scale = 5.0f;

        std::string message = "PAUSED";

        textRenderer.push(
          {vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale,
           vSize.y * 0.5f - 8 * scale},
          message, glm::vec3(1), scale);
      }

      if (currentState == StateManager::States::StartGeneration) {
        {
          const float scale = 3.0f;

          std::stringstream sstr;
          sstr << "Generation: " << simulation.getGenerationNumber();
          const std::string message = sstr.str();

          textRenderer.push(
            {vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale,
             vSize.y * 0.5f + 16 * scale},
            message, glm::vec3(1), scale);
        }

        {
          const float scale = 2.0f;

          const float prevFitness = logic.fitnessStats.get(-2);
          const float currFitness = logic.fitnessStats.get(-1);

          if (currFitness > 0.0f) {
            {
              std::stringstream sstr;
              sstr << "Fitness: " << std::fixed << std::setprecision(1)
                   << currFitness;
              std::string message = sstr.str();

              textRenderer.push(
                {vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale,
                 vSize.y * 0.5f + 0 * scale},
                message, glm::vec3(1), scale);
            }

            if (currFitness != prevFitness) {
              std::stringstream sstr;

              if (currFitness > prevFitness)
                sstr << "Smarter result (+" << std::fixed
                     << std::setprecision(1) << (currFitness - prevFitness)
                     << ")";
              else if (currFitness < prevFitness)
                sstr << "Worse result (" << std::fixed << std::setprecision(1)
                     << (currFitness - prevFitness) << ")";

              const std::string message = sstr.str();

              textRenderer.push(
                {vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale,
                 vSize.y * 0.5f - 16 * scale},
                message, glm::vec3(1), scale);
            }
          }
        }
      }

    } // big titles

    textRenderer.render();

  } // texts

  { // wireframes

    coreUsageRndr.renderWireframe();
    newLeaderRndr.renderWireframe();

    auto& stackRenderer = graphic.stackRenderers;

    { // progresses curve

      const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);

      const glm::vec2 borderPos(8, 160);
      const glm::vec2 borderSize(150, 75);

      stackRenderer.triangles.pushQuad(
        glm::vec3(borderPos + borderSize * 0.5f, -0.1f), borderSize,
        glm::vec4(0, 0, 0, 0.75f));
      stackRenderer.wireframes.pushRectangle(borderPos, borderSize, whiteColor);

      const float maxFitness = logic.fitnessStats.max();
      float stepWidth = borderSize.x / (logic.fitnessStats.size() - 1);

      for (std::size_t ii = 1; ii < logic.fitnessStats.size(); ++ii) {
        const float prevData = logic.fitnessStats.get(ii - 1);
        const float currData = logic.fitnessStats.get(ii);

        const glm::vec2 prevPos = {
          stepWidth * (ii - 1),
          (prevData / maxFitness) * borderSize.y,
        };
        const glm::vec2 currPos = {
          stepWidth * ii,
          (currData / maxFitness) * borderSize.y,
        };

        stackRenderer.wireframes.pushLine(borderPos + prevPos,
                                          borderPos + currPos, whiteColor);
      }

    } // progresses curve

    if (logic.leaderCar.hasLeader()) {
      const auto& vSize = graphic.camera.viewportSize;

      graphic.topologyRenderer.render(glm::vec2(vSize.x - 150 - 10, 170),
                                      glm::vec2(150, 125));

      renderLeaderEye(glm::vec2(vSize.x - 100 - 10, 305), glm::vec2(100, 60));
    }

    stackRenderer.wireframes.flush();
    stackRenderer.triangles.flush();

  } // wireframes
}

void Scene::_renderHUD_thirdPerson() {
  auto& context = Context::get();
  auto& logic = context.logic;
  auto& graphic = context.graphic;
  const auto& leaderCar = logic.leaderCar;

  // valid leading car?
  if (logic.isAccelerated || !leaderCar.hasLeader())
    return;

  auto& camera = graphic.camera;

  const auto& viewportSize = camera.viewportSize;
  const glm::vec2 thirdPViewportPos = {
    viewportSize.x - scene::thirdPViewportWidth - 10, 10};

  GlContext::enable(GlContext::States::scissorTest);
  GlContext::setScissor(thirdPViewportPos.x, thirdPViewportPos.y,
                        scene::thirdPViewportWidth,
                        scene::thirdPViewportHeight);
  GlContext::setViewport(thirdPViewportPos.x, thirdPViewportPos.y,
                         scene::thirdPViewportWidth,
                         scene::thirdPViewportHeight);
  GlContext::clearColor(0, 0, 0, 1);
  GlContext::clear(asValue(GlContext::Buffers::color) |
                   asValue(GlContext::Buffers::depth));

  const Camera& camInstance = camera.thirdPerson.scene;

  const auto& matriceData = camInstance.getMatricesData();

  graphic.stackRenderers.wireframes.setMatricesData(matriceData);
  graphic.stackRenderers.triangles.setMatricesData(matriceData);
  graphic.particleManager.setMatricesData(matriceData);
  graphic.floorRenderer.setMatricesData(matriceData);
  graphic.backGroundCylindersRenderer.setMatricesData(matriceData);
  graphic.animatedCircuitRenderer.setMatricesData(matriceData);
  graphic.flockingManager.setMatricesData(matriceData);
  graphic.carTailsRenderer.setMatricesData(matriceData);

  Scene::_renderFloor(camInstance);
  graphic.animatedCircuitRenderer.renderWireframe();
  graphic.animatedCircuitRenderer.renderWalls();

  Scene::_renderLeadingCarSensors();
  graphic.flockingManager.render();

  graphic.stackRenderers.wireframes.flush();
  graphic.stackRenderers.triangles.flush();

  graphic.particleManager.render();

  graphic.modelsRenderer.render(camInstance);
  graphic.animatedCircuitRenderer.renderGround();
  graphic.carTailsRenderer.render();

  GlContext::disable(GlContext::States::scissorTest);
  GlContext::setViewport(0, 0, viewportSize.x, viewportSize.y);
}

void Scene::_renderHUD() {
  auto& context = Context::get();
  auto& graphic = context.graphic;

  { // render in framebuffer

    graphic.postProcess.bind();

    GlContext::clearColor(0, 0, 0, 0);
    GlContext::clear(asValue(GlContext::Buffers::color) |
                     asValue(GlContext::Buffers::depth));

    Scene::_renderHUD_ortho();
    Scene::_renderHUD_thirdPerson();

    graphic.postProcess.unbind();

  } // render in framebuffer

  { // render framebuffer texture in curved geometry

    graphic.postProcess.render();

  } // render framebuffer texture in curved geometry
}
