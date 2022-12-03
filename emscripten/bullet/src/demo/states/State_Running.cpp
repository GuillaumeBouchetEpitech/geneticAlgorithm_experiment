
#include "demo/defines.hpp"

#include "State_Running.hpp"

#include "StateManager.hpp"

#include "demo/logic/Data.hpp"

#include "demo/logic/graphic/Scene.hpp"

#include "framework/helpers/GLMath.hpp"

#include <limits> // std::numeric_limits<T>::max();
#include <cmath> // std::ceil

void State_Running::update(int deltaTime)
{
    State_AbstractSimulation::update(deltaTime);

    float elapsedTime = float(deltaTime) / 1000.0f;

    auto& data = Data::get();
    auto& logic = data.logic;
    auto& simulation = *logic.simulation;
    auto& graphic = data.graphic;
    auto& camera = graphic.camera;

    { // simulation update

        if (!simulation.isGenerationComplete())
        {
            constexpr float fakeElapsedTime = 1.0f / 30.0f; // TODO: hardcoded
            const unsigned int totalSteps = (logic.isAccelerated ? 50 : 1);

            simulation.update(fakeElapsedTime, totalSteps);
        }
        else
        {
            simulation.breed();
        }

    } // simulation update


    Scene::updateMatrices(elapsedTime);

    { // camera tracking

        // done to avoid a spurious change of camera
        // -> true when changing states: Running -> EndGeneration
        if (StateManager::get()->getState() == StateManager::States::Running)
        {
            glm::vec3   cameraNextCenter = logic.circuitDimension.center;
            float       cameraNextDistance = 200.0f;

            //
            //

            auto& leaderCar = logic.leaderCar;

            if (logic.isAccelerated)
            {
                leaderCar.reset();
            }
            else
            {
                cameraNextDistance = 40.0f;

                leaderCar.update(elapsedTime);

                if (auto leaderPos = leaderCar.leaderPosition())
                    cameraNextCenter = *leaderPos;
            }

            //
            //

            {
                const float lerpRatio = 6.0f * elapsedTime;

                camera.scene.center += (cameraNextCenter - camera.scene.center) * lerpRatio;
                camera.scene.distance += (cameraNextDistance - camera.scene.distance) * lerpRatio;
            }
        }

    } // camera tracking


    {
        graphic.particleManager.update(elapsedTime);
        graphic.backGroundCylindersRenderer.update(elapsedTime);
        graphic.animatedCircuitRenderer.update(elapsedTime);
        graphic.flockingManager.update();
        graphic.postProcess.update(elapsedTime);
    }
}
