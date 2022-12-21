
#pragma once

#include "framework/system/Timer.hpp"
#include "framework/system/math/GenericEasing.hpp"

class InformationTextRenderer {
private:
  Timer _timer;
  GenericEasing<2> _alphaEasing;
  float _alpha = 0.0f;

public:
  void fadeIn(float delay, float duration);
  void fadeOut(float delay, float duration);

  void update(float elapsedTime);

  void render();
};
