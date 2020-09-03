
#include "sceneToScreen.hpp"

bool sceneToScreen(const glm::vec3& obj,
                   const glm::mat4& modl, const glm::mat4& proj,
                   const glm::vec2& viewportPos, const glm::vec2& viewportSize,
                   glm::vec3& screenCoord)
{
    // modelview transform

    glm::vec4 modPos = {
        modl[0].x * obj.x + modl[1].x * obj.y + modl[2].x * obj.z + modl[3].x, // obj.w is always 1
        modl[0].y * obj.x + modl[1].y * obj.y + modl[2].y * obj.z + modl[3].y,
        modl[0].z * obj.x + modl[1].z * obj.y + modl[2].z * obj.z + modl[3].z,
        modl[0].w * obj.x + modl[1].w * obj.y + modl[2].w * obj.z + modl[3].w,
    };

    // projection transform, the final row of projection matrix is always [0 0 -1 0] so we optimize for that.

    glm::vec4 projPos = {
        proj[0].x * modPos.x + proj[1].x * modPos.y + proj[2].x * modPos.z + proj[3].x * modPos.w,
        proj[0].y * modPos.x + proj[1].y * modPos.y + proj[2].y * modPos.z + proj[3].y * modPos.w,
        proj[0].z * modPos.x + proj[1].z * modPos.y + proj[2].z * modPos.z + proj[3].z * modPos.w,
        -modPos.z,
    };

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
