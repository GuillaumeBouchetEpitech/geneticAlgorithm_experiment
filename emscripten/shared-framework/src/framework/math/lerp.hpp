
#pragma once

template<typename T>
T lerp(T valueA, T valueB, float coef)
{
  return valueA + (valueB - valueA) * coef;
}
