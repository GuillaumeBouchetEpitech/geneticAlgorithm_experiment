
#pragma once

#include "framework/helpers/GLMath.hpp"
#include "framework/system/Timer.hpp"
#include "framework/system/math/GenericEasing.hpp"

class FitnessDataRenderer {

private:
  glm::vec2 _position;
  glm::vec2 _size;
  float _alpha = 0.0f;

  bool _isVisible = false;
  Timer _timer;
  GenericEasing<2> _moveEasing;
  GenericEasing<2> _alphaEasing;

public:
  FitnessDataRenderer();

  void fadeIn(float delay, float duration);
  void fadeOut(float delay, float duration);
  void resize();

  void update(float elapsedTime);

  void renderWireframe();
  void renderHudText();
};