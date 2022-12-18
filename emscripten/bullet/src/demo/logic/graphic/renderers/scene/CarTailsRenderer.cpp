
#include "CarTailsRenderer.hpp"

#include "demo/logic/Context.hpp"
#include "demo/logic/graphicIds.hpp"

#include "framework/asValue.hpp"

void CarTailsRenderer::initialise() {

  auto& resourceManager = Context::get().graphic.resourceManager;

  _shader = resourceManager.getShader(asValue(ShaderIds::wireframes));

  auto geoDef = resourceManager.getGeometryDefinition(
    asValue(GeometryIds::wireframesLineStrip));
  _geometries.leaderCarTrail.initialise(*_shader, geoDef);
  _geometries.leaderCarTrail.setPrimitiveCount(0);

  for (auto& wheelsTrail : _geometries.bestNewCarsTrails)
    for (auto& wheel : wheelsTrail.wheels) {
      wheel.initialise(*_shader, geoDef);
      wheel.setPrimitiveCount(0);
    }
}

void CarTailsRenderer::setMatricesData(
  const Camera::MatricesData& matricesData) {
  _matricesData = matricesData;
}

void CarTailsRenderer::updateLatestTrail() {
  auto& context = Context::get();
  auto& logic = context.logic;

  const auto& bestGenome = logic.simulation->getBestGenome();

  const auto& bestWheelsTrailData =
    logic.carWheelsTrails.getTrailById(bestGenome.id);

  auto& currCarNewTrail = _geometries.bestNewCarsTrails.at(_currentTrailIndex);

  for (std::size_t ii = 0; ii < currCarNewTrail.wheels.size(); ++ii) {
    currCarNewTrail.wheels.at(ii).updateBuffer(
      0, bestWheelsTrailData.wheels.at(ii));
    currCarNewTrail.wheels.at(ii).setPrimitiveCount(
      bestWheelsTrailData.wheels.at(ii).size());
  }

  // increase the currently used trail index (loop if too high)
  _currentTrailIndex =
    (_currentTrailIndex + 1) % _geometries.bestNewCarsTrails.size();
}

void CarTailsRenderer::render() {

  if (!_shader)
    D_THROW(std::runtime_error, "shader not setup");

  auto& logic = Context::get().logic;

  _shader->bind();
  _shader->setUniform("u_composedMatrix", _matricesData.composed);
  _shader->setUniform("u_color", 1.0f, 1.0f, 1.0f, 1.0f);

  { // best trails

    for (const auto& currentCarTrail : _geometries.bestNewCarsTrails)
      for (const auto& wheelTrail : currentCarTrail.wheels)
        wheelTrail.render();

  } // best trails

  { // leader trail

    if (logic.leaderCar.hasLeader()) {
      const auto& trailData =
        logic.carWheelsTrails.getTrailByIndex(logic.leaderCar.leaderIndex());

      // rely on only the 30 last positions recorded
      constexpr int maxSize = 30;

      for (const auto& currWheel : trailData.wheels) {
        if (currWheel.empty())
          continue;

        const int totalSize = currWheel.size();
        const int currSize = std::min(totalSize, maxSize);

        const float* dataPointer = &currWheel.at(totalSize - currSize).x;
        const int dataSize = currSize * sizeof(glm::vec3);

        _geometries.leaderCarTrail.updateBuffer(0, dataPointer, dataSize, true);
        _geometries.leaderCarTrail.setPrimitiveCount(currSize);
        _geometries.leaderCarTrail.render();
      }
    }

  } // leader trail
}
