
#include "AnimationTransition.hpp"

#include "framework/ErrorHandler.hpp"

AnimationTransition::AnimationTransition()
{
  _size = 0;
}

AnimationTransition& AnimationTransition::push(float coefStep, float value)
{
  if (_size == _maxSteps)
    D_THROW(std::runtime_error, "max steps reached");
  if (_size != 0 && coefStep <= _steps.at(_size - 1).coefStep)
    D_THROW(std::runtime_error, "coef step will be missed");

  _steps.at(_size) = { coefStep, value };
  _size += 1;
  return *this;
}

float AnimationTransition::get(float coef)
{
  if (_size < 2)
    D_THROW(std::runtime_error, "not enought coef steps");

  if (coef < _steps.at(0).coefStep)
    return _steps.at(0).value;
  if (coef > _steps.at(_size - 1).coefStep)
    return _steps.at(_size - 1).value;

  for (std::size_t index = 0; index + 1 < _size; ++index)
  {
    Step& currStep = _steps.at(index);
    Step& nextStep = _steps.at(index + 1);

    if (coef >= currStep.coefStep && coef < nextStep.coefStep)
    {
      const float subCoef = (coef - currStep.coefStep) / (nextStep.coefStep - currStep.coefStep);
      return currStep.value + subCoef * (nextStep.value - currStep.value);
    }
  }

  D_THROW(std::runtime_error, "unreachable");
}

