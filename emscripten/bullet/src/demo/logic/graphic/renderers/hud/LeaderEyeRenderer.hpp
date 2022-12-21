
#pragma once

#include "framework/system/Timer.hpp"
#include "framework/system/math/GenericEasing.hpp"

class LeaderEyeRenderer {
private:
  glm::vec2 _position = {0, 0};
  bool _isVisible = false;
  Timer _timer;
  GenericEasing<2> _moveEasing;

public:
  void initialise();

public:
  void fadeIn(float delay, float duration);
  void fadeOut(float delay, float duration);
  void resize();

  void update(float elapsedTime);

  void render();
};
