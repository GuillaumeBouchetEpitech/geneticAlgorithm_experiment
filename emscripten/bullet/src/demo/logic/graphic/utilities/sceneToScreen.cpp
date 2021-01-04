
#include "sceneToScreen.hpp"

bool sceneToScreen(const glm::vec3& obj,
                   const glm::mat4& modl, const glm::mat4& proj,
                   const glm::vec2& viewportPos, const glm::vec2& viewportSize,
                   glm::vec3& screenCoord)
{
    glm::vec4 projPos = proj * modl * glm::vec4(obj, 1.0f);

    // the result normalizes between -1 and 1
    if (projPos.w == 0.0f)
        return false;

    projPos.w = 1.0f / projPos.w;

    projPos.x *= projPos.w;
    projPos.y *= projPos.w;
    projPos.z *= projPos.w;

    // window coordinates, Map x, y to range [0..1]

    screenCoord.x = (projPos.x * 0.5f + 0.5f) * viewportSize.x + viewportPos.x;
    screenCoord.y = (projPos.y * 0.5f + 0.5f) * viewportSize.y + viewportPos.y;

    // (/!\) this is only correct when glDepthRange(0.0, 1.0)
    screenCoord.z = (1.0f + projPos.z) * 0.5f; // range [0..1]

    return true;
}
