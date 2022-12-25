
#pragma once

#include "framework/system/ErrorHandler.hpp"

#include <array>
#include <functional>

template <std::size_t _MaxSize> class GenericEasing {
private:
  struct Step {
    float coefStep;
    float value;
    std::function<float(float)> easing;
  };

  std::array<Step, _MaxSize> _steps;
  std::size_t _size = 0;

public:
  GenericEasing() { _size = 0; }

  // GenericEasing(const GenericEasing& other)
  // {
  //   if (&other == this)
  //     return;

  //   _size = other.size;
  //   _steps = other._steps;
  // }

  ~GenericEasing() = default;

public:
  GenericEasing& reset() { _size = 0; }

  GenericEasing& push(float coefStep, float value,
                      const std::function<float(float)>& easing) {
    if (_size == _MaxSize)
      D_THROW(std::runtime_error, "max steps reached, max: " << _MaxSize);
    if (_size != 0 && coefStep <= _steps.at(_size - 1).coefStep)
      D_THROW(std::runtime_error, "coef step will be missed");
    if (coefStep < 0.0f)
      D_THROW(std::runtime_error, "coef cannot be < 0");
    if (coefStep > 1.0f)
      D_THROW(std::runtime_error, "coef cannot be > 1");

    _steps.at(_size) = {coefStep, value, easing};
    _size += 1;
    return *this;
  }

  GenericEasing& push(float coefStep, float value) {
    return push(coefStep, value, [](float coef) { return coef; });
  }

  float get(float coef) const {
    if (_size < 2)
      D_THROW(std::runtime_error, "not enought coef steps");

    const Step& first = _steps.at(0);
    if (coef < first.coefStep)
      return first.value;

    const Step& last = _steps.at(_size - 1);
    if (coef >= last.coefStep)
      return last.value;

    for (std::size_t index = 0; index + 1 < _size; ++index) {
      const Step& currStep = _steps.at(index);
      const Step& nextStep = _steps.at(index + 1);

      if (coef >= currStep.coefStep && coef < nextStep.coefStep) {
        const float subCoef =
          (coef - currStep.coefStep) / (nextStep.coefStep - currStep.coefStep);

        return currStep.value +
               currStep.easing(subCoef) * (nextStep.value - currStep.value);
      }
    }

    D_THROW(std::runtime_error, "unreachable");
  }
};

// using EasingTransition = GenericEasing<16>;
