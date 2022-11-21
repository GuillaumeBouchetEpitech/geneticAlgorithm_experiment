
#include "Scene.hpp"

#include "common.hpp"

#include "demo/states/StateManager.hpp"

#include "framework/graphic/ShaderProgram.hpp"

#include "framework/math/clamp.hpp"

#include "framework/graphic/GlContext.hpp"

void Scene::initialise()
{
  GlContext::enable(GlContext::States::depthTest);
  GlContext::setDepthFunc(GlContext::DepthFuncs::less);

  GlContext::enable(GlContext::States::blend);
  GlContext::setBlendFunc(GlContext::BlendFuncs::srcAlpha, GlContext::BlendFuncs::oneMinuxSrcAlpha);

  GlContext::disable(GlContext::States::cullFace);
}

void Scene::renderSimple()
{
  Scene::_clear();

  auto& graphic = Data::get().graphic;

  { // scene

    const Camera& camInstance = graphic.camera.scene.instance;
    graphic.stackRenderer.setMatricesData(camInstance.getSceneMatricsData());
    graphic.particleManager.setMatricesData(camInstance.getSceneMatricsData());
    graphic.animatedCircuitRenderer.setMatricesData(camInstance.getSceneMatricsData());

    Scene::_renderFloor(camInstance);
    graphic.animatedCircuitRenderer.render();
  }

  { // HUD

    const Camera& camInstance = graphic.camera.scene.instance;
    graphic.stackRenderer.setMatricesData(camInstance.getHudMatricsData());
    graphic.particleManager.setMatricesData(camInstance.getHudMatricsData());
    graphic.textRenderer.setMatricesData(camInstance.getHudMatricsData());

    Scene::_renderHUD();
  }

  ShaderProgram::unbind();
}

void Scene::renderAll()
{
  Scene::_clear();

  auto& data = Data::get();
  auto& graphic = data.graphic;

  { // scene

    auto& logic = data.logic;
    auto& graphic = data.graphic;
    auto& camera = graphic.camera;
    const Camera& camInstance = camera.scene.instance;

    graphic.stackRenderer.setMatricesData(camInstance.getSceneMatricsData());
    graphic.particleManager.setMatricesData(camInstance.getSceneMatricsData());
    graphic.floorRenderer.setMatricesData(camInstance.getSceneMatricsData());
    graphic.backGroundCylindersRenderer.setMatricesData(camInstance.getSceneMatricsData());
    graphic.animatedCircuitRenderer.setMatricesData(camInstance.getSceneMatricsData());
    graphic.flockingManager.setMatricesData(camInstance.getSceneMatricsData());
    graphic.carTailsRenderer.setMatricesData(camInstance.getSceneMatricsData());

    Scene::_renderFloor(camInstance);
    graphic.animatedCircuitRenderer.render();

    if (!logic.isAccelerated)
      Scene::_renderLeadingCarSensors();

    graphic.flockingManager.render();
    graphic.stackRenderer.flush();

    graphic.particleManager.render();
    graphic.modelsRenderer.render(camInstance);
    graphic.carTailsRenderer.render();
  }

  { // HUD

    const Camera& camInstance = graphic.camera.scene.instance;
    graphic.stackRenderer.setMatricesData(camInstance.getHudMatricsData());
    graphic.particleManager.setMatricesData(camInstance.getHudMatricsData());
    graphic.floorRenderer.setMatricesData(camInstance.getHudMatricsData());
    graphic.backGroundCylindersRenderer.setMatricesData(camInstance.getHudMatricsData());
    graphic.textRenderer.setMatricesData(camInstance.getHudMatricsData());

    Scene::_renderHUD();
  }

  ShaderProgram::unbind();
}

