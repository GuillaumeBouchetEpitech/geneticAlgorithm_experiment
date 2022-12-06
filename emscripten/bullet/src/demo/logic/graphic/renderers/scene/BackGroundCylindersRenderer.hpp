
#pragma once

#include "framework/graphic/Geometry.hpp"
#include "framework/graphic/ShaderProgram.hpp"
#include "framework/graphic/Texture.hpp"

#include "framework/graphic/camera/Camera.hpp"

#include <array>
#include <memory>

class BackGroundCylindersRenderer {
public:
  BackGroundCylindersRenderer() = default;
  ~BackGroundCylindersRenderer() = default;

public:
  void initialise(const glm::vec3& size);
  void setMatricesData(const Camera::MatricesData& matricesData);

public:
  void update(float elapsedTime);
  void render(const glm::vec3& center);

private:
  std::shared_ptr<ShaderProgram> _shader;

  std::array<Texture, 4> _textures;
  std::array<Geometry, 4> _geometries;

  Camera::MatricesData _matricesData;

  float _cylinderAnimationTime = 0.0f;
};
