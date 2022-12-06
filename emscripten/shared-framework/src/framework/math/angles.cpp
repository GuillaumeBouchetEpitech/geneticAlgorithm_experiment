
#include "angles.hpp"

#include <cmath>

float getAngle(const glm::vec2& direction) {
  return std::atan2(direction.y, direction.x);
}

glm::vec2 getDirection(float angle) {
  return glm::vec2(std::cos(angle), std::sin(angle));
}
