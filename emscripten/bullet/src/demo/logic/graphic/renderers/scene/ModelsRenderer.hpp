
#pragma once

#include "framework/graphic/camera/Camera.hpp"

#include "framework/graphic/ShaderProgram.hpp"
#include "framework/graphic/Geometry.hpp"

#include "framework/helpers/GLMath.hpp"

#include <vector>
#include <array>
#include <memory>

class ModelsRenderer
{
private:
  std::shared_ptr<ShaderProgram> _shader;

  struct Geometries
  {
    Geometry cars;
    Geometry wheels;
  }
  _geometries;

public:
  ModelsRenderer() = default;

public:
  void initialise();

public:
  void render(const Camera &cameraInstance);
};
