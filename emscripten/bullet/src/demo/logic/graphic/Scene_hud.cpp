
#include "Scene.hpp"

#include "common.hpp"

#include "demo/states/StateManager.hpp"

#include "demo/logic/graphicIds.hpp"

#include "framework/asValue.hpp"

#include "framework/TraceLogger.hpp"
#include "framework/math/clamp.hpp"

#include "renderers/hud/renderTopology.hpp"
#include "renderers/hud/CoreUsageRenderer.hpp"
#include "renderers/hud/NewLeaderRenderer.hpp"
#include "renderers/scene/renderLeaderEye.hpp"

#include "./helpers/writeTime.hpp"

#include "framework/graphic/ResourceManager.hpp"
#include "framework/graphic/GlContext.hpp"

#include <iomanip>
#include <sstream>

void Scene::_renderHUD_ortho()
{
  auto& data = Data::get();
  auto& graphic = data.graphic;
  auto& logic = data.logic;

  // glDisable(GL_DEPTH_TEST); // <= not useful for a HUD

  NewLeaderRenderer newLeaderRndr;
  newLeaderRndr.compute();

  CoreUsageRenderer coreUsageRndr;
  coreUsageRndr.position = { 8, 4 * 16 + 7 };
  coreUsageRndr.size = { 150, 100 };

  { // texts

    auto&       textRenderer = graphic.textRenderer;
    const auto& simulation = *logic.simulation;
    const auto& hudText = logic.hudText;

    textRenderer.clear();

    const auto& vSize = graphic.camera.viewportSize;

    { // top-left header text

      textRenderer.push({ 8, vSize.y - 16 - 8 }, hudText.header, glm::vec3(1), 1.0f);

    } // top-left header text

    { // top-left performance stats

      std::stringstream sstr;

      sstr
        << "Update: " << writeTime(logic.metrics.updateTime) << std::endl
        << "Render: " << writeTime(logic.metrics.renderTime);

      const std::string str = sstr.str();

      textRenderer.push({ 8, vSize.y - 5 * 16 - 8 }, str,  glm::vec3(1), 1.0f);

    } // top-left performance stats

    { // bottom-left text

      const unsigned int totalCars = logic.cores.totalCars;
      unsigned int carsLeft = 0;
      float localBestFitness = 0.0f;

      for (unsigned int ii = 0; ii < totalCars; ++ii)
      {
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

        textRenderer.push({ 8, 8 + 2 * 16 }, str, glm::vec3(1), 1.0f);
      }

      {
        std::stringstream sstr;

        sstr
          << std::fixed << std::setprecision(1)
          << "Fitness: " << localBestFitness << "/" << simulation.getBestGenome().fitness;

        const std::string str = sstr.str();

        const float bestFitness = simulation.getBestGenome().fitness;
        const float coef = bestFitness == 0.0f
          ? 0.0f
          : clamp(localBestFitness / bestFitness, 0.0f, 1.0f);

        const glm::vec3 textColor = glm::mix(glm::vec3(1,0,0), glm::vec3(0,1,0), coef);

        textRenderer.push({ 8, 8 + 1 * 16 }, str, textColor, 1.0f);
      }

      {
        std::stringstream sstr;

        sstr
          << std::fixed << std::setprecision(1)
          << "Cars: " << carsLeft << "/" << totalCars;

        const std::string str = sstr.str();

        const float coef = 1.0f - clamp(float(carsLeft) / totalCars, 0.0f, 1.0f);

        const glm::vec3 textColor = glm::mix(glm::vec3(0,1,0), glm::vec3(1,0,0), coef);

        textRenderer.push({ 8, 8 + 0 * 16 }, str, textColor, 1.0f);
      }

    } // bottom-left text

    { // advertise a new leader

      newLeaderRndr.renderHudText();

    } // advertise a new leader

    { // show cores status

      coreUsageRndr.renderHudText();

    } // show cores status

    { // big titles

      StateManager::States currentState = StateManager::get()->getState();

#if defined D_WEB_WEBWORKER_BUILD
      if (currentState == StateManager::States::WorkersLoading)
      {
        float scale = 2.0f;

        std::stringstream sstr;
        sstr
          << "WEB WORKERS" << std::endl
          << "  LOADING  " << std::endl;
        std::string message = sstr.str();

        textRenderer.push({ vSize.x * 0.5f - 5 * 16 * scale, vSize.y * 0.5f - 8 * scale }, message, glm::vec3(1), scale);
      }
#endif

      if (currentState == StateManager::States::Paused)
      {
        float scale = 5.0f;

        std::string message = "PAUSED";

        textRenderer.push({ vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale, vSize.y * 0.5f - 8 * scale }, message, glm::vec3(1), scale);
      }

      if (currentState == StateManager::States::StartGeneration)
      {
        {
          const float scale = 3.0f;

          std::stringstream sstr;
          sstr << "Generation: " << simulation.getGenerationNumber();
          const std::string message = sstr.str();

          textRenderer.push({ vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale, vSize.y * 0.5f + 16 * scale }, message, glm::vec3(1), scale);
        }

        {
          const float scale = 2.0f;

          const std::size_t totalStats = logic.fitnessStats.allStats.size();
          const float prevFitness = logic.fitnessStats.allStats[totalStats - 2];
          const float currFitness = logic.fitnessStats.allStats[totalStats - 1];

          if (currFitness > 0.0f)
          {
            {
              std::stringstream sstr;
              sstr << "Fitness: " << std::fixed << std::setprecision(1) << currFitness;
              std::string message = sstr.str();

              textRenderer.push({ vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale, vSize.y * 0.5f + 0 * scale }, message, glm::vec3(1), scale);
            }

            if (currFitness != prevFitness)
            {
              std::stringstream sstr;

              if (currFitness > prevFitness)
                sstr << "Smarter result (+" << std::fixed << std::setprecision(1) << (currFitness - prevFitness) << ")";
              else if (currFitness < prevFitness)
                sstr << "Worse result (" << std::fixed << std::setprecision(1) << (currFitness - prevFitness) << ")";

              const std::string message = sstr.str();

              textRenderer.push({ vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale, vSize.y * 0.5f - 16 * scale }, message, glm::vec3(1), scale);
            }
          }
        }
      }

    } // big titles

    textRenderer.render();

  } // texts

  { // wireframes

    { // cores history graphics

      coreUsageRndr.renderWireframe();

    } // cores history graphics

    { // advertise a new leader

      newLeaderRndr.renderWireframe();

    } // advertise a new leader

    auto& stackRenderer = graphic.stackRenderer;

    { // progresses curve

      const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);

      const glm::vec2 borderPos(8, 160);
      const glm::vec2 borderSize(150, 75);

      stackRenderer.pushQuad(glm::vec3(borderPos + borderSize * 0.5f, -0.1f), borderSize, glm::vec4(0,0,0, 0.75f));
      stackRenderer.pushRectangle(borderPos, borderSize, whiteColor);

      const auto& allStats = logic.fitnessStats.allStats;

      float maxFitness = 0.0f;
      for (float stat : allStats)
        maxFitness = std::max(maxFitness, stat);

      float stepWidth = borderSize.x / (allStats.size() - 1);

      for (std::size_t ii = 1; ii < allStats.size(); ++ii)
      {
        const float prevData = allStats[ii - 1];
        const float currData = allStats[ii];

        const glm::vec2 prevPos =
        {
          stepWidth * (ii - 1),
          (prevData / maxFitness) * borderSize.y,
        };
        const glm::vec2 currPos =
        {
          stepWidth * ii,
          (currData / maxFitness) * borderSize.y,
        };

        stackRenderer.pushLine(borderPos + prevPos, borderPos + currPos, whiteColor);
      }

    } // progresses curve

    if (logic.leaderCar.index >= 0)
    {
      const auto& vSize = graphic.camera.viewportSize;

      { // topology

        renderTopology(
          glm::vec2(vSize.x - 150 - 10, 170),
          glm::vec2(150, 125));

      } // topology

      { // leader's eye

        renderLeaderEye(
          glm::vec2(vSize.x - 100 - 10, 305),
          glm::vec2(100, 60));

      } // leader's eye
    }

    stackRenderer.flush();

  } // wireframes

  // glEnable(GL_DEPTH_TEST);
}

