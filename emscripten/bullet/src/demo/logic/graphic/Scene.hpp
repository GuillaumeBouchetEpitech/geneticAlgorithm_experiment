
#pragma once

#include "demo/helpers/GLMath.hpp"

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

    static void _renderLeadingCarSensors(const glm::mat4& sceneMatrix);
    static void _renderParticles(const glm::mat4& sceneMatrix);

    static void _renderFloor(const glm::mat4& sceneMatrix);

    static void _renderCars(const glm::mat4& sceneMatrix);
    // static void _renderCars(const Data::t_graphic::t_cameraData::t_matricesData::t_matrices& matrices);

    static void _renderWireframesGeometries(const glm::mat4& sceneMatrix, bool trails = true);
    static void _renderAnimatedCircuit(const glm::mat4& sceneMatrix);
    static void _renderHUD_ortho();
    static void _renderHUD_thirdPerson();
    static void _renderHUD();

};
