
#include "Scene.hpp"

#include "demo/states/StateManager.hpp"

#include "framework/graphic/GlContext.hpp"
#include "framework/graphic/ShaderProgram.hpp"
#include "framework/system/math/clamp.hpp"

void Scene::initialise() {
  GlContext::enable(GlContext::States::depthTest);
  GlContext::setDepthFunc(GlContext::DepthFuncs::less);

  GlContext::enable(GlContext::States::blend);
  GlContext::setBlendFunc(GlContext::BlendFuncs::srcAlpha,
                          GlContext::BlendFuncs::oneMinuxSrcAlpha);

  GlContext::disable(GlContext::States::cullFace);
}

void Scene::renderSimple() {
  Scene::_clear();
  Scene::updateMatrices();

  auto& graphic = Context::get().graphic;

  graphic.hud.postProcess.startRecording();

  { // scene

    const auto& vSize = graphic.camera.viewportSize;

    GlContext::setViewport(0, 0, vSize.x, vSize.y);

    GlContext::clearColor(0.0f, 0.0f, 0.0f, 1.0f);
    GlContext::clear(asValue(GlContext::Buffers::color) |
                     asValue(GlContext::Buffers::depth));
  }

  graphic.hud.postProcess.stopRecording();

  { // HUD

    const auto& matricesData = graphic.camera.hud.getMatricesData();
    graphic.hud.postProcess.setMatricesData(matricesData);
    graphic.hud.stackRenderers.wireframes.setMatricesData(matricesData);
    graphic.hud.stackRenderers.triangles.setMatricesData(matricesData);
    graphic.hud.textRenderer.setMatricesData(matricesData);

    Scene::_renderHUD();
  }

  ShaderProgram::unbind();
}

void Scene::renderAll() {
  Scene::_clear();
  Scene::updateMatrices();

  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& camera = graphic.camera;

  { // scene

    const Camera& camInstance = camera.scene;
    const auto& vSize = camera.viewportSize;

    graphic.hud.postProcess.startRecording();

    Scene::renderScene(camInstance);

    graphic.hud.postProcess.stopRecording();

    GlContext::setViewport(0, 0, vSize.x, vSize.y);
  }

  { // HUD

    const auto& matricesData = camera.hud.getMatricesData();

    graphic.hud.postProcess.setMatricesData(matricesData);
    graphic.hud.stackRenderers.wireframes.setMatricesData(matricesData);
    graphic.hud.stackRenderers.triangles.setMatricesData(matricesData);
    graphic.hud.textRenderer.setMatricesData(matricesData);

    Scene::_renderHUD();
  }

  ShaderProgram::unbind();
}

void Scene::updateMatrices() {
  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& camera = graphic.camera;

  { // scene

    // clamp vertical rotation [-70..+70]
    const float verticalLimit = glm::radians(70.0f);
    auto& rotations = camera.rotations;
    rotations.phi = glm::clamp(rotations.phi, -verticalLimit, verticalLimit);

    const float cosPhi = std::cos(rotations.phi);
    const glm::vec3 cameraDir = {
      cosPhi * std::cos(rotations.theta),
      cosPhi * std::sin(rotations.theta),
      std::sin(rotations.phi)
    };
    const glm::vec3 eye = camera.center + cameraDir * camera.distance;
    const glm::vec3 upAxis = {0.0f, 0.0f, 1.0f};

    camera.scene.setSize(camera.viewportSize.x, camera.viewportSize.y);
    camera.scene.lookAt(eye, camera.center, upAxis);
    camera.scene.computeMatrices();

    camera.hud.setOrthographic(0.0f, float(camera.viewportSize.x), 0.0f,
                               float(camera.viewportSize.y), -10.0f, +10.0f);

    camera.hud.lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0),
                      glm::vec3(0, 1, 0));
    camera.hud.computeMatrices();

  } // scene
}

void Scene::_clear() {
  const auto& viewportSize = Context::get().graphic.camera.viewportSize;

  GlContext::setViewport(0, 0, viewportSize.x, viewportSize.y);

  GlContext::clearColor(0.0f, 0.0f, 0.0f, 1.0f);
  GlContext::clear(asValue(GlContext::Buffers::color) |
                   asValue(GlContext::Buffers::depth));
}
