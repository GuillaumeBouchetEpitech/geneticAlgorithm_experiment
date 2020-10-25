
#include "demo/defines.hpp"

#include "State_AbstractSimulation.hpp"

#include "StateManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/logic/graphic/Scene.hpp"

#include "thirdparty/GLMath.hpp"

#include <limits> // std::numeric_limits<T>::max();
#include <cmath> // std::ceil

void State_AbstractSimulation::enter()
{
    // D_MYLOG("step");
}

void State_AbstractSimulation::leave()
{
    // D_MYLOG("step");
}

//

void State_AbstractSimulation::handleEvent(const SDL_Event& event)
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

                glm::ivec2 newPosition(event.motion.x, event.motion.y);
                mouse.delta = (newPosition - mouse.position) * coef;
                mouse.position = newPosition;
            }
            break;
        }

        default:
            break;
    }
}

void State_AbstractSimulation::update(int deltaTime)
{
    float elapsedTime = float(deltaTime) / 1000.0f;

    auto& data = Data::get();
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

            // only pthread builds support this
            data.logic.isAccelerated = (keys[SDLK_SPACE]); // spacebar

#endif
        } // keyboard event(s)

    } // events

    {
        graphic.particleManager.update(elapsedTime);
    }

    {
        data.sounds.manager.setListener(camera.eye, camera.front, {0, 0, 1});
    }
}


void State_AbstractSimulation::render(const SDL_Window& window)
{
    static_cast<void>(window); // <= unused

    Scene::renderAll();
}

void State_AbstractSimulation::resize(int width, int height)
{
    Data::get().graphic.camera.viewportSize = { width, height };
}

void State_AbstractSimulation::visibility(bool visible)
{
    if (!visible)
    {
        Data::get().logic.state.previousState = StateManager::get()->getState();
        StateManager::get()->changeState(StateManager::States::Paused);
    }
}
