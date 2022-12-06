
#include "ParticleManager.hpp"

#include "demo/logic/Context.hpp"

#include "demo/logic/graphic/helpers/generateSphereVerticesFilled.hpp"

#include "demo/logic/graphicIds.hpp"

#include "framework/asValue.hpp"

#include "framework/graphic/GeometryBuilder.hpp"
#include "framework/graphic/ResourceManager.hpp"

#include "framework/math/RandomNumberGenerator.hpp"

namespace /*anonymous*/
{

std::array<const glm::vec3, 3> k_particleColors{{
  {1.0f, 0.0f, 0.0f}, // red
  {1.0f, 1.0f, 0.0f}, // yellow
  {1.0f, 0.5f, 0.0f}, // orange
}};

glm::vec3 getRandomVec3(float radius) {
  return glm::vec3(RNG::getRangedValue(-radius, radius),
                   RNG::getRangedValue(-radius, radius),
                   RNG::getRangedValue(-radius, radius));
}

}; // namespace

ParticleManager::ExplosionParticle::ExplosionParticle(
  const glm::vec3& position, const glm::vec3& linearVelocity,
  const glm::vec3& color, float scale, float life)
  : position(position), linearVelocity(linearVelocity), scale(scale),
    color(color), life(life), maxLife(life) {}

ParticleManager::TrailParticle::TrailParticle(const glm::vec3& position,
                                              const glm::vec3& linearVelocity,
                                              const glm::vec3& color,
                                              float scale, float life)
  : ExplosionParticle(position, linearVelocity, color, scale, life) {
  // initialise the particle's trail
  for (auto& trailPos : trail)
    trailPos = position;
}

