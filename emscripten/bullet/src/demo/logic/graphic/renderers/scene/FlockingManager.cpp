
#include "FlockingManager.hpp"

#include "demo/logic/Data.hpp"

#include "demo/logic/graphic/helpers/generateSphereVerticesFilled.hpp"

#include "demo/logic/graphicIds.hpp"

#include "framework/asValue.hpp"

#include "framework/graphic/ResourceManager.hpp"
#include "framework/graphic/GeometryBuilder.hpp"

#include "framework/math/RandomNumberGenerator.hpp"

#include "framework/graphic/GlContext.hpp"

FlockingManager::Boid::Boid()
{
  position = {0,0,0};
  position.x = RNG::getRangedValue(-0.1f, +0.1f);
  position.y = RNG::getRangedValue(-0.1f, +0.1f);
  position.z = RNG::getRangedValue(-0.1f, +0.1f);

  velocity = {0,0,0};

  distance = 0.0f;

  for (std::size_t ii = 0; ii < trail.size(); ++ii)
  {
    trail[ii].position = {0,0,0};
    trail[ii].distance = 0;
  }
}

FlockingManager::FlockingManager()
{
  constexpr std::size_t totalEntities = 16;
  _boids.resize(totalEntities);
  _particlesInstances.reserve(totalEntities);
}

