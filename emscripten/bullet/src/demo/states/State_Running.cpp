
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
                        bool isEnabled = data.sounds.manager.isEnabled();
                        data.sounds.manager.setEnabled(!isEnabled);
                    }
                }

            } // button sound on/off (hacky: to refactor)

        } // mouse/touch event(s)

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
        } // keyboard event(s)

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
        data.sounds.manager.setListener(camera.eye, camera.front, {0, 0, 1});
    }

    { // camera tracking

        glm::vec3   cameraNextCenter = logic.circuitAnimation.boundaries.center;
        float       cameraNextDistance = 200.0f;

        //
        //

        auto&   leaderCar = logic.leaderCar;

        if (logic.isAccelerated)
        {
            leaderCar.timeoutUntilNewLeader = 0.0f;
            leaderCar.index = -1;
            leaderCar.totalTimeAsLeader = 0.0f;
        }
        else
        {
            cameraNextDistance = 40.0f;

            if (leaderCar.timeoutUntilNewLeader > 0)
                leaderCar.timeoutUntilNewLeader -= elapsedTime;

            leaderCar.totalTimeAsLeader += elapsedTime;

            if (// no leader yet
                leaderCar.index == -1 ||
                // the current leader is dead
                simulation.getCarResult(leaderCar.index).isAlive == false ||
                // time to check for a potentially better leader
                leaderCar.timeoutUntilNewLeader <= 0.0f)
            {
                // reset the timeout
                leaderCar.timeoutUntilNewLeader = 1.0f; // <= one second

                // refresh the currently best car

                unsigned int totalCars = simulation.getTotalCars();

                float bestFitness = 0.0f;
                int oldLeaderCarIndex = leaderCar.index;
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

                if (leaderCar.index >= 0 && leaderCar.index != oldLeaderCarIndex)
                    leaderCar.totalTimeAsLeader = 0.0f; // new leader
            }
        }

        // do we have a car to focus the camera on?
        if (leaderCar.index >= 0)
        {
            const auto& carResult = simulation.getCarResult(leaderCar.index);

            // this part elevate where the camera look along the up axis of the car
            // => without it the camera look at the ground
            // => mostly useful for a shoulder camera (TODO)
            glm::vec4 carOrigin = carResult.transform * glm::vec4(0.0f, 0.0f, 2.0f, 1.0f);

            cameraNextCenter = glm::vec3(carOrigin);
        }

        //
        //

        {
            const float lerpRatio = 0.1f;

            camera.center += (cameraNextCenter - camera.center) * lerpRatio;
            camera.distance += (cameraNextDistance - camera.distance) * lerpRatio;
        }

    } // camera tracking

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
