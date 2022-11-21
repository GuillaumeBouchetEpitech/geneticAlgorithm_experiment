
#pragma once

#include "framework/graphic/camera/Camera.hpp"

#include "framework/graphic/ShaderProgram.hpp"
#include "framework/graphic/Geometry.hpp"

#include "framework/helpers/GLMath.hpp"

#include <memory>
#include <array>

class CarTailsRenderer
{
private:

  std::shared_ptr<ShaderProgram> _shader;

  Camera::MatricesData _matricesData;

  struct Geometries
  {
    struct WheelsTrail
    {
      std::array<Geometry, 4> wheels;
    };

    std::array<WheelsTrail, 5> bestNewCarsTrails;

    Geometry leaderCarTrail;
  }
  _geometries;

  unsigned int _currentTrailIndex = 0;

public:
  CarTailsRenderer() = default;
  ~CarTailsRenderer() = default;

public:
  void initialise();
  void setMatricesData(const Camera::MatricesData& matricesData);

public:
  void updateLatestTrail();

public:
  void render();

};