void Scene::_renderHUD_thirdPerson()
{
  auto& data = Data::get();
  auto& logic = data.logic;
  auto& graphic = data.graphic;
  const auto& leaderCar = logic.leaderCar;

  // valid leading car?
  if (logic.isAccelerated || leaderCar.index < 0)
    return;

  auto& camera = graphic.camera;

  const auto& viewportSize = camera.viewportSize;
  const glm::vec2 thirdPViewportPos = { viewportSize.x - scene::thirdPViewportWidth - 10, 10 };

  GlContext::enable(GlContext::States::scissorTest);
  GlContext::setScissor(thirdPViewportPos.x, thirdPViewportPos.y, scene::thirdPViewportWidth, scene::thirdPViewportHeight);
  GlContext::setViewport(thirdPViewportPos.x, thirdPViewportPos.y, scene::thirdPViewportWidth, scene::thirdPViewportHeight);
  GlContext::clearColor(0, 0, 0, 1);
  GlContext::clear(asValue(GlContext::Buffers::color) | asValue(GlContext::Buffers::depth));

  const Camera& camInstance = camera.thirdPerson.instance;

  graphic.stackRenderer.setMatricesData(camInstance.getSceneMatricsData());
  graphic.particleManager.setMatricesData(camInstance.getSceneMatricsData());
  graphic.floorRenderer.setMatricesData(camInstance.getSceneMatricsData());
  graphic.backGroundCylindersRenderer.setMatricesData(camInstance.getSceneMatricsData());
  graphic.animatedCircuitRenderer.setMatricesData(camInstance.getSceneMatricsData());
  graphic.flockingManager.setMatricesData(camInstance.getSceneMatricsData());
  graphic.carTailsRenderer.setMatricesData(camInstance.getSceneMatricsData());

  Scene::_renderFloor(camInstance);

  Scene::_renderLeadingCarSensors();
  graphic.flockingManager.render();

  graphic.stackRenderer.flush();

  graphic.particleManager.render();

  graphic.modelsRenderer.render(camInstance);
  graphic.carTailsRenderer.render();
  graphic.animatedCircuitRenderer.render();

  GlContext::disable(GlContext::States::scissorTest);
  GlContext::setViewport(0, 0, viewportSize.x, viewportSize.y);
}

void Scene::_renderHUD()
{
  auto& data = Data::get();
  auto& graphic = data.graphic;

  { // render in framebuffer

    graphic.hudComponents.frameBuffer.bind();

    GlContext::clearColor(0, 0, 0, 0);
    GlContext::clear(asValue(GlContext::Buffers::color) | asValue(GlContext::Buffers::depth));

    Scene::_renderHUD_ortho();
    Scene::_renderHUD_thirdPerson();

    FrameBuffer::unbind();

  } // render in framebuffer

  { // render framebuffer texture in curved geometry

    auto shader = ResourceManager::get().getShader(asValue(Shaders::simpleTexture));

    shader->bind();
    shader->setUniform("u_composedMatrix", graphic.camera.scene.instance.getHudMatricsData().composed);

    GlContext::disable(GlContext::States::depthTest);

    graphic.hudComponents.colorTexture.bind();

    graphic.geometries.hudPerspective.geometry.render();

    GlContext::enable(GlContext::States::depthTest);

  } // render framebuffer texture in curved geometry
}
