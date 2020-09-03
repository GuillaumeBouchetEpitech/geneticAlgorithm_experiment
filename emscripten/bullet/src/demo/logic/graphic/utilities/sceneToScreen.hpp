
#pragma once

#include "thirdparty/GLMath.hpp"

bool sceneToScreen(const glm::vec3& obj,
                   const glm::mat4& modelview, const glm::mat4& projection,
                   const glm::vec2& viewportPos, const glm::vec2& viewportSize,
                   glm::vec3& screenCoord);
