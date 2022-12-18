
#pragma once

#include <array>

class AnimationTransition
{
private:
  struct Step
  {
    float coefStep;
    float value;
  };

  static constexpr std::size_t _maxSteps = 16;
  std::array<Step, _maxSteps> _steps;
  std::size_t _size = 0;

public:
  AnimationTransition();
  ~AnimationTransition() = default;

public:
  AnimationTransition& push(float coefStep, float value);
  float get(float coef);

};
