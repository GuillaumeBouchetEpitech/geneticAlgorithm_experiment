
#include "CarTailsRenderer.hpp"

#include "demo/logic/Data.hpp"

#include "demo/logic/graphicIds.hpp"

#include "framework/asValue.hpp"

#include "framework/graphic/ResourceManager.hpp"
#include "framework/graphic/GeometryBuilder.hpp"

void CarTailsRenderer::initialise()
{
  _shader = ResourceManager::get().getShader(asValue(Shaders::wireframes));

  {

    GeometryBuilder geometryBuilder;

    geometryBuilder
      .reset()
      .setShader(*_shader)
      .setPrimitiveType(Geometry::PrimitiveType::line_strip)
      .addVbo()
      .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0);

    for (auto& wheelsTrail : _geometries.bestNewCarsTrails)
      for (auto& wheel : wheelsTrail.wheels)
        geometryBuilder.build(wheel);

    geometryBuilder.build(_geometries.leaderCarTrail);

  }

}

void CarTailsRenderer::setMatricesData(const Camera::MatricesData& matricesData)
{
  _matricesData = matricesData;
}

void CarTailsRenderer::updateLatestTrail()
{
  auto& data = Data::get();
  auto& logic = data.logic;

  const auto& bestGenome = logic.simulation->getBestGenome();

  const auto& bestWheelsTrailData = logic.carWheelsTrails.getTrailById(bestGenome.id);

  auto& currCarNewTrail = _geometries.bestNewCarsTrails[_currentTrailIndex];

  for (std::size_t ii = 0; ii < currCarNewTrail.wheels.size(); ++ii)
  {
    currCarNewTrail.wheels[ii].updateBuffer(0, bestWheelsTrailData.wheels[ii]);
    currCarNewTrail.wheels[ii].setPrimitiveCount(bestWheelsTrailData.wheels[ii].size());
  }

  // increase the currently used trail index (loop if too high)
  _currentTrailIndex = (_currentTrailIndex + 1) % _geometries.bestNewCarsTrails.size();
}

void CarTailsRenderer::render()
{

  if (!_shader)
    D_THROW(std::runtime_error, "shader not setup");

  auto& data = Data::get();
  auto& logic = data.logic;

  _shader->bind();
  _shader->setUniform("u_composedMatrix", _matricesData.composed);
  _shader->setUniform("u_color", 1.0f, 1.0f, 1.0f, 1.0f);

  { // best trails

    for (const auto& currentCarTrail : _geometries.bestNewCarsTrails)
      for (const auto& wheelTrail : currentCarTrail.wheels)
        wheelTrail.render();

  } // best trails

  { // leader trail

    if (logic.leaderCar.index >= 0)
    {
      const auto& trailData = logic.carWheelsTrails.getTrailByIndex(logic.leaderCar.index);

      // rely on only the 30 last positions recorded
      constexpr int maxSize = 30;

      for (const auto& currWheel : trailData.wheels)
      {
        if (currWheel.empty())
          continue;

        const int totalSize = currWheel.size();
        const int currSize = std::min(totalSize, maxSize);

        const float* dataPointer = &currWheel[totalSize - currSize].x;
        const int dataSize = currSize * sizeof(glm::vec3);

        _geometries.leaderCarTrail.updateBuffer(0, dataPointer, dataSize, true);
        _geometries.leaderCarTrail.setPrimitiveCount(currSize);
        _geometries.leaderCarTrail.render();
      }
    }

  } // leader trail
}
