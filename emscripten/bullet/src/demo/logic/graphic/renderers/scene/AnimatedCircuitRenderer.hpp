
#pragma once

#include "framework/graphic/ShaderProgram.hpp"
#include "framework/graphic/Geometry.hpp"
#include "framework/graphic/Texture.hpp"

#include "framework/graphic/camera/Camera.hpp"

#include <array>
#include <memory>

class AnimatedCircuitRenderer
{
public:

  struct AnimatedVertex
  {
    glm::vec3 postion;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec3 animatedNormal; // <= animation
    float limitId; // <= animation

    AnimatedVertex() = default;

    AnimatedVertex(
        const glm::vec3& postion,
        const glm::vec3& color,
        const glm::vec3& normal,
        const glm::vec3& animatedNormal,
        float limitId)
        : postion(postion)
        , color(color)
        , normal(normal)
        , animatedNormal(animatedNormal)
        , limitId(limitId)
    {}
  };

  using AnimatedVertices = std::vector<AnimatedVertex>;

public:
  AnimatedCircuitRenderer() = default;
  ~AnimatedCircuitRenderer() = default;

public:
  void initialise(
    const std::vector<glm::vec3>& skeletonVertices,
    const AnimatedVertices& groundVertices,
    const AnimatedVertices& wallsVertices,
    float maxUpperValue);
  void setMatricesData(const Camera::MatricesData& matricesData);

public:
  void update(float elapsedTime);
  void renderWireframe();
  void renderWalls();
  void renderGround();

private:
  std::shared_ptr<ShaderProgram> _shaderWireframe;
  std::shared_ptr<ShaderProgram> _shaderCircuitLit;
  std::shared_ptr<ShaderProgram> _shaderCircuit;

  struct Geometries
  {
    Geometry skeleton;
    Geometry grounds;
    Geometry walls;
  }
  _geometries;

  float _targetValue = 10.0f;
  float _lowerValue = 10.0f;
  float _upperValue = 10.0f;
  float _maxUpperValue = 0.0f;
  std::size_t _maxPrimitiveCount = 0;

  Camera::MatricesData _matricesData;

};
