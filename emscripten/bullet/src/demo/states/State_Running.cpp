
#include "demo/defines.hpp"

#include "State_Running.hpp"

#include "StateManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/logic/graphic/wrappers/Shader.hpp"
#include "demo/logic/graphic/Scene.hpp"

// #include "demo/utilities/TraceLogger.hpp"

#include "thirdparty/GLMath.hpp"

#include <limits> // std::numeric_limits<T>::max();
#include <cmath> // std::ceil

void State_Running::enter()
{
    // D_MYLOG("step");
}

void State_Running::leave()
{
    // D_MYLOG("step");
}

//

void State_Running::handleEvent(const SDL_Event& event)
{
    auto& data = Data::get();
    auto& keys = data.inputs.keys;
    auto& mouse = data.inputs.mouse;

    switch (event.type)
    {
        case SDL_KEYDOWN:
        {
            keys[event.key.keysym.sym] = true;
            break;
        }
        case SDL_KEYUP:
        {
            keys[event.key.keysym.sym] = false;
            break;
        }

        case SDL_MOUSEBUTTONDOWN:
        {
            mouse.position = { event.motion.x, event.motion.y };
            mouse.delta = { 0, 0 };
            mouse.tracking = true;
            mouse.wasTracking = false;
            break;
        }
        case SDL_MOUSEBUTTONUP:
        {
            mouse.tracking = false;
            mouse.wasTracking = true;
            break;
        }
        case SDL_MOUSEMOTION:
        {
            if (mouse.tracking)
            {
                // mouse/touch events are 4 times more sentitive in web build
                // this bit ensure the mouse/touch are even in native/web build
#if defined D_WEB_BUILD
                int coef = 1;
#else
                int coef = 4;
#endif

                glm::ivec2 newPosition = { event.motion.x, event.motion.y };
                mouse.delta = (newPosition - mouse.position) * coef;
                mouse.position = newPosition;
            }
            break;
        }

        default:
            break;
    }
}

