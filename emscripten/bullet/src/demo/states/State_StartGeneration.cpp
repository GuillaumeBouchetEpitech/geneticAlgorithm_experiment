
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

    // Data::get()->logic.isPaused = true;

    Data::get()->logic.state.countdown = 750;
}

void State_StartGeneration::leave()
{
    // D_MYLOG("step");

    // Data::get()->logic.isPaused = false;
}

//

void State_StartGeneration::handleEvent(const SDL_Event& event)
{
    auto&   data = *Data::get();
    auto&   keys = data.inputs.keys;
    auto&   mouse = data.inputs.mouse;

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
    // static_cast<void>(delta); // <= unused


    float elapsedTime = float(deltaTime) / 1000.0f;

    auto&   data = *Data::get();
    auto&   logic = data.logic;
    auto&   simulation = *logic.simulation;
    auto&   graphic = data.graphic;
    auto&   camera = graphic.camera;

    { // events

        { // mouse/touch event(s)

            auto& mouse = data.inputs.mouse;

            if (mouse.tracking)
            {
                camera.rotations.x += float(mouse.delta.x) * 0.5f * elapsedTime;
                camera.rotations.y -= float(mouse.delta.y) * 0.5f * elapsedTime;
                mouse.delta = { 0, 0 };
            }
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

            if (rotateLeft)
                camera.rotations.x -= 2.0f * elapsedTime;
            else if (rotateRight)
                camera.rotations.x += 2.0f * elapsedTime;

            if (rotateUp)
                camera.rotations.y += 1.0f * elapsedTime;
            else if (rotateDown)
                camera.rotations.y -= 1.0f * elapsedTime;

#if not defined D_WEB_WEBWORKER_BUILD

            logic.isAccelerated = (keys[SDLK_SPACE]); // spacebar

#endif
        }

    } // events

    {
        graphic.particleManager.update(elapsedTime);
    }

    { // camera tracking

        glm::vec3   cameraNextCenter = logic.circuitAnimation.boundaries.center;
        float       cameraNextDistance = 200.0f;

        if (!logic.isAccelerated)
        {
            cameraNextDistance = 40.0f;

            // const auto& carResult = simulation.getCarResult(leaderCar.index);
            const auto& carResult = simulation.getCarResult(0);

            // this part elevate where the camera look along the up axis of the car
            // => without it the camera look at the ground
            // => mostly useful for a shoulder camera (TODO)
            glm::vec4   carOrigin = carResult.transform * glm::vec4(0.0f, 0.0f, 2.0f, -1.0f);

            cameraNextCenter = glm::vec3(carOrigin);
        }

        //
        //

        const float lerpRatio = 0.1f;

        camera.center += (cameraNextCenter - camera.center) * lerpRatio;
        camera.distance += (cameraNextDistance - camera.distance) * lerpRatio;

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

    Data::get()->logic.state.countdown -= deltaTime;
    if (Data::get()->logic.state.countdown <= 0)
        StateManager::get()->changeState(StateManager::States::eRunning);
}


void State_StartGeneration::render(const SDL_Window& window)
{
    static_cast<void>(window); // <= unused

    Scene::renderAll();
}

void State_StartGeneration::resize(int width, int height)
{
    Data::get()->graphic.camera.viewportSize = { width, height };
}

void State_StartGeneration::visibility(bool visible)
{
    // static_cast<void>(visible); // <= unused

    if (!visible)
    {
        Data::get()->logic.state.previousState = StateManager::States::eStartGeneration;
        StateManager::get()->changeState(StateManager::States::ePaused);
    }
}
