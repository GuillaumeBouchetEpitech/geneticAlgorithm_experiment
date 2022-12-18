
#pragma once

#include "framework/graphic/Geometry.hpp"
#include "framework/graphic/ShaderProgram.hpp"
#include "framework/graphic/Texture.hpp"

#include "framework/graphic/camera/Camera.hpp"

#include <array>
#include <memory>

class BackGroundTorusRenderer {
public:
  BackGroundTorusRenderer() = default;
  ~BackGroundTorusRenderer() = default;

public:
  void initialise();

public:
  void update(float elapsedTime);
  void render(const glm::vec3& center);

private:
  std::shared_ptr<ShaderProgram> _shader;
  Geometry _geometry;
  Texture _texture;

  float _animationTime = 0.0f;
};
