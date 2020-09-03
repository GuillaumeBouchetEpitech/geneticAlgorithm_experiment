
#include "State_StartGeneration.hpp"

#include "StateManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/logic/graphic/wrappers/Shader.hpp"
#include "demo/logic/graphic/Scene.hpp"

// #include "demo/utilities/TraceLogger.hpp"

#include "thirdparty/GLMath.hpp"

void State_StartGeneration::enter()
{
    // D_MYLOG("step");

    Data::get().logic.state.countdown = 750; // wait 0.75 seconds
}

void State_StartGeneration::leave()
{
    // D_MYLOG("step");
}

//

void State_StartGeneration::handleEvent(const SDL_Event& event)
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


void State_StartGeneration::update(int deltaTime)
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
                camera.rotations.theta += 2.0f * elapsedTime;
            else if (rotateRight)
                camera.rotations.theta -= 2.0f * elapsedTime;

            if (rotateUp)
                camera.rotations.phi -= 1.0f * elapsedTime;
            else if (rotateDown)
                camera.rotations.phi += 1.0f * elapsedTime;

#if not defined D_WEB_WEBWORKER_BUILD

            logic.isAccelerated = (keys[SDLK_SPACE]); // spacebar

#endif
        } // keyboard event(s)

    } // events

    {
        graphic.particleManager.update(elapsedTime);
    }

    {
        data.sounds.manager.setListener(camera.eye, camera.front, {0, 0, 1});
    }

    { // camera tracking

        glm::vec3   cameraNextCenter = logic.circuitAnimation.boundaries.center;
        float       cameraNextDistance = 200.0f;

        if (!logic.isAccelerated)
        {
            cameraNextDistance = 40.0f;

            const auto& carResult = simulation.getCarResult(0);

            // this part elevate where the camera look along the up axis of the car
            // => without it the camera look at the ground
            // => mostly useful for a shoulder camera
            glm::vec4 carOrigin = carResult.transform * glm::vec4(0.0f, 0.0f, 2.0f, 1.0f);

            cameraNextCenter = carOrigin;
        }

        //
        //

        const float lerpRatio = 0.1f;

        camera.center += (cameraNextCenter - camera.center) * lerpRatio;
        camera.distance += (cameraNextDistance - camera.distance) * lerpRatio;

    } // camera tracking

    Data::get().logic.state.countdown -= deltaTime;
    if (Data::get().logic.state.countdown <= 0)
        StateManager::get()->changeState(StateManager::States::eRunning);
}


void State_StartGeneration::render(const SDL_Window& window)
{
    static_cast<void>(window); // <= unused

    Scene::renderAll();
}

void State_StartGeneration::resize(int width, int height)
{
    Data::get().graphic.camera.viewportSize = { width, height };
}

void State_StartGeneration::visibility(bool visible)
{
    if (!visible)
    {
        Data::get().logic.state.previousState = StateManager::States::eStartGeneration;
        StateManager::get()->changeState(StateManager::States::ePaused);
    }
}
