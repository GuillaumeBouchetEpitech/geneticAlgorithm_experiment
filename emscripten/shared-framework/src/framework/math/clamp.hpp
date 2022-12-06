
#pragma once

#include <algorithm>

template <typename T> T clamp(T value, T minVal, T maxVal) {
  return std::min(std::max(value, minVal), maxVal);
}
