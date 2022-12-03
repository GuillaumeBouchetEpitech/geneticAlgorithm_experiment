
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
  position = { 0, 0, 0 };
  position.x = RNG::getRangedValue(-10.0f, +10.0f);
  position.y = RNG::getRangedValue(-10.0f, +10.0f);
  position.z = RNG::getRangedValue(-10.0f, +10.0f);

  acceleration = { 0, 0, 0 };
  velocity = { 0, 0, 0 };

  for (auto& item : trail)
    item = position;
}

void FlockingManager::Boid::seek(const glm::vec3& target, float coef)
{
  glm::vec3 diff = target - position;
  const float diffMagnitude = glm::length(diff);

  if (diffMagnitude > 0.0f)
    diff = diff / diffMagnitude;

  acceleration += diff * coef;
}

void FlockingManager::Boid::separate(const glm::vec3& target, float coef)
{
  return seek(target, -coef);
}

void FlockingManager::Boid::separate(const std::vector<Boid>& boids, float radius, float coef)
{
  glm::vec3 total = {0,0,0};
  for (const Boid& other : boids)
  {
    if (*this == other)
      continue;

    const glm::vec3 diff = position - other.position;

    if (glm::length(diff) > radius)
      continue;

    total += diff;
  }

  const float magnitude = glm::length(total);
  if (magnitude > 0.0f)
    total = total / magnitude;

  acceleration += total * coef;
}

void FlockingManager::Boid::wander(float coef)
{
  acceleration.x += RNG::getRangedValue(-coef, +coef);
  acceleration.y += RNG::getRangedValue(-coef, +coef);
  acceleration.z += RNG::getRangedValue(-coef, +coef);
}

void FlockingManager::Boid::applyAcceleration(float maxAcceleration, float maxVelocity)
{
  // limitate acc
  const float accMagnitude = glm::length(acceleration);
  if (accMagnitude > maxAcceleration)
    acceleration = (acceleration / accMagnitude) * maxAcceleration;
  velocity += acceleration;

  // limitate vel
  const float velMagnitude = glm::length(velocity);
  if (velMagnitude > maxVelocity)
    velocity = (velocity / velMagnitude) * maxVelocity;
  position += velocity;
}

bool FlockingManager::Boid::operator==(const Boid& other) const
{
  return this == &other;
}

FlockingManager::FlockingManager()
{
  constexpr std::size_t totalEntities = 16;
  _boids.resize(totalEntities);
  _particlesInstances.reserve(totalEntities);
}

void FlockingManager::initialise()
{
  _shader = Data::get().graphic.resourceManager.getShader(asValue(Shaders::particles));

  {
    GeometryBuilder geometryBuilder;

    std::vector<glm::vec3> particlesVertices;
    generateSphereVerticesFilled(0.5f, 0, particlesVertices);

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
  if (auto leaderData = leaderCar.leaderData())
  {
    // leading car alive?
    if (leaderData->isAlive)
    {
      target = leaderData->liveTransforms.chassis * glm::vec4(0, 0, 0, 1);
    }
  }

  constexpr float maxAcc = 0.1f;
  constexpr float maxVel = 2.0f;

  for (Boid& boid : _boids)
  {
    boid.acceleration = { 0, 0, 0 };

    const float distance = glm::distance(boid.position, target);

    if (distance > 20.0f)
      boid.seek(target, 1.0f);
    else
      boid.separate(target, 3.0f);

    boid.separate(_boids, 1.5f, 2.0f);
    boid.wander(0.5f);

    const float coef = distance > 30.0f ? 3.0f : 1.0f;
    boid.applyAcceleration(maxAcc * coef, maxVel * coef);

    // make a trail by reusing the previous positions N times
    for (std::size_t ii = boid.trail.size() - 1; ii > 0; --ii)
      boid.trail[ii] = boid.trail[ii - 1];
    boid.trail[0] = boid.position;
  }
}

void FlockingManager::render()
{
  if (!_shader)
    D_THROW(std::runtime_error, "shader not setup");

  if (_boids.empty())
    return;

  //
  //

  {

    _particlesInstances.clear();
    for (Boid& boid : _boids)
      _particlesInstances.emplace_back(boid.position, 0.4f, glm::vec3(0.6f, 0.6f, 0.0f));

    if (!_particlesInstances.empty())
    {
      _shader->bind();
      _shader->setUniform("u_composedMatrix", _matricesData.composed);

      _geometry.updateBuffer(1, _particlesInstances);
      _geometry.setInstancedCount(uint32_t(_particlesInstances.size()));
      _geometry.render();
    }

  }

  //
  //

  {

    GlContext::disable(GlContext::States::depthTest);

    auto& stackRenderer = Data::get().graphic.stackRenderer;

    const glm::vec4 color = glm::vec4(0.6f, 0.6f, 0.0f, 0.2f);

    for (Boid& boid : _boids)
      for (std::size_t kk = 0; kk + 1 < boid.trail.size(); ++kk)
        stackRenderer.pushThickTriangle3dLine(
          boid.trail[kk + 0],
          boid.trail[kk + 1],
          0.2f,
          color
        );

    stackRenderer.flush();

    GlContext::enable(GlContext::States::depthTest);

  }
}
