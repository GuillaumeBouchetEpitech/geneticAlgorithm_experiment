
#include "ModelsRenderer.hpp"

#include "demo/logic/Context.hpp"
#include "demo/logic/graphicIds.hpp"

#include "framework/graphic/loaders/loadObjModel.hpp"
#include "framework/system/asValue.hpp"
#include "framework/system/math/GenericEasing.hpp"
#include "framework/system/math/RandomNumberGenerator.hpp"
#include "framework/system/math/easingFunctions.hpp"

namespace {

void
updateVerticesNormals(loader::ModelVertices& vertices) {
  for (std::size_t index = 0; index < vertices.size(); index += 3) {
    loader::ModelVertex& vertexA = vertices.at(index + 0);
    loader::ModelVertex& vertexB = vertices.at(index + 1);
    loader::ModelVertex& vertexC = vertices.at(index + 2);

    const glm::vec3 normal = glm::cross(
      vertexA.position - vertexB.position, vertexA.position - vertexC.position);

    vertexA.normal = normal;
    vertexB.normal = normal;
    vertexC.normal = normal;
  }
}

} // namespace

void
ModelsRenderer::initialise() {

  auto& resourceManager = Context::get().graphic.resourceManager;

  { // chassis geometry (instanced)

    loader::ModelVertices modelVertices;
    loader::loadObjModel(
      "./assets/model/CarNoWheels.obj", "./assets/model/", modelVertices);

    updateVerticesNormals(modelVertices);

    _chassis.shader = resourceManager.getShader(asValue(ShaderIds::modelsLit));

    auto geoDef =
      resourceManager.getGeometryDefinition(asValue(GeometryIds::modelsLit));
    _chassis.geometry.initialise(*_chassis.shader, geoDef);
    _chassis.geometry.updateBuffer(0, modelVertices);
    _chassis.geometry.setPrimitiveCount(modelVertices.size());
  }

  { // wheel geometry (instanced)

    loader::ModelVertices modelVertices;
    loader::loadObjModel(
      "./assets/model/CarWheel.obj", "./assets/model/", modelVertices);

    updateVerticesNormals(modelVertices); // TODO: useful?

    _wheels.shader = resourceManager.getShader(asValue(ShaderIds::models));

    auto geoDef =
      resourceManager.getGeometryDefinition(asValue(GeometryIds::models));
    _wheels.geometry.initialise(*_wheels.shader, geoDef);
    _wheels.geometry.updateBuffer(0, modelVertices);
    _wheels.geometry.setPrimitiveCount(modelVertices.size());
  }
}

void
ModelsRenderer::fadeIn(float delay, float duration) {
  _timer.start(delay, duration);
}

void
ModelsRenderer::update(float elapsedTime) {
  if (_timer.isDone())
    return;

  _timer.update(elapsedTime);

  _colorAlpha = GenericEasing<3>()
                  .push(0.0f, 0.0f)
                  .push(0.5f, 0.0f, easing::easeOutCubic)
                  .push(1.0f, 1.0f)
                  .get(_timer.getCoefElapsed());

  _outlineColoralpha = GenericEasing<2>()
                         .push(0.0f, 0.0f, easing::easeOutCubic)
                         .push(0.5f, 1.0f)
                         .get(_timer.getCoefElapsed());
}

void
ModelsRenderer::render(const Camera& cameraInstance) {
  if (!_chassis.shader || !_wheels.shader)
    D_THROW(std::runtime_error, "shader not setup");

  const auto& logic = Context::get().logic;
  const auto& simulation = *logic.simulation;

  const unsigned int totalCars = simulation.getTotalCars();
  if (totalCars == 0)
    return;

  const IFrustumCulling& frustumCulling = cameraInstance.getFrustumCulling();

  const auto& matricesData = cameraInstance.getMatricesData();

  _modelsChassisMatrices.clear();
  _modelWheelsMatrices.clear();
  _modelsChassisMatrices.reserve(totalCars);   // pre-allocate
  _modelWheelsMatrices.reserve(totalCars * 4); // pre-allocate

  const glm::vec3 modelHeight(0.0f, 0.0f, 0.2f);

  const glm::vec4 k_outlineColor(1, 1, 1, _outlineColoralpha);
  const glm::vec4 k_whiteColor(1, 1, 1, _colorAlpha);
  const glm::vec4 k_greenColor(0, 1, 0, _colorAlpha);
  const glm::vec4 k_redColor(1, 0, 0, _colorAlpha);
  const glm::vec4& leaderColor = k_whiteColor;
  const glm::vec4& lifeColor = k_greenColor;
  const glm::vec4& deathColor = k_redColor;

  for (unsigned int ii = 0; ii < totalCars; ++ii) {
    const auto& carData = simulation.getCarResult(ii);

    if (!carData.isAlive)
      continue;

    //
    // 3d clipping

    const glm::mat4 chassisTransform =
      glm::translate(carData.liveTransforms.chassis, modelHeight);
    const glm::vec4 carOrigin =
      chassisTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    if (!frustumCulling.sphereInFrustum(carOrigin, 5.0f))
      continue;

    //
    // color

    const bool isLeader = (logic.leaderCar.leaderIndex() == int(ii));
    const glm::vec4 targetColor = isLeader ? leaderColor : lifeColor;
    const glm::vec4 color = glm::mix(deathColor, targetColor, carData.life);

    //
    // transforms

    _modelsChassisMatrices.emplace_back(
      chassisTransform, color, k_outlineColor);
    for (const auto& wheelTransform : carData.liveTransforms.wheels)
      _modelWheelsMatrices.emplace_back(
        wheelTransform, k_whiteColor, k_outlineColor);
  }

  if (!_modelsChassisMatrices.empty()) {
    _chassis.shader->bind();
    _chassis.shader->setUniform("u_projectionMatrix", matricesData.projection);
    _chassis.shader->setUniform("u_modelViewMatrix", matricesData.view);

    _chassis.geometry.updateBuffer(1, _modelsChassisMatrices);
    _chassis.geometry.setInstancedCount(_modelsChassisMatrices.size());
    _chassis.geometry.render();
  }

  if (!_modelWheelsMatrices.empty()) {
    _wheels.shader->bind();
    _wheels.shader->setUniform("u_composedMatrix", matricesData.composed);

    _wheels.geometry.updateBuffer(1, _modelWheelsMatrices);
    _wheels.geometry.setInstancedCount(_modelWheelsMatrices.size());
    _wheels.geometry.render();
  }
}
