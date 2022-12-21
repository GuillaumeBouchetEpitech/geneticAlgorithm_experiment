
#pragma once

#include "framework/helpers/GLMath.hpp"
#include "framework/system/Timer.hpp"
#include "framework/system/math/GenericEasing.hpp"

class CoreUsageRenderer {

private:
  glm::vec2 _position;
  glm::vec2 _size;

  bool _isVisible = false;
  Timer _timer;
  GenericEasing<2> _moveEasing;

public:
  CoreUsageRenderer();

  void fadeIn(float delay, float duration);
  void fadeOut(float delay, float duration);
  void resize();

  void update(float elapsedTime);

  void renderWireframe();
  void renderHudText();
};