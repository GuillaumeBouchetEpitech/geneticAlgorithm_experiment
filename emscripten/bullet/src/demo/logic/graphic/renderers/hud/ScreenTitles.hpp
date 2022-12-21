
#pragma once

#include "framework/system/Timer.hpp"
#include "framework/system/math/GenericEasing.hpp"

class ScreenTitles {
private:
  Timer _timer;

  GenericEasing<3> _backgroundEasing;
  GenericEasing<3> _mainTitleEasing;
  GenericEasing<3> _fitnessTitleEasing;
  GenericEasing<3> _commentTitleEasing;

  float _backgroundAlpha = 1.0f;
  float _mainTitleAlpha = 0.0f;
  float _fitnessTitleAlpha = 0.0f;
  float _commentTitleAlpha = 0.0f;

public:
  void fadeIn(float delay, float duration);
  void fadeOut(float delay, float duration);

  void update(float elapsedTime);
  void render();
};
