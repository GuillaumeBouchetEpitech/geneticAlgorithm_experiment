
#pragma once

#include "framework/helpers/GLMath.hpp"

bool sceneToScreen(
  const glm::vec3& obj,
  const glm::mat4& modelview2, const glm::mat4& projection2,
  const glm::vec2& viewportPos, const glm::vec2& viewportSize,
  glm::vec3& screenCoord);
