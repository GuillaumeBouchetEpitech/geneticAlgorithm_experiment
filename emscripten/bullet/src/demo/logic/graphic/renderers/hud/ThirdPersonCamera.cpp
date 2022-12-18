
#include "ThirdPersonCamera.hpp"

#include "demo/logic/Context.hpp"
#include "demo/logic/graphic/Scene.hpp"

#include "framework/asValue.hpp"
#include "framework/graphic/GlContext.hpp"

#include <iomanip>
#include <sstream>

namespace {

constexpr float k_faceInX = -10.0f;
constexpr float k_faceOutX = +500.0f;

} // namespace

void ThirdPersonCamera::initialise() {
  auto& context = Context::get();
  const auto& vSize = context.graphic.camera.viewportSize;

  _size = {175, 150};

  _position.x = vSize.x - _size.x + k_faceInX;
  _position.y = 10;
}

void ThirdPersonCamera::fadeIn() {
  auto& context = Context::get();
  auto& graphic = context.graphic;

  _animRef = graphic.hud.animationManager.push(
    _animRef, 0.0f, 2.0f, [this, &graphic](float coef) {
      const auto& vSize = graphic.camera.viewportSize;
      const float targetPos = vSize.x - _size.x + k_faceInX;
      _position.x = _position.x + (targetPos - _position.x) * coef;
    });
}

void ThirdPersonCamera::fadeOut() {
  auto& context = Context::get();
  auto& graphic = context.graphic;

  _animRef = graphic.hud.animationManager.push(
    _animRef, 0.0f, 5.0f, [this, &graphic](float coef) {
      const auto& vSize = graphic.camera.viewportSize;
      const float targetPos = vSize.x - _size.x + k_faceOutX;
      _position.x = _position.x + (targetPos - _position.x) * coef;
    });
}

void ThirdPersonCamera::render() {
  auto& context = Context::get();
  auto& logic = context.logic;
  auto& graphic = context.graphic;
  const auto& leaderCar = logic.leaderCar;

  // valid leading car?
  if (logic.isAccelerated || !leaderCar.hasLeader())
    return;

  auto& camera = graphic.camera;

  const auto& vSize = camera.viewportSize;
  // const glm::vec2 thirdPViewportPos = {
  //   viewportSize.x - _size.x - 10, 10};

  GlContext::enable(GlContext::States::scissorTest);
  GlContext::setScissor(_position.x, _position.y, uint32_t(_size.x),
                        uint32_t(_size.y));
  GlContext::setViewport(_position.x, _position.y, uint32_t(_size.x),
                         uint32_t(_size.y));
  GlContext::clearColor(0, 0, 0, 1);
  GlContext::clear(asValue(GlContext::Buffers::color) |
                   asValue(GlContext::Buffers::depth));

  const Camera& camInstance = camera.thirdPerson.scene;

  const auto& matriceData = camInstance.getMatricesData();

  graphic.hud.stackRenderers.wireframes.setMatricesData(matriceData);
  graphic.hud.stackRenderers.triangles.setMatricesData(matriceData);
  graphic.scene.particleManager.setMatricesData(matriceData);
  graphic.scene.floorRenderer.setMatricesData(matriceData);
  graphic.scene.animatedCircuitRenderer.setMatricesData(matriceData);
  graphic.scene.flockingManager.setMatricesData(matriceData);
  graphic.scene.carTailsRenderer.setMatricesData(matriceData);

  Scene::_renderFloor(camInstance);
  graphic.scene.animatedCircuitRenderer.renderWireframe();
  graphic.scene.animatedCircuitRenderer.renderWalls();

  Scene::_renderLeadingCarSensors();
  graphic.scene.flockingManager.render();

  graphic.hud.stackRenderers.wireframes.flush();
  graphic.hud.stackRenderers.triangles.flush();

  graphic.scene.particleManager.render();

  graphic.scene.modelsRenderer.render(camInstance);
  graphic.scene.animatedCircuitRenderer.renderGround();
  graphic.scene.carTailsRenderer.render();

  GlContext::disable(GlContext::States::scissorTest);
  GlContext::setViewport(0, 0, vSize.x, vSize.y);
}