void State_Running::update(int deltaTime)
{
    float elapsedTime = float(deltaTime) / 1000.0f;

    auto& data = Data::get();
    auto& logic = data.logic;
    auto& simulation = *logic.simulation;
    auto& graphic = data.graphic;
    auto& camera = graphic.camera;

    { // events

        { // mouse/touch event(s)

            auto& mouse = data.inputs.mouse;

            if (mouse.tracking)
            {
                // camera.rotations.x -= float(mouse.delta.x) * 0.5f * elapsedTime;
                // camera.rotations.y += float(mouse.delta.y) * 0.5f * elapsedTime;
                camera.rotations.theta -= float(mouse.delta.x) * 0.5f * elapsedTime;
                camera.rotations.phi += float(mouse.delta.y) * 0.5f * elapsedTime;
                mouse.delta = { 0, 0 };
            }

            { // button sound on/off (hacky: to refactor)

                if (!mouse.tracking && mouse.wasTracking)
                {
                    mouse.wasTracking = false;

                    const auto& hudText = graphic.hudText;
                    const glm::vec2 letterSize = hudText.textureSize / hudText.gridSize;

                    const glm::vec2 center(720, 125);
                    const glm::vec2 size(10.0f * letterSize.x, 4.0f * letterSize.y);
                    const glm::vec2 hsize = size * 0.5f;

                    if (mouse.position.x > center.x - hsize.x &&
                        mouse.position.x < center.x + hsize.x &&
                        mouse.position.y > center.y - hsize.y &&
                        mouse.position.y < center.y + hsize.y)
                    {
                        bool isEnabled = data.sounds.soundManager.isEnabled();
                        data.sounds.soundManager.setEnabled(!isEnabled);
                    }
                }

            } // button sound on/off (hacky: to refactor)

        }

        { // keyboard event(s)

            auto& keys = data.inputs.keys;

            bool rotateLeft = (
                keys[SDLK_LEFT] ||  // ARROW
                keys[SDLK_q] ||     // QWERTY (UK-US keyboard layout)
                keys[SDLK_a]        // AZERTY (FR keyboard layout)
            );

            bool rotateRight = (
                keys[SDLK_RIGHT] || // ARROW
                keys[SDLK_d]
            );

            bool rotateUp = (
                keys[SDLK_UP] ||    // ARROW
                keys[SDLK_w] ||     // QWERTY (UK-US keyboard layout)
                keys[SDLK_z]        // AZERTY (FR keyboard layout)
            );

            bool rotateDown = (
                keys[SDLK_DOWN] ||  // ARROW
                keys[SDLK_s]
            );

            // if (rotateLeft)
            //     camera.rotations.x -= 2.0f * elapsedTime;
            // else if (rotateRight)
            //     camera.rotations.x += 2.0f * elapsedTime;

            // if (rotateUp)
            //     camera.rotations.y += 1.0f * elapsedTime;
            // else if (rotateDown)
            //     camera.rotations.y -= 1.0f * elapsedTime;

            if (rotateLeft)
                camera.rotations.theta -= 2.0f * elapsedTime;
            else if (rotateRight)
                camera.rotations.theta += 2.0f * elapsedTime;

            if (rotateUp)
                camera.rotations.phi += 1.0f * elapsedTime;
            else if (rotateDown)
                camera.rotations.phi -= 1.0f * elapsedTime;

#if not defined D_WEB_WEBWORKER_BUILD

            // only pthread builds support this
            logic.isAccelerated = (keys[SDLK_SPACE]); // spacebar

#endif
        }

    } // events

#if not defined D_WEB_WEBWORKER_BUILD
    {
        int simualtionStep = (logic.isAccelerated ? 50 : 1);

        for (unsigned int ii = 0; ii < simulation.getTotalCores(); ++ii)
            logic.cores.statesData[ii].delta = 0;

        for (int ii = 0; ii < simualtionStep; ++ii)
            simulation.update();

    }
#endif

    {
        graphic.particleManager.update(elapsedTime);
    }

    {
        data.sounds.soundManager.setListener(camera.eye, camera.front, {0, 0, 1});
    }

    { // camera tracking

        glm::vec3   cameraNextCenter = logic.circuitAnimation.boundaries.center;
        float       cameraNextDistance = 200.0f;

        //
        //

        auto&   leaderCar = logic.leaderCar;

        if (logic.isAccelerated)
        {
            leaderCar.timeout = 0.0f;
            leaderCar.index = -1;
        }
        else
        {
            cameraNextDistance = 40.0f;

            if (leaderCar.timeout > 0)
                leaderCar.timeout -= elapsedTime;

            if (// no leader yet
                leaderCar.index == -1 ||
                // the current leader is dead
                simulation.getCarResult(leaderCar.index).isAlive == false ||
                // time to check for a potentially better leader
                leaderCar.timeout <= 0.0f)
            {
                // reset the timeout
                leaderCar.timeout = 1.0f; // <= one second

                // refresh the currently best car

                unsigned int totalCars = simulation.getTotalCars();

                float bestFitness = 0.0f;
                leaderCar.index = -1;
                for (unsigned int ii = 0; ii < totalCars; ++ii)
                {
                    const auto& carData = simulation.getCarResult(ii);

                    if (!carData.isAlive)
                        continue;

                    if (leaderCar.index != -1 &&
                        !(carData.fitness > bestFitness + 2.0f))
                        continue;

                    bestFitness = carData.groundIndex;
                    leaderCar.index = ii;
                }
            }

            { // frustum dybamic camera distance

                // unsigned int carsAlive = 0;

                // const unsigned int totalCars = simulation.getTotalCars();
                // for (unsigned int ii = 0; ii < totalCars; ++ii)
                // {
                //     const auto& carData = simulation.getCarResult(ii);

                //     if (!carData.isAlive)
                //         continue;

                //     //

                //     ++carsAlive;
                // }

                // // D_MYLOG("carsAlive=" << carsAlive);

                // if (carsAlive > 0)
                // {
                //     const float maxDistance = 50.0f;
                //     const float minDistance = 30.0f;

                //     unsigned int bestCarsVisible = 0;
                //     float bestDistance = minDistance;

                //     for (float currDistance = minDistance; currDistance < maxDistance; currDistance += 10.0f)
                //     {
                //         {
                //             glm::mat4 projection;
                //             glm::mat4 modelMatrix;

                //             const float fovy = glm::radians(70.0f);
                //             const float aspectRatio = float(camera.viewportSize.x) / camera.viewportSize.y;

                //             projection = glm::perspective(fovy, aspectRatio, 1.0f, 1000.f);
                //             glm::mat4 viewMatrix = glm::mat4(1.0f); // <= identity matrix
                //             modelMatrix = glm::mat4(1.0f); // <= identity matrix

                //             // clamp vertical rotation [0..PI]
                //             camera.rotations.y = std::max(0.0f, std::min(3.14f, camera.rotations.y));

                //             viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -currDistance));
                //             viewMatrix = glm::rotate(viewMatrix, camera.rotations.y, glm::vec3(-1.0f, 0.0f, 0.0f));
                //             viewMatrix = glm::rotate(viewMatrix, camera.rotations.x, glm::vec3(0.0f, 0.0f, 1.0f));

                //             modelMatrix = glm::translate(modelMatrix, camera.center);

                //             modelMatrix = viewMatrix * modelMatrix;

                //             camera.frustumCulling.calculateFrustum(projection, modelMatrix);
                //         }

                //         unsigned int carsVisible = 0;
                //         for (unsigned int ii = 0; ii < totalCars; ++ii)
                //         {
                //             const auto& carData = simulation.getCarResult(ii);

                //             if (!carData.isAlive)
                //                 continue;

                //             //

                //             glm::vec3 sensorsHeight(0.0f, 0.0f, 2.0f);
                //             glm::mat4 carTransform = glm::translate(carData.transform, sensorsHeight);
                //             glm::vec4 carOrigin = carTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

                //             if (graphic.camera.frustumCulling.sphereInFrustum(carOrigin, 1.0f))
                //                 ++carsVisible;
                //         }

                //         if (bestCarsVisible < carsVisible)
                //         {
                //             bestCarsVisible = carsVisible;
                //             bestDistance = currDistance;

                //             // D_MYLOG("=> bestCarsVisible=" << bestCarsVisible);
                //         }
                //     }

                //     cameraNextDistance = bestDistance;
                // }

            } // frustum dybamic camera distance


        }

        // do we have a car to focus the camera on?
        if (leaderCar.index >= 0)
        {
            const auto& carResult = simulation.getCarResult(leaderCar.index);

            // this part elevate where the camera look along the up axis of the car
            // => without it the camera look at the ground
            // => mostly useful for a shoulder camera (TODO)
            // glm::vec4 carOrigin = carResult.transform * glm::vec4(0.0f, 0.0f, 2.0f, -1.0f);
            glm::vec4 carOrigin = carResult.transform * glm::vec4(0.0f, 0.0f, 2.0f, 1.0f);

            cameraNextCenter = glm::vec3(carOrigin);

            // D_MYLOG("pos: " << cameraNextCenter.x << "/" << cameraNextCenter.y << "/" << cameraNextCenter.z);
        }

        //
        //

        {
            const float lerpRatio = 0.1f;

            camera.center += (cameraNextCenter - camera.center) * lerpRatio;
            camera.distance += (cameraNextDistance - camera.distance) * lerpRatio;
        }

    } // camera tracking

    { // circuit animation

        // auto&    leaderCar = logic.leaderCar;
        auto&   animation = logic.circuitAnimation;

        if (logic.isAccelerated)
        {
            animation.targetValue = animation.maxUpperValue;
            animation.lowerValue = animation.maxUpperValue;
            animation.upperValue = animation.maxUpperValue;
        }
        else
        {
            animation.targetValue = 3.0f; // <= default value

            int bestGroundIndex = -1;
            for (unsigned int ii = 0; ii < simulation.getTotalCars(); ++ii)
            {
                const auto& carData = simulation.getCarResult(ii);

                if (!carData.isAlive || bestGroundIndex > carData.groundIndex)
                    continue;

                bestGroundIndex = carData.groundIndex;
            }

            // do we have a car to focus the camera on?
            if (bestGroundIndex >= 0)
                animation.targetValue += bestGroundIndex;

            // lower value, closest from the cars

            if (animation.lowerValue > animation.targetValue + 10.0f)
            {
                // fall really quickly
                animation.lowerValue -= 1.0f;
                if (animation.lowerValue < animation.targetValue)
                    animation.lowerValue = animation.targetValue;
            }
            else if (animation.lowerValue > animation.targetValue)
            {
                // fall quickly
                animation.lowerValue -= 0.3f;
                if (animation.lowerValue < animation.targetValue)
                    animation.lowerValue = animation.targetValue;
            }
            else
            {
                // rise slowly
                animation.lowerValue += 0.2f;
                if (animation.lowerValue > animation.targetValue)
                    animation.lowerValue = animation.targetValue;
            }

            // upper value, farthest from the cars

            if (animation.upperValue > animation.targetValue + 10.0f)
            {
                // fall really quickly
                animation.upperValue -= 0.6f;
                if (animation.upperValue < animation.targetValue)
                    animation.upperValue = animation.targetValue;
            }
            else if (animation.upperValue > animation.targetValue)
            {
                // fall slowly
                animation.upperValue -= 0.1f;
                if (animation.upperValue < animation.targetValue)
                    animation.upperValue = animation.targetValue;
            }
            else
            {
                // rise really quickly
                animation.upperValue += 1.0f;
                if (animation.upperValue > animation.targetValue)
                    animation.upperValue = animation.targetValue;
            }

        }

        auto& animatedCircuit = graphic.geometries.animatedCircuit;

        const unsigned int verticesLength = 36; // <= 3 * 12 triangles
        int indexValue = std::ceil(animation.upperValue) * verticesLength;
        if (indexValue > animation.maxPrimitiveCount)
            indexValue = animation.maxPrimitiveCount;

        animatedCircuit.ground.setPrimitiveCount(indexValue);
        animatedCircuit.walls.setPrimitiveCount(indexValue * 2); // <= 2 walls

    } // circuit animation

#if defined D_WEB_WEBWORKER_BUILD

    simulation.update();

#endif
}


void State_Running::render(const SDL_Window& window)
{
    static_cast<void>(window); // <= unused

    Scene::renderAll();
}

void State_Running::resize(int width, int height)
{
    Data::get().graphic.camera.viewportSize = { width, height };
}

void State_Running::visibility(bool visible)
{
    if (!visible)
    {
        Data::get().logic.state.previousState = StateManager::States::eRunning;
        StateManager::get()->changeState(StateManager::States::ePaused);
    }
}
