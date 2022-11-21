
#include "rotate2d.hpp"

#include "angles.hpp"

glm::vec2 rotate2d(const glm::vec2& point, const glm::vec2& center, float angle)
{
  const glm::vec2 dir = getDirection(angle);
  const glm::vec2 diff = point - center;

  return glm::vec2(
    center.x + diff.x * dir.x + diff.y * dir.y,
    center.y + diff.x * dir.y - diff.y * dir.x
  );
}

glm::vec3 rotate2d(const glm::vec3& point, const glm::vec2& center, float angle)
{
  return glm::vec3(rotate2d(glm::vec2(point), center, angle), point.z);
}