void Scene::updateMatrices(float elapsedTime)
{
  auto& data = Data::get();
  const auto& logic = data.logic;
  const auto& simulation = *logic.simulation;
  auto& graphic = data.graphic;
  auto& camera = graphic.camera;

  { // scene

    // clamp vertical rotation [-70..+70]
    const float verticalLimit = glm::radians(70.0f);
    camera.scene.rotations.phi = glm::clamp(camera.scene.rotations.phi, -verticalLimit, verticalLimit);

    const float cosPhi = std::cos(camera.scene.rotations.phi);
    const glm::vec3 eye =
    {
      camera.scene.center.x + camera.scene.distance * cosPhi * std::cos(camera.scene.rotations.theta),
      camera.scene.center.y + camera.scene.distance * cosPhi * std::sin(camera.scene.rotations.theta),
      camera.scene.center.z + camera.scene.distance * std::sin(camera.scene.rotations.phi)
    };
    const glm::vec3 upAxis = { 0.0f, 0.0f, 1.0f };

    camera.scene.instance.setSize(camera.viewportSize.x, camera.viewportSize.y);
    camera.scene.instance.lookAt(eye, camera.scene.center, upAxis);
    camera.scene.instance.computeMatrices();

  } // scene

  { // third person

    if (logic.leaderCar.index >= 0)
    {
      const auto& carResult = simulation.getCarResult(logic.leaderCar.index);

      glm::vec3 carOrigin = carResult.liveTransforms.chassis * glm::vec4(0.0f, 0.0f, 2.5f, 1.0f);
      glm::vec3 carUpAxis = carResult.liveTransforms.chassis * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

      StateManager::States currentState = StateManager::get()->getState();

      if (
        // do not update the third person camera if not in a correct state
        (currentState == StateManager::States::Running ||
          currentState == StateManager::States::StartGeneration) &&
        // do not update the third person camera if too close from the target
        glm::distance(carOrigin, camera.thirdPerson.eye) > 0.25f
      ) {
        // simple lerp to setup the third person camera
        const float lerpRatio = 0.1f * 60.0f * elapsedTime;
        camera.thirdPerson.eye += (carOrigin - camera.thirdPerson.eye) * lerpRatio;
        camera.thirdPerson.upAxis += (carUpAxis - camera.thirdPerson.upAxis) * lerpRatio;
      }

      const glm::vec3 eye = camera.thirdPerson.eye;
      const glm::vec3 target = carOrigin;
      const glm::vec3 upAxis = camera.thirdPerson.upAxis;

      camera.thirdPerson.instance.setSize(scene::thirdPViewportWidth, scene::thirdPViewportHeight);
      camera.thirdPerson.instance.lookAt(eye, target, upAxis);
      camera.thirdPerson.instance.computeMatrices();
    }

  } // third person

  { // hud ortho

    // const auto& vSize = camera.viewportSize;

    // glm::mat4 projection = glm::ortho(0.0f, vSize.x, 0.0f, vSize.y, -1.0f, 1.0f);

    // glm::vec3 eye = { 0.0f, 0.0f, 0.5f };
    // glm::vec3 center = { 0.0f, 0.0f, 0.0f };
    // glm::vec3 upAxis = { 0.0f, 1.0f, 0.0f };
    // glm::mat4 viewMatrix = glm::lookAt(eye, center, upAxis);

    // matrices.hud_ortho = projection * viewMatrix;

  } // hud ortho

  { // hud_perspective

    // const auto& vSize = camera.viewportSize;

    {

      // const float fovy = glm::radians(70.0f);
      // const float aspectRatio = float(vSize.x) / vSize.y;
      // glm::mat4 projection = glm::perspective(fovy, aspectRatio, 1.0f, 1000.f);

      // const glm::vec2 halfViewportSize = vSize * 0.5f;
      // glm::vec3 eye = { halfViewportSize.x, halfViewportSize.y, 425.0f };
      // glm::vec3 center = { halfViewportSize.x, halfViewportSize.y, 0.0f };
      // glm::vec3 upAxis = { 0.0f, 1.0f, 0.0f };
      // glm::mat4 viewMatrix = glm::lookAt(eye, center, upAxis);

      // matrices.hud_perspective = projection * viewMatrix;

    }

    {

      // glm::mat4 projection = glm::ortho(0.0f, vSize.x, 0.0f, vSize.y, -1.0f, 1.0f);

      // glm::vec3 eye = { 0.0f, 0.0f, 0.5f };
      // glm::vec3 center = { 0.0f, 0.0f, 0.0f };
      // glm::vec3 upAxis = { 0.0f, 1.0f, 0.0f };
      // glm::mat4 viewMatrix = glm::lookAt(eye, center, upAxis);

      // matrices.hud_perspective = projection * viewMatrix;

    }

  } // hud_perspective
}

void Scene::_clear()
{
  const auto& viewportSize = Data::get().graphic.camera.viewportSize;

  GlContext::setViewport(0, 0, viewportSize.x, viewportSize.y);

  GlContext::clearColor(0.0f, 0.0f, 0.0f, 1.0f);
  GlContext::clear(asValue(GlContext::Buffers::color) | asValue(GlContext::Buffers::depth));
}
