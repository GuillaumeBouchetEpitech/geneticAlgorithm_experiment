
#include "State_StartGeneration.hpp"

#include "StateManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/logic/graphic/wrappers/Shader.hpp"
#include "demo/logic/graphic/Scene.hpp"

#include "thirdparty/GLMath.hpp"

void State_StartGeneration::enter()
{
    Data::get().logic.state.countdown = 750; // wait 0.75 seconds
}

void State_StartGeneration::update(int deltaTime)
{
    State_AbstractSimulation::update(deltaTime);

    auto& data = Data::get();
    auto& logic = data.logic;
    auto& simulation = *logic.simulation;
    auto& graphic = data.graphic;
    auto& camera = graphic.camera;

    { // camera tracking

        glm::vec3   cameraNextCenter = camera.center;
        float       cameraNextDistance = 200.0f;

        if (logic.isAccelerated)
        {
            cameraNextCenter = logic.circuitAnimation.boundaries.center;
        }
        else
        {
            cameraNextDistance = 40.0f;
            cameraNextCenter = simulation.getStartPosition();
        }

        //
        //

        const float lerpRatio = 0.1f;

        camera.center += (cameraNextCenter - camera.center) * lerpRatio;
        camera.distance += (cameraNextDistance - camera.distance) * lerpRatio;

    } // camera tracking

    logic.state.countdown -= deltaTime;
    if (logic.state.countdown <= 0)
        StateManager::get()->changeState(StateManager::States::Running);
}
