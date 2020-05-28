
#include "sceneToScreen.hpp"

bool sceneToScreen(const glm::vec3& obj,
                   const glm::mat4& modelview2, const glm::mat4& projection2,
                   const glm::vec2& viewportPos, const glm::vec2& viewportSize,
                   glm::vec3& screenCoord)
{
    const float* modelview = glm::value_ptr(modelview2);
    const float* projection = glm::value_ptr(projection2);

    // Transformation vectors
    float fTempo[8];

    // Modelview transform
    fTempo[0] = modelview[0] * obj.x + modelview[4] * obj.y + modelview[8] * obj.z + modelview[12]; // w is always 1
    fTempo[1] = modelview[1] * obj.x + modelview[5] * obj.y + modelview[9] * obj.z + modelview[13];
    fTempo[2] = modelview[2] * obj.x + modelview[6] * obj.y + modelview[10] * obj.z + modelview[14];
    fTempo[3] = modelview[3] * obj.x + modelview[7] * obj.y + modelview[11] * obj.z + modelview[15];

    // Projection transform, the final row of projection matrix is always [0 0 -1 0]
    // so we optimize for that.
    fTempo[4] = projection[0] * fTempo[0] + projection[4] * fTempo[1] + projection[8] * fTempo[2] + projection[12] * fTempo[3];
    fTempo[5] = projection[1] * fTempo[0] + projection[5] * fTempo[1] + projection[9] * fTempo[2] + projection[13] * fTempo[3];
    fTempo[6] = projection[2] * fTempo[0] + projection[6] * fTempo[1] + projection[10] * fTempo[2] + projection[14] * fTempo[3];
    fTempo[7] = -fTempo[2];

    // The result normalizes between -1 and 1
    if (fTempo[7] == 0.0f) // The w value
        return false;

    fTempo[7] = 1.0f / fTempo[7];

    // Perspective division
    fTempo[4] *= fTempo[7];
    fTempo[5] *= fTempo[7];
    fTempo[6] *= fTempo[7];

    // Window coordinates
    // Map x, y to range 0-1
    screenCoord[0] = (fTempo[4] * 0.5 + 0.5) * viewportSize.x + viewportPos.x;
    screenCoord[1] = (fTempo[5] * 0.5 + 0.5) * viewportSize.y + viewportPos.y;

    // This is only correct when glDepthRange(0.0, 1.0)
    screenCoord[2] = (1.0 + fTempo[6]) * 0.5; // Between 0 and 1

    return true;
}
