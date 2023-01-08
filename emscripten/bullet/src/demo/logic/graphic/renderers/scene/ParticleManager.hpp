
#pragma once

#include "framework/graphic/Geometry.hpp"
#include "framework/graphic/ShaderProgram.hpp"
#include "framework/graphic/camera/Camera.hpp"
#include "framework/helpers/GLMath.hpp"
#include "framework/system/containers/dynamic_heap_array.hpp"

#include <array>
#include <memory>
#include <vector>

class ParticleManager {
private:
  struct ExplosionParticle {
    glm::vec3 position;
    glm::vec3 linearVelocity;
    float scale;
    glm::vec3 color;
    float life;
    float maxLife;

    ExplosionParticle(
      const glm::vec3& position, const glm::vec3& linearVelocity,
      const glm::vec3& color, float scale, float life);
  };

  struct TrailParticle : public ExplosionParticle {
    static constexpr std::size_t trail_size = 2;
    std::array<glm::vec3, trail_size> trail;

    TrailParticle(
      const glm::vec3& position, const glm::vec3& linearVelocity,
      const glm::vec3& color, float scale, float life);
  };

  using ExplosionParticles = dynamic_heap_array<ExplosionParticle>;
  using TrailParticles = dynamic_heap_array<TrailParticle>;

private:
  struct ParticleInstance {
    glm::vec3 position;
    float scale;
    glm::vec3 color;

    ParticleInstance(
      const glm::vec3& position, float scale, const glm::vec3& color)
      : position(position), scale(scale), color(color) {}
  };

  TrailParticles _trailParticles;
  ExplosionParticles _smallExplosionParticles;
  ExplosionParticles _bigExplosionParticles;

  std::vector<ParticleInstance> _smallSphereInstances;
  std::vector<ParticleInstance> _bigSphereInstances;
  std::shared_ptr<ShaderProgram> _shader;
  Geometry _geometrySmallSphere;
  Geometry _geometryBigSphere;

  Camera::MatricesData _matricesData;

public:
  ParticleManager() = default;

public:
  void initialise();
  void setMatricesData(const Camera::MatricesData& matricesData);

public:
  void update(float delta);
  void render();

public:
  void emitParticles(const glm::vec3& position, const glm::vec3& velocity);
};
