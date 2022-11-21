
#include "State_StartGeneration.hpp"

#include "StateManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/logic/graphic/Scene.hpp"

#include "framework/helpers/GLMath.hpp"

void State_StartGeneration::enter()
{
    auto& data = Data::get();
    auto& logic = data.logic;

    const float currFitness = logic.fitnessStats.allStats.back();

    if (currFitness > 0.0f)
        _countdown = 2000; // wait 2.0 second(s)
    else
        _countdown = 1000; // wait 1.0 second(s)
}

void State_StartGeneration::update(int deltaTime)
{
    State_AbstractSimulation::update(deltaTime);

    auto& data = Data::get();
    auto& logic = data.logic;
    auto& graphic = data.graphic;

    float elapsedTime = float(deltaTime) / 1000.0f;

    Scene::updateMatrices(elapsedTime);

    {
        graphic.particleManager.update(elapsedTime);
        graphic.backGroundCylindersRenderer.update(elapsedTime);
        graphic.animatedCircuitRenderer.update(elapsedTime);
        graphic.flockingManager.update();
    }

    { // camera tracking

        auto& simulation = *logic.simulation;
        auto& camera = graphic.camera;

        glm::vec3   cameraNextCenter = camera.scene.center;
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

        camera.scene.center += (cameraNextCenter - camera.scene.center) * lerpRatio;
        camera.scene.distance += (cameraNextDistance - camera.scene.distance) * lerpRatio;

    } // camera tracking

    _countdown -= deltaTime;
    if (_countdown <= 0)
        StateManager::get()->changeState(StateManager::States::Running);
}
