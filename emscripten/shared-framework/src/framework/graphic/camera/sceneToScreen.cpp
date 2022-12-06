
#include "sceneToScreen.hpp"

bool sceneToScreen(const glm::vec3& sceneTarget, const glm::mat4& modelview,
                   const glm::mat4& projection, const glm::vec2& viewportPos,
                   const glm::vec2& viewportSize, glm::vec3& screenCoord) {
  const float* modM = glm::value_ptr(modelview);
  const float* projM = glm::value_ptr(projection);

  // Modelview transform
  glm::vec4 vec4A;
  vec4A.x = modM[0] * sceneTarget.x + modM[4] * sceneTarget.y +
            modM[8] * sceneTarget.z + modM[12]; // w is always 1
  vec4A.y = modM[1] * sceneTarget.x + modM[5] * sceneTarget.y +
            modM[9] * sceneTarget.z + modM[13];
  vec4A.z = modM[2] * sceneTarget.x + modM[6] * sceneTarget.y +
            modM[10] * sceneTarget.z + modM[14];
  vec4A.w = modM[3] * sceneTarget.x + modM[7] * sceneTarget.y +
            modM[11] * sceneTarget.z + modM[15];

  // Projection transform, the final row of projection matrix is always [0 0 -1
  // 0] so we optimize for that.
  glm::vec4 vec4B;
  vec4B.x = projM[0] * vec4A.x + projM[4] * vec4A.y + projM[8] * vec4A.z +
            projM[12] * vec4A.w;
  vec4B.y = projM[1] * vec4A.x + projM[5] * vec4A.y + projM[9] * vec4A.z +
            projM[13] * vec4A.w;
  vec4B.z = projM[2] * vec4A.x + projM[6] * vec4A.y + projM[10] * vec4A.z +
            projM[14] * vec4A.w;
  vec4B.w = -vec4A.z;

  // The result normalizes between -1 and 1
  if (vec4B.w == 0.0f) // The w value
    return false;

  vec4B.w = 1.0f / vec4B.w;

  // Perspective division
  vec4B.x *= vec4B.w;
  vec4B.y *= vec4B.w;
  vec4B.z *= vec4B.w;

  // Window coordinates
  // Map x, y to range 0-1
  screenCoord.x = (vec4B.x * 0.5f + 0.5f) * viewportSize.x + viewportPos.x;
  screenCoord.y = (vec4B.y * 0.5f + 0.5f) * viewportSize.y + viewportPos.y;

  // This is only correct when glDepthRange(0.0, 1.0)
  screenCoord.z = (1.0f + vec4B.z) * 0.5f; // Between 0 and 1

  return true;
}
