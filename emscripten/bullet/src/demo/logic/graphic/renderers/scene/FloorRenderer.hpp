
#pragma once

#include "framework/graphic/ShaderProgram.hpp"
#include "framework/graphic/Geometry.hpp"
#include "framework/graphic/Texture.hpp"

#include "framework/graphic/camera/Camera.hpp"

#include "framework/helpers/GLMath.hpp"

#include <vector>
#include <memory>

class FloorRenderer
{
public:
  FloorRenderer() = default;
  ~FloorRenderer() = default;

public:
  void initialise(
    const glm::vec3& center,
    const glm::vec3& size);
  void setMatricesData(const Camera::MatricesData& matricesData);

public:
  void render();

private:
  std::shared_ptr<ShaderProgram> _shader;

  Texture _texture;
  Geometry _geometry;

  Camera::MatricesData _matricesData;

};
