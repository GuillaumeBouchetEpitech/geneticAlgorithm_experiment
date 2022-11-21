
#pragma once

#include "framework/graphic/camera/Camera.hpp"

#include "framework/graphic/ShaderProgram.hpp"
#include "framework/graphic/Geometry.hpp"

#include "framework/helpers/GLMath.hpp"

#include <vector>
#include <array>
#include <memory>

constexpr int trailSize = 30;

class FlockingManager
{
private:
  struct Boid
  {
    glm::vec3   position;
    glm::vec3   velocity;
    float       distance;

    struct TrailData
    {
      glm::vec3   position;
      float       distance;
    };

    std::array<TrailData, trailSize> trail;

    Boid();
  };
  using Boids = std::vector<Boid>;

private:
  Boids _boids;

  int _thickerTrailIndex = 0;

private:
  struct BufferAttr
  {
    glm::vec3   position;
    float       scale;
    glm::vec3   color;

    BufferAttr(const glm::vec3& position, float scale, const glm::vec3& color)
      : position(position)
      , scale(scale)
      , color(color)
    {}
  };

  std::vector<BufferAttr> _particlesInstances;

  std::shared_ptr<ShaderProgram> _shader;

  Geometry _geometry;

  Camera::MatricesData _matricesData;

public:
  FlockingManager();
  ~FlockingManager() = default;

public:
  void initialise();
  void setMatricesData(const Camera::MatricesData& matricesData);

public:
  void update();

public:
  void render();

};
