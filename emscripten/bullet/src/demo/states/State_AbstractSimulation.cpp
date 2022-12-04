
#include "demo/defines.hpp"

#include "State_AbstractSimulation.hpp"

#include "StateManager.hpp"

#include "demo/logic/Context.hpp"
#include "demo/logic/graphic/Scene.hpp"

#include "framework/helpers/GLMath.hpp"

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
    auto& context = Context::get();
    auto& keys = context.inputs.keys;
    auto& mouse = context.inputs.mouse;

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
            break;
        }
        case SDL_MOUSEBUTTONUP:
        {
            mouse.tracking = false;
            break;
        }
        case SDL_MOUSEMOTION:
        {
            if (mouse.tracking)
            {
                // mouse/touch events are 4 times more sentitive in web build
                // this bit ensure the mouse/touch are even in native/web build
#if defined D_WEB_BUILD
                constexpr float coef = 1;
#else
                constexpr float coef = 4;
#endif

                const glm::vec2 newPosition = { event.motion.x, event.motion.y };
                mouse.delta = (newPosition - glm::vec2(mouse.position)) * coef;
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

    auto& context = Context::get();
    auto& graphic = context.graphic;
    auto& camera = graphic.camera;

    { // events

        auto& rotations = camera.main.rotations;

        { // mouse/touch event(s)

            auto& mouse = context.inputs.mouse;

            if (mouse.tracking)
            {
                rotations.theta -= float(mouse.delta.x) * 1.0f * elapsedTime;
                rotations.phi += float(mouse.delta.y) * 1.0f * elapsedTime;
                mouse.delta = { 0, 0 };
            }

        } // mouse/touch event(s)

        { // keyboard event(s)

            auto& keys = context.inputs.keys;

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
                rotations.theta += 2.0f * elapsedTime;
            else if (rotateRight)
                rotations.theta -= 2.0f * elapsedTime;

            if (rotateUp)
                rotations.phi -= 1.0f * elapsedTime;
            else if (rotateDown)
                rotations.phi += 1.0f * elapsedTime;

            context.logic.isAccelerated = (keys[SDLK_SPACE]); // spacebar

        } // keyboard event(s)

    } // events
}


void State_AbstractSimulation::render(const SDL_Window& window)
{
    static_cast<void>(window); // <= unused

    Scene::renderAll();
}

void State_AbstractSimulation::resize(int width, int height)
{
    auto& graphic = Context::get().graphic;

    graphic.camera.viewportSize = { width, height };

    graphic.postProcess.resize({ width, height });
}

void State_AbstractSimulation::visibility(bool visible)
{
#if 0 // disable pause state?
    static_cast<void>(visible); // unused
#else
    auto* stateManager = StateManager::get();
    StateManager::States currentState = stateManager->getState();

    if (currentState != StateManager::States::Paused && !visible)
        stateManager->changeState(StateManager::States::Paused);
#endif
}