void ParticleManager::initialise() {
  _shader = Context::get().graphic.resourceManager.getShader(
    asValue(Shaders::particles));

  {

    GeometryBuilder geometryBuilder;

    geometryBuilder.reset()
      .setShader(*_shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
      .addVbo()
      .setVboAsInstanced()
      .addVboAttribute("a_offsetPosition", Geometry::AttrType::Vec3f, 0)
      .addVboAttribute("a_offsetScale", Geometry::AttrType::Float, 3)
      .addVboAttribute("a_offsetColor", Geometry::AttrType::Vec3f, 4);

    std::vector<glm::vec3> particlesVertices;

    generateSphereVerticesFilled(0.5f, 0, particlesVertices);
    geometryBuilder.build(_geometrySmallSphere);
    _geometrySmallSphere.updateBuffer(0, particlesVertices);
    _geometrySmallSphere.setPrimitiveCount(particlesVertices.size());

    generateSphereVerticesFilled(0.5f, 1, particlesVertices);
    geometryBuilder.build(_geometryBigSphere);
    _geometryBigSphere.updateBuffer(0, particlesVertices);
    _geometryBigSphere.setPrimitiveCount(particlesVertices.size());
  }

  _trailParticles.pre_allocate(2048);
  _smallSphereInstances.reserve(2048); // pre-allocate
  _bigSphereInstances.reserve(2048);   // pre-allocate
}

void ParticleManager::setMatricesData(
  const Camera::MatricesData& matricesData) {
  _matricesData = matricesData;
}

void ParticleManager::update(float delta) {
  // update particle's life and handle removal of dead particles
  for (std::size_t ii = 0; ii < _trailParticles.size();) {
    _trailParticles.at(ii).life -= delta;
    if (_trailParticles.at(ii).life > 0.0f) {
      ++ii;
      continue;
    }

    _trailParticles.unsorted_erase(ii);
  }

  // update particle's life and handle removal of dead particles
  for (std::size_t ii = 0; ii < _smallExplosionParticles.size();) {
    _smallExplosionParticles.at(ii).life -= delta;
    if (_smallExplosionParticles.at(ii).life > 0.0f) {
      ++ii;
      continue;
    }

    _smallExplosionParticles.unsorted_erase(ii);
  }

  // update particle's life and handle removal of dead particles
  for (std::size_t ii = 0; ii < _bigExplosionParticles.size();) {
    _bigExplosionParticles.at(ii).life -= delta;
    if (_bigExplosionParticles.at(ii).life > 0.0f) {
      ++ii;
      continue;
    }

    _bigExplosionParticles.unsorted_erase(ii);
  }

  for (auto& trailParticle : _trailParticles) {
    // make a trail by reusing the previous positions N times
    auto& trail = trailParticle.trail;
    for (std::size_t ii = trail.size() - 1; ii > 0; --ii)
      trail.at(ii) = trail.at(ii - 1);
    trail.at(0) = trailParticle.position;

    // update current position
    trailParticle.position += trailParticle.linearVelocity * delta;

    if (trailParticle.position.z < 0.0f) {
      trailParticle.position.z = 0.0f;

      // bounce off the ground
      trailParticle.linearVelocity.z += 100.0f * delta;
    } else {
      // apply fake gravity
      trailParticle.linearVelocity.z -= 20.0f * delta;
    }
  }

  for (auto& currParticle : _smallExplosionParticles) {
    // update current position
    currParticle.position += currParticle.linearVelocity * delta;

    if (currParticle.position.z < 0.0f) {
      currParticle.position.z = 0.0f;

      // bounce off the ground
      currParticle.linearVelocity.z += 100.0f * delta;
    } else {
      // apply fake gravity
      currParticle.linearVelocity.z -= 20.0f * delta;
    }
  }

  for (auto& currParticle : _bigExplosionParticles) {
    // update current position
    currParticle.position += currParticle.linearVelocity * delta;

    if (currParticle.position.z < 0.0f) {
      currParticle.position.z = 0.0f;

      // bounce off the ground
      currParticle.linearVelocity.z += 100.0f * delta;
    } else {
      // apply fake gravity
      currParticle.linearVelocity.z -= 20.0f * delta;
    }
  }
}

void ParticleManager::render() {

  for (auto& particle : _smallExplosionParticles) {
    // update scale
    const float localScale = particle.life / particle.maxLife * particle.scale;

    _smallSphereInstances.emplace_back(particle.position, localScale,
                                       particle.color);
  }

  for (auto& particle : _bigExplosionParticles) {
    // update scale
    const float localScale = particle.life / particle.maxLife * particle.scale;

    _bigSphereInstances.emplace_back(particle.position, localScale,
                                     particle.color);
  }

  if (!_smallSphereInstances.empty() || !_bigSphereInstances.empty()) {
    if (!_shader)
      D_THROW(std::runtime_error, "shader not setup");

    _shader->bind();
    _shader->setUniform("u_composedMatrix", _matricesData.composed);

    if (!_smallSphereInstances.empty()) {
      _geometrySmallSphere.updateBuffer(1, _smallSphereInstances, true);
      _geometrySmallSphere.setInstancedCount(
        uint32_t(_smallSphereInstances.size()));
      _geometrySmallSphere.render();

      _smallSphereInstances.clear();
    }
    if (!_bigSphereInstances.empty()) {
      _geometryBigSphere.updateBuffer(1, _bigSphereInstances, true);
      _geometryBigSphere.setInstancedCount(
        uint32_t(_bigSphereInstances.size()));
      _geometryBigSphere.render();

      _bigSphereInstances.clear();
    }
  }

  {
    auto& stackRenderer = Context::get().graphic.stackRenderers.triangles;

    for (auto& trailParticle : _trailParticles) {
      // update scale
      const float localScale =
        trailParticle.life / trailParticle.maxLife * trailParticle.scale;

      const glm::vec4 particleColor = glm::vec4(trailParticle.color, 1);

      const auto& trail = trailParticle.trail;

      for (std::size_t ii = 0; ii + 1 < trail.size(); ++ii) {
        stackRenderer.pushThickTriangle3dLine(
          trail.at(ii + 0), trail.at(ii + 1), localScale * 0.5f, particleColor);
      }
      stackRenderer.pushThickTriangle3dLine(trailParticle.position, trail.at(0),
                                            localScale * 0.5f, particleColor);
    }
  }
}

void ParticleManager::emitParticles(const glm::vec3& position,
                                    const glm::vec3& velocity) {
  const unsigned int totalParticles = RNG::getRangedValue(5, 8);

  const float maxVelLength = 10.0f;
  const float velLength = std::min(glm::length(velocity), maxVelLength);
  glm::vec3 normalizedVel(0);
  if (velLength > 0) {
    if (velLength < maxVelLength)
      normalizedVel = velocity / maxVelLength; // smaller than max velocity
    else
      normalizedVel = glm::normalize(velocity); // max velocity
  }

  for (unsigned int ii = 0; ii < totalParticles; ++ii) {
    const float maxVelocity = RNG::getRangedValue(15.0f, 25.0f);
    const glm::vec3 particleVel =
      glm::normalize(normalizedVel + getRandomVec3(0.25f)) * maxVelocity;
    const glm::vec3 color = k_particleColors.at(RNG::getRangedValue(0, 3));
    const float scale = RNG::getRangedValue(0.5f, 1.5f);
    const float life = RNG::getRangedValue(0.5f, 1.5f);

    _trailParticles.emplace_back(position, particleVel, color, scale, life);
  }

  for (unsigned int ii = 0; ii < totalParticles; ++ii) {
    const glm::vec3 particlePos = position + getRandomVec3(1.0f);
    const glm::vec3 color = k_particleColors.at(RNG::getRangedValue(0, 3));

    const float maxVelocity = RNG::getRangedValue(15.0f, 25.0f);
    const glm::vec3 linearVelocity =
      glm::normalize(normalizedVel + getRandomVec3(0.25f)) * maxVelocity;

    const float scale = RNG::getRangedValue(0.5f, 1.5f);

    const float life = RNG::getRangedValue(0.5f, 1.5f);

    _smallExplosionParticles.emplace_back(particlePos, linearVelocity, color,
                                          scale, life);
  }

  for (unsigned int ii = 0; ii < 3; ++ii) {
    const glm::vec3 particlePos = position + getRandomVec3(1.0f);

    const int colorIndex = RNG::getRangedValue(0, 3);
    const glm::vec3 color = k_particleColors.at(colorIndex);

    const glm::vec3 linearVelocity = glm::vec3(0, 0, 0);

    const float scale = RNG::getRangedValue(3.0f, 5.0f);

    const float life = RNG::getRangedValue(0.25f, 0.75f);

    _bigExplosionParticles.emplace_back(particlePos, linearVelocity, color,
                                        scale, life);
  }
}
