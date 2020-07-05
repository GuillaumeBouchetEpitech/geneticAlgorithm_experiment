
#pragma once

#include "thirdparty/GLMath.hpp"

class Scene
{
public:
    static void renderSimple();
    static void renderAll();

private:
    static void updateMatrices();

    static void clear();

    static void renderLeadingCarSensors(const glm::mat4& sceneMatrix);
    static void renderParticles(const glm::mat4& sceneMatrix);
    static void renderCars(const glm::mat4& sceneMatrix);
    static void renderWireframesGeometries(const glm::mat4& sceneMatrix, bool trails = true);
    static void renderAnimatedCircuit(const glm::mat4& sceneMatrix);
    static void renderHUD();

};
