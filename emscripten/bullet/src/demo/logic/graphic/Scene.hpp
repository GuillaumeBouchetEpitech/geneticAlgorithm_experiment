
#pragma once

// #include "demo/logic/Data.hpp"

#include "thirdparty/GLMath.hpp"

class Scene
{
public:
    static void initialise();
    static void renderSimple();
    static void renderAll();

private:
    static void _updateMatrices();
    static void _updateCircuitAnimation();

    static void _clear();

    static void _renderLeadingCarSensors(const glm::mat4& sceneMatrix);
    static void _renderParticles(const glm::mat4& sceneMatrix);

    static void _renderCars(const glm::mat4& sceneMatrix);
    // static void _renderCars(const Data::t_graphic::t_cameraData::t_matricesData::t_matrices& matrices);

    static void _renderWireframesGeometries(const glm::mat4& sceneMatrix, bool trails = true);
    static void _renderAnimatedCircuit(const glm::mat4& sceneMatrix);
    static void _renderHUD_ortho();
    static void _renderHUD_thirdPerson();
    static void _renderHUD();

};
