
#pragma once

#include "demo/logic/Data.hpp"

#include "helpers/GLMath.hpp"

class Scene
{
public:
    static void initialise();
    static void renderSimple();
    static void renderAll();

    static void updateMatrices(float elapsedTime);
    static void updateCircuitAnimation(float elapsedTime);

private:
    static void _clear();

    static void _renderLeadingCarSensors(const Camera& camera);
    static void _renderParticles(const Camera& camera);
    static void _renderFloor(const Camera& camera);
    static void _renderCars(const Camera& camera);

    static void _renderWireframesGeometries(const glm::mat4& sceneMatrix, bool trails = true);
    static void _renderAnimatedCircuit(const glm::mat4& sceneMatrix);

    static void _renderHUD_ortho();
    static void _renderHUD_thirdPerson();
    static void _renderHUD();

};