void FlockingManager::initialise()
{
  _shader = ResourceManager::get().getShader(asValue(Shaders::particles));

  {
    GeometryBuilder geometryBuilder;

    std::vector<glm::vec3> particlesVertices;
    generateSphereVerticesFilled(0.5f, particlesVertices);

    geometryBuilder
      .reset()
      .setShader(*_shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
      .addVbo()
      .setVboAsInstanced()
      .addVboAttribute("a_offsetPosition", Geometry::AttrType::Vec3f, 0)
      .addVboAttribute("a_offsetScale", Geometry::AttrType::Float, 3)
      .addVboAttribute("a_offsetColor", Geometry::AttrType::Vec3f, 4);

    geometryBuilder.build(_geometry);
    _geometry.updateBuffer(0, particlesVertices);
    _geometry.setPrimitiveCount(particlesVertices.size());
  }

}

void FlockingManager::setMatricesData(const Camera::MatricesData& matricesData)
{
    _matricesData = matricesData;
}

void FlockingManager::update()
{
  auto& data = Data::get();
  const auto& logic = data.logic;
  const auto& simulation = *logic.simulation;
  glm::vec3 target = simulation.getStartPosition();
  const auto& leaderCar = logic.leaderCar;

  // valid leading car?
  if (leaderCar.index >= 0)
  {
    const auto& leaderCarData = simulation.getCarResult(leaderCar.index);

    // leading car alive?
    if (leaderCarData.isAlive)
    {
      target = leaderCarData.liveTransforms.chassis * glm::vec4(0, 0, 0, 1);
    }
  }

  constexpr float maxAcc = 0.1f;
  constexpr float maxVel = 2.0f;
  constexpr float sepRadius = 1.0f;

  for (std::size_t ii = 0; ii < _boids.size(); ++ii)
  {
    Boid& boid = _boids[ii];

    glm::vec3 acceleration = {0,0,0};

    {
      // seek

      glm::vec3 diff = target - boid.position;
      const float diffMagnitude = glm::length(diff);

      boid.distance = diffMagnitude;

      float coef = 1.0f;
      if (diffMagnitude < 20.0f)
      {
        diff = -diff; // separate
        coef = 3.0f;
      }

      if (diffMagnitude > 0.0f)
        diff = diff / diffMagnitude;

      acceleration += diff * coef;
    }
    // else
    // {
    //   // slow down

    //   const float velMagnitude = glm::length(boid.velocity);
    //   if (velMagnitude > 0.1f)
    //     boid.velocity = boid.velocity * 0.9f;
    // }

    {

      const glm::vec3& target = data.graphic.camera.scene.instance.getEye();

      glm::vec3 diff = boid.position - target;
      const float diffMagnitude = glm::length(diff);

      if (diffMagnitude < 20.0f)
      {
        if (diffMagnitude > 0.0f)
          diff = diff / diffMagnitude;

        acceleration += diff * 3.0f;
      }

    }

    {
      // separate

      glm::vec3 sep = {0,0,0};
      for (std::size_t jj = 0; jj < _boids.size(); ++jj)
      {
        if (ii == jj)
          continue;

        const Boid& other = _boids[jj];

        const glm::vec3 diff = boid.position - other.position;

        if (glm::length(diff) > sepRadius)
          continue;

        sep += diff;
      }

      const float sepVel = glm::length(sep);
      if (sepVel > 0.0f)
        sep = sep / sepVel;

      acceleration += sep * 2.0f;
    }

    {

      constexpr float maxWander = 0.5f;
      acceleration.x += RNG::getRangedValue(-maxWander, +maxWander);
      acceleration.y += RNG::getRangedValue(-maxWander, +maxWander);
      acceleration.z += RNG::getRangedValue(-maxWander, +maxWander);

    }

    // limitate acc
    const float accMagnitude = glm::length(acceleration);
    if (accMagnitude > maxAcc)
      acceleration = (acceleration / accMagnitude) * maxAcc;
    boid.velocity += acceleration;

    // limitate vel
    const float velMagnitude = glm::length(boid.velocity);
    if (velMagnitude > maxVel)
      boid.velocity = (boid.velocity / velMagnitude) * maxVel;
    boid.position += boid.velocity;

    // make a trail by reusing the previous positions N times
    for (std::size_t ii = boid.trail.size() - 1; ii > 0; --ii)
      boid.trail[ii] = boid.trail[ii - 1];
    boid.trail[0].position = boid.position;
    boid.trail[0].distance = boid.distance;
  }

  _thickerTrailIndex = (_thickerTrailIndex + 1) % trailSize;
}

void FlockingManager::render()
{
  if (!_shader)
    D_THROW(std::runtime_error, "shader not setup");

  if (_boids.empty())
    return;

  //
  //

  _particlesInstances.clear();
  for (Boid& boid : _boids)
    _particlesInstances.emplace_back(boid.position, 0.4f, glm::vec3(0.6f, 0.6f, 0.0f));

  _shader->bind();
  _shader->setUniform("u_composedMatrix", _matricesData.composed);

  _geometry.updateBuffer(1, _particlesInstances);
  _geometry.setInstancedCount(uint32_t(_particlesInstances.size()));
  _geometry.render();

  //
  //

  GlContext::disable(GlContext::States::depthTest);

  auto& stackRenderer = Data::get().graphic.stackRenderer;

  const glm::vec3 focusedColor = glm::vec3(1.0f, 1.0f, 1.0f);
  const glm::vec3 standardColor = glm::vec3(0.6f, 0.6f, 0.0f);

  for (Boid& boid : _boids)
  {
    for (std::size_t kk = 0; kk + 1 < boid.trail.size(); ++kk)
    {
      const std::size_t indexA = kk;
      const std::size_t indexB = kk + 1;
      // const bool focusedA = int(indexA) == _thickerTrailIndex;
      // const bool focusedB = int(indexB) == _thickerTrailIndex;
      const bool focusedA = false;
      const bool focusedB = false;

      const float alphaA = (boid.trail[indexA].distance > 20.0f) ? 0.4f : 0.1f;
      const float alphaB = (boid.trail[indexB].distance > 20.0f) ? 0.4f : 0.1f;

      const float thicknessA = (focusedA ? 0.2f : 0.1f);
      const float thicknessB = (focusedB ? 0.2f : 0.1f);
      const glm::vec3& colorA = (focusedA ? focusedColor : standardColor);
      const glm::vec3& colorB = (focusedB ? focusedColor : standardColor);

      stackRenderer.pushThickTriangle3DLine(
        boid.trail[indexA].position,
        boid.trail[indexB].position,
        thicknessA,
        thicknessB,
        glm::vec4(colorA, alphaA),
        glm::vec4(colorB, alphaB)
      );
    }
  }

  stackRenderer.flush();

  GlContext::enable(GlContext::States::depthTest);
}
