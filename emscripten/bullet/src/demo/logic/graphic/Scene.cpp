
#include "Scene.hpp"

#include "demo/states/StateManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/logic/graphic/wrappers/ShaderProgram.hpp"

#include "demo/utilities/TraceLogger.hpp"

#include "./utilities/sceneToScreen.hpp"

// #include <memory> // <= make_unique
#include <cstring> // <= std::memcpy

#include <iomanip>
#include <sstream>

namespace /*anonymous*/
{

std::string writeTime(unsigned int time)
{
    std::stringstream sstr;
    sstr << std::setw(5) << std::fixed << std::setprecision(1);

    // if (time < 1000)
    // {
    //     sstr << time << "us";
    // }
    // else
    if (time < 1000000)
    {
        sstr << (float(time) / 1000) << "ms";
    }
    else
    {
        sstr << (float(time) / 1000000) << "s";
    }

    return sstr.str();
}

}

void Scene::initialise()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDisable(GL_CULL_FACE);
    // glEnable(GL_CULL_FACE);
}

void Scene::renderSimple()
{
    Scene::_clear();

    { // scene

        const auto& scene = Data::get().graphic.camera.matrices.scene;

        Scene::_renderFloor(scene.composed);

        Scene::_renderWireframesGeometries(scene.composed, false); // circuit only
    }

    { // HUD

        Scene::_renderHUD();
    }

    ShaderProgram::unbind();
}

void Scene::renderAll()
{
    Scene::_clear();

    { // scene

        auto& data = Data::get();
        auto& logic = data.logic;
        auto& camera = data.graphic.camera;
        const auto& matrices = camera.matrices;

        // camera.frustumCulling.calculateFrustum(matrices.scene.projection, matrices.scene.modelView);
        glm::mat4 modelView = matrices.scene.view * matrices.scene.model;
        camera.frustumCulling.calculateFrustum(matrices.scene.projection, modelView);

        if (!logic.isAccelerated)
            Scene::_renderLeadingCarSensors(matrices.scene.composed);

        data.graphic.flockingManager.render(matrices.scene.composed);

        Scene::_renderParticles(matrices.scene.composed);

        Scene::_renderFloor(matrices.scene.composed);

        Scene::_renderCars(matrices.scene.composed);

        Scene::_renderWireframesGeometries(matrices.scene.composed);
        Scene::_renderAnimatedCircuit(matrices.scene.composed);
    }

    { // HUD

        Scene::_renderHUD();
    }

    ShaderProgram::unbind();
}

void Scene::updateMatrices(float elapsedTime)
{
    auto& data = Data::get();
    const auto& logic = data.logic;
    const auto& simulation = *logic.simulation;
    auto& graphic = data.graphic;
    auto& camera = graphic.camera;
    auto& matrices = camera.matrices;

    { // scene

        const float fovy = glm::radians(70.0f);
        const float aspectRatio = float(camera.viewportSize.x) / camera.viewportSize.y;

        matrices.scene.projection = glm::perspective(fovy, aspectRatio, 1.0f, 1000.f);

        // clamp vertical rotation [-45..+45]
        const float verticalLimit = glm::radians(45.0f);
        camera.rotations.phi = glm::clamp(camera.rotations.phi, -verticalLimit, verticalLimit);

        camera.eye = {
            camera.distance * std::cos(camera.rotations.phi) * std::cos(camera.rotations.theta),
            camera.distance * std::cos(camera.rotations.phi) * std::sin(camera.rotations.theta),
            camera.distance * std::sin(camera.rotations.phi),
        };
        glm::vec3 center = { 0.0f, 0.0f, 0.0f };
        glm::vec3 upAxis = { 0.0f, 0.0f, 1.0f };
        glm::mat4 viewMatrix = glm::lookAt(camera.eye, center, upAxis);

        camera.front = glm::normalize(camera.eye - center);

        glm::mat4 modelMatrix = glm::mat4(1.0f); // <= identity matrix
        modelMatrix = glm::translate(modelMatrix, -camera.center);

        matrices.scene.model = modelMatrix;
        matrices.scene.view = viewMatrix;

        matrices.scene.composed = matrices.scene.projection * matrices.scene.view * matrices.scene.model;

    } // scene

    { // third person

        if (logic.leaderCar.index >= 0)
        {
            matrices.thirdPerson.projection = matrices.scene.projection;

            const auto& carResult = simulation.getCarResult(logic.leaderCar.index);

            glm::vec3 carOrigin = carResult.transforms.chassis * glm::vec4(0.0f, 0.0f, 2.5f, 1.0f);
            glm::vec3 carUpAxis = carResult.transforms.chassis * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

            StateManager::States currentState = StateManager::get()->getState();

            if (// do not update the third person camera if not in a correct state
                (currentState == StateManager::States::Running ||
                 currentState == StateManager::States::StartGeneration) &&
                // do not update the third person camera if too close from the target
                glm::distance(carOrigin, camera.thirdPersonEye) > 0.25f)
            {
                // simple lerp to setup the third person camera
                const float lerpRatio = 0.1f * 60.0f * elapsedTime;
                camera.thirdPersonEye += (carOrigin - camera.thirdPersonEye) * lerpRatio;
                camera.thirdPersonUpAxis += (carUpAxis - camera.thirdPersonUpAxis) * lerpRatio;
            }

            const glm::vec3 eye = camera.thirdPersonEye;
            const glm::vec3 target = carOrigin;
            const glm::vec3 upAxis = camera.thirdPersonUpAxis;

            matrices.thirdPerson.model = glm::identity<glm::mat4>();
            matrices.thirdPerson.view = glm::lookAt(eye, target, upAxis);

            matrices.thirdPerson.composed = matrices.thirdPerson.projection * matrices.thirdPerson.model * matrices.thirdPerson.view;
        }

    } // third person

    { // hud ortho

        const auto& vSize = camera.viewportSize;

        glm::mat4 projection = glm::ortho(0.0f, vSize.x, 0.0f, vSize.y, -1.0f, 1.0f);

        glm::vec3 eye = { 0.0f, 0.0f, 0.5f };
        glm::vec3 center = { 0.0f, 0.0f, 0.0f };
        glm::vec3 upAxis = { 0.0f, 1.0f, 0.0f };
        glm::mat4 viewMatrix = glm::lookAt(eye, center, upAxis);

        matrices.hud_ortho = projection * viewMatrix;

    } // hud ortho

    { // hud_perspective

        const auto& vSize = camera.viewportSize;

        // const float fovy = glm::radians(70.0f);
        // const float aspectRatio = float(vSize.x) / vSize.y;
        // glm::mat4 projection = glm::perspective(fovy, aspectRatio, 1.0f, 1000.f);
        glm::mat4 projection = glm::ortho(0.0f, vSize.x, 0.0f, vSize.y, -1.0f, 1.0f);

        // const glm::vec2 halfViewportSize = vSize * 0.5f;
        // glm::vec3 eye = { halfViewportSize.x, halfViewportSize.y, 425.0f };
        // glm::vec3 center = { halfViewportSize.x, halfViewportSize.y, 0.0f };
        glm::vec3 eye = { 0.0f, 0.0f, 0.5f };
        glm::vec3 center = { 0.0f, 0.0f, 0.0f };
        glm::vec3 upAxis = { 0.0f, 1.0f, 0.0f };
        glm::mat4 viewMatrix = glm::lookAt(eye, center, upAxis);

        matrices.hud_perspective = projection * viewMatrix;

    } // hud_perspective
}

void Scene::updateCircuitAnimation(float elapsedTime)
{
    // do not run if not currently in one of those states
    auto currentState = StateManager::get()->getState();
    if (currentState != StateManager::States::StartGeneration &&
        currentState != StateManager::States::Running)
        return;

    auto& logic = Data::get().logic;
    const auto& simulation = *logic.simulation;
    auto& graphic = Data::get().graphic;

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
            animation.lowerValue -= 60.0f * elapsedTime;
            if (animation.lowerValue < animation.targetValue)
                animation.lowerValue = animation.targetValue;
        }
        else if (animation.lowerValue > animation.targetValue)
        {
            // fall quickly
            animation.lowerValue -= 18.0f * elapsedTime;
            if (animation.lowerValue < animation.targetValue)
                animation.lowerValue = animation.targetValue;
        }
        else
        {
            // rise slowly
            animation.lowerValue += 12.0f * elapsedTime;
            if (animation.lowerValue > animation.targetValue)
                animation.lowerValue = animation.targetValue;
        }

        // upper value, farthest from the cars

        if (animation.upperValue > animation.targetValue + 10.0f)
        {
            // fall really quickly
            animation.upperValue -= 36.0f * elapsedTime;
            if (animation.upperValue < animation.targetValue)
                animation.upperValue = animation.targetValue;
        }
        else if (animation.upperValue > animation.targetValue)
        {
            // fall slowly
            animation.upperValue -= 6.0f * elapsedTime;
            if (animation.upperValue < animation.targetValue)
                animation.upperValue = animation.targetValue;
        }
        else
        {
            // rise really quickly
            animation.upperValue += 60.0f * elapsedTime;
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
}

void Scene::_clear()
{
    const auto& viewportSize = Data::get().graphic.camera.viewportSize;

    glViewport(0, 0, viewportSize.x, viewportSize.y);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene::_renderLeadingCarSensors(const glm::mat4& sceneMatrix)
{
    auto&       data = Data::get();
    const auto& logic = data.logic;
    const auto& leaderCar = logic.leaderCar;
    const auto& simulation = *logic.simulation;
    auto&       graphic = data.graphic;
    auto&       stackRenderer = graphic.stackRenderer;
    const auto& shader = *graphic.shaders.stackRenderer;

    // valid leading car?
    if (leaderCar.index < 0)
        return;

    const auto& carData = simulation.getCarResult(leaderCar.index);

    // leading car alive?
    if (!carData.isAlive)
        return;

    // do not show the sensor until far enough
    if (carData.groundIndex < 1)
        return;

    shader.bind();
    shader.setUniform("u_composedMatrix", sceneMatrix);

    const glm::vec3 greenColor(0.0f, 1.0f, 0.0f);
    const glm::vec3 yellowColor(1.0f, 1.0f, 0.0f);
    const glm::vec3 orangeColor(1.0f, 0.5f, 0.0f);
    const glm::vec3 redColor(1.0f, 0.0f, 0.0f);
    const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);

    // eye sensors
    for (const auto& sensor : carData.eyeSensors)
    {
        glm::vec3 color = greenColor;
        if (sensor.value < 0.125f)
            color = redColor;
        else if (sensor.value < 0.25f)
            color = orangeColor;
        else if (sensor.value < 0.5f)
            color = yellowColor;

        stackRenderer.pushLine(sensor.near, sensor.far, color);
        stackRenderer.pushCross(sensor.far, color, 1.0f);
    }

    // ground sensor
    const auto& groundSensor = carData.groundSensor;
    stackRenderer.pushLine(groundSensor.near, groundSensor.far, whiteColor);
    stackRenderer.pushCross(groundSensor.far, whiteColor, 1.0f);

    stackRenderer.flush();
}

void Scene::_renderParticles(const glm::mat4 &sceneMatrix)
{
    // instanced geometrie(s)

    const auto& graphic = Data::get().graphic;
    const auto& shader = *graphic.shaders.particles;
    const auto& geometry = graphic.geometries.particles.firework;

    shader.bind();
    shader.setUniform("u_composedMatrix", sceneMatrix);

    geometry.render();
}

void Scene::_renderFloor(const glm::mat4& sceneMatrix)
{
    const auto& graphic = Data::get().graphic;

    const auto& shader = *graphic.shaders.simpleTexture;

    shader.bind();
    shader.setUniform("u_composedMatrix", sceneMatrix);

    graphic.textures.chessboard.bind();

    // hide the floor if the camera is looking from beneath it
    glEnable(GL_CULL_FACE);

    graphic.geometries.ground.chessboard.render();

    glDisable(GL_CULL_FACE);
}

void Scene::_renderCars(const glm::mat4& sceneMatrix)
{
    // instanced geometrie(s)

    auto& data = Data::get();
    auto& graphic = data.graphic;
    const auto& logic = data.logic;
    const auto& simulation = *logic.simulation;

    graphic.shaders.model->bind();
    graphic.shaders.model->setUniform("u_composedMatrix", sceneMatrix);

    unsigned int totalCars = simulation.getTotalCars();

    struct Attributes
    {
        glm::mat4   tranform;
        glm::vec3   color;

        Attributes(const glm::mat4& tranform, const glm::vec3& color)
            : tranform(tranform)
            , color(color)
        {}
    };

    std::vector<Attributes> modelsChassisMatrices;
    std::vector<Attributes> modelWheelsMatrices;

    modelsChassisMatrices.reserve(totalCars); // pre-allocate
    modelWheelsMatrices.reserve(totalCars * 4); // pre-allocate

    glm::vec3 modelHeight(0.0f, 0.0f, 0.2f);

    const glm::vec3 whiteColor(1, 1, 1);
    const glm::vec3 greenColor(0, 1, 0);
    const glm::vec3 redColor(1, 0, 0);
    const glm::vec3& leaderColor = whiteColor;
    const glm::vec3& lifeColor = greenColor;
    const glm::vec3& deathColor = redColor;

    for (unsigned int ii = 0; ii < totalCars; ++ii)
    {
        const auto& carData = simulation.getCarResult(ii);

        if (!carData.isAlive)
            continue;

        //
        // 3d clipping

        glm::mat4 chassisTransform = glm::translate(carData.transforms.chassis, modelHeight);
        glm::vec4 carOrigin = chassisTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        if (!graphic.camera.frustumCulling.sphereInFrustum(carOrigin, 5.0f))
            continue;

        //
        // color

        const bool isLeader = (logic.leaderCar.index == int(ii));
        glm::vec3 targetColor = isLeader ? leaderColor : lifeColor;
        glm::vec3 color = glm::mix(deathColor, targetColor, carData.life);

        //
        // transforms

        modelsChassisMatrices.emplace_back(chassisTransform, color);
        for (const auto& wheelTransform : carData.transforms.wheels)
            modelWheelsMatrices.emplace_back(wheelTransform, whiteColor);
    }

    graphic.geometries.model.car.updateBuffer(1, modelsChassisMatrices);
    graphic.geometries.model.car.setInstancedCount(modelsChassisMatrices.size());

    graphic.geometries.model.wheel.updateBuffer(1, modelWheelsMatrices);
    graphic.geometries.model.wheel.setInstancedCount(modelWheelsMatrices.size());

    graphic.geometries.model.car.render();
    graphic.geometries.model.wheel.render();
}

void Scene::_renderWireframesGeometries(const glm::mat4& sceneMatrix, bool trails /*= true*/)
{
    // static geometrie(s) (mono color)

    const auto& data = Data::get();
    const auto& graphic = data.graphic;
    const auto& shader = *graphic.shaders.wireframes;
    const auto& wireframes = graphic.geometries.wireframes;

    shader.bind();
    shader.setUniform("u_composedMatrix", sceneMatrix);

    GLint colorLoc = shader.getUniform("u_color");

    { // circuit skeleton

        shader.setUniform(colorLoc, 0.6f, 0.6f, 0.6f, 1.0f);

        wireframes.circuitSkelton.render();

    } // circuit skeleton

    if (!trails)
        return;

    shader.setUniform(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);

    { // best trails

        for (const auto& currentCarTrail : wireframes.bestNewCarsTrails)
            for (const auto& wheelTrail : currentCarTrail.wheels)
                wheelTrail.render();

    } // best trails

    { // leader trail

        if (data.logic.leaderCar.index >= 0)
        {
            auto& reusedGeometry = Data::get().graphic.geometries.wireframes.leaderCarTrail;

            const auto& trailData = data.logic.carsTrails.allWheelsTrails[data.logic.leaderCar.index];

            // rely on only the 30 last positions recorded
            constexpr int maxSize = 30;

            for (const auto& currWheel : trailData.wheels)
            {
                if (currWheel.empty())
                    continue;

                const int totalSize = currWheel.size();
                const int currSize = std::min(totalSize, maxSize);

                const float* dataPointer = &currWheel[totalSize - currSize].x;
                const int dataSize = currSize * sizeof(glm::vec3);

                reusedGeometry.updateBuffer(0, dataPointer, dataSize, true);
                reusedGeometry.setPrimitiveCount(currSize);
                reusedGeometry.render();
            }
        }

    } // leader trail
}

void Scene::_renderAnimatedCircuit(const glm::mat4& sceneMatrix)
{
    // static geometrie(s) (animated)

    const auto& data = Data::get();
    const auto& graphic = data.graphic;
    const auto& shader = *graphic.shaders.animatedCircuit;
    const auto& animatedCircuit = graphic.geometries.animatedCircuit;
    const auto& circuitAnimation = data.logic.circuitAnimation;

    shader.bind();
    shader.setUniform("u_composedMatrix", sceneMatrix);
    shader.setUniform("u_lowerLimit", circuitAnimation.lowerValue);
    shader.setUniform("u_upperLimit", circuitAnimation.upperValue);

    GLint alphaLoc = shader.getUniform("u_alpha");
    shader.setUniform(alphaLoc, 0.8f);

    animatedCircuit.ground.render();

    glDisable(GL_DEPTH_TEST); // <= prevent "blending artifact"

    shader.setUniform(alphaLoc, 0.2f);

    animatedCircuit.walls.render();

    glEnable(GL_DEPTH_TEST);
}

void Scene::_renderHUD_ortho()
{
    auto& data = Data::get();
    auto& graphic = data.graphic;
    auto& logic = data.logic;

    // glDisable(GL_DEPTH_TEST); // <= not useful for a HUD

    { // texts

        const auto& shader = *graphic.shaders.hudText;
        auto&       textRenderer = graphic.hudText.renderer;
        const auto& simulation = *logic.simulation;
        const auto& hudText = logic.hudText;

        shader.bind();
        shader.setUniform("u_composedMatrix", graphic.camera.matrices.hud_ortho);

        graphic.textures.textFont.bind();

        textRenderer.clear();

        const auto& vSize = graphic.camera.viewportSize;

        { // top-left header text

            textRenderer.push({ 8, vSize.y - 16 - 8 }, hudText.header, 1.0f);

        } // top-left header text

        { // top-left performance stats

            std::stringstream sstr;

            sstr
                << "Update: " << writeTime(logic.metrics.updateTime) << std::endl
                << "Render: " << writeTime(logic.metrics.renderTime);

            std::string str = sstr.str();

            textRenderer.push({ 8, vSize.y - 5 * 16 - 8 }, str, 1.0f);

        } // top-left performance stats

        { // bottom-left text

            const unsigned int  totalCars = logic.cores.totalCars;
            unsigned int        carsLeft = 0;
            float               localBestFitness = 0.0f;

            for (unsigned int ii = 0; ii < totalCars; ++ii)
            {
                const auto& carData = simulation.getCarResult(ii);

                if (carData.isAlive)
                    ++carsLeft;

                if (localBestFitness < carData.fitness)
                    localBestFitness = carData.fitness;
            }

            std::stringstream sstr;

            sstr
                << "Generation: " << simulation.getGenerationNumber() << std::endl
                << std::fixed << std::setprecision(1)
                << "Fitness: " << localBestFitness << "/" << simulation.getBestGenome().fitness << std::endl
                << "Cars: " << carsLeft << "/" << totalCars;

            std::string str = sstr.str();

            textRenderer.push({ 8, 8 + 2 * 16 }, str, 1.0f);

        } // bottom-left text

        { // advertise a new leader

            if (logic.leaderCar.index >= 0 &&
                logic.leaderCar.totalTimeAsLeader < 1.0f)
            {
                const auto& scene = graphic.camera.matrices.scene;

                const auto& carData = simulation.getCarResult(logic.leaderCar.index);

                if (// we don't advertise a dead leader
                    carData.isAlive &&
                    // we don't advertise an early leader
                    carData.fitness > 5.0f &&
                    // we don't advertise a dying leader
                    carData.groundSensor.value < 0.5f)
                {
                    const glm::vec3 carPos = carData.transforms.chassis * glm::vec4(0, 0, 0, 1);

                    glm::vec3 screenCoord;

                    const glm::vec2 viewportPos(0, 0); // hardcoded :(

                    bool isVisible = sceneToScreen(
                        carPos,
                        scene.view * scene.model,
                        scene.projection,
                        viewportPos,
                        graphic.camera.viewportSize,
                        screenCoord
                    );

                    if (isVisible &&
                        // out of range?
                        screenCoord.z < 1.0f)
                    {
                        glm::vec2 textPos = { screenCoord.x + 50, screenCoord.y + 50 };

                        textRenderer.push(textPos, "NEW\nLEADER", 1.1f);

                        auto& stackRenderer = graphic.stackRenderer;
                        stackRenderer.pushLine(screenCoord, textPos, {1, 1, 1});
                    }
                }
            }

        } // advertise a new leader

        { // show cores status

            std::stringstream sstr;

            unsigned int globalDelta = 0;
            unsigned int globalGenomesAlive = 0;
            for (unsigned int ii = 0; ii < logic.cores.statesData.size(); ++ii)
            {
                const auto& coreState = logic.cores.statesData[ii];

                globalDelta += coreState.delta;
                globalGenomesAlive += coreState.genomesAlive;
            }


            {
                // const auto& coreState = logic.cores.statesData[ii];

#if defined D_WEB_WEBWORKER_BUILD

                sstr
                    << "WORKERS: " << logic.cores.statesData.size() << std::endl
                    << "Total: " << writeTime(globalDelta * 1000);

#else

                sstr
                    << "THREADS: " << logic.cores.statesData.size() << std::endl
                    << "Total: " << writeTime(globalDelta);

#endif
            }

            std::string str = sstr.str();

            textRenderer.push({ 8, 8 + 8 * 16 }, str, 1.0f);

        } // show cores status

        { // big titles

            StateManager::States currentState = StateManager::get()->getState();

#if defined D_WEB_WEBWORKER_BUILD
            if (currentState == StateManager::States::WorkersLoading)
            {
                float scale = 2.0f;

                std::stringstream sstr;
                sstr
                    << "WEB WORKERS" << std::endl
                    << "  LOADING  " << std::endl;
                std::string message = sstr.str();

                textRenderer.push({ vSize.x * 0.5f - 5 * 16 * scale, vSize.y * 0.5f - 8 * scale }, message, scale);
            }
#endif

            if (currentState == StateManager::States::Paused)
            {
                float scale = 5.0f;

                std::string message = "PAUSED";

                textRenderer.push({ vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale, vSize.y * 0.5f - 8 * scale }, message, scale);
            }

            if (currentState == StateManager::States::StartGeneration)
            {
                float scale = 2.0f;

                std::stringstream sstr;
                sstr << "Generation: " << simulation.getGenerationNumber();
                std::string message = sstr.str();

                textRenderer.push({ vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale, vSize.y * 0.5f - 8 * scale }, message, scale);
            }

        } // big titles

        textRenderer.render();

    } // texts

    { // cores history graphics

        auto& stackRenderer = graphic.stackRenderer;
        const auto& shader = *graphic.shaders.stackRenderer;

        shader.bind();
        shader.setUniform("u_composedMatrix", graphic.camera.matrices.hud_ortho);

        const glm::vec3    whiteColor(1.0f, 1.0f, 1.0f);

        const glm::vec2 borderPos(8, 4 * 16 + 7);
        const glm::vec2 borderSize(150, 48);

        const auto& cores = data.logic.cores;

        unsigned int commonMaxDelta = 0;
        for (auto& stateHistory : cores.statesHistory)
            for (auto& history : stateHistory)
                commonMaxDelta = std::max(commonMaxDelta, history.delta);

        constexpr float divider = 5000.0f; // 5ms
        const float maxVal = (std::ceil(float(commonMaxDelta) / divider)) * divider;



        { // background

            glm::vec4 bgColor = commonMaxDelta > 16000 ? glm::vec4(1,0,0, 1.00f) : glm::vec4(0,0,0, 0.75f);

            stackRenderer.pushQuad(glm::vec3(borderPos + borderSize * 0.5f, -0.1f), borderSize, bgColor);
            stackRenderer.pushRectangle(borderPos, borderSize, whiteColor);

        } // background

        //
        //

        { // dividers

            for (float ii = divider; ii < maxVal; ii += divider)
            {
                const float coef = ii / maxVal;

                glm::vec2 coordA(0, borderSize.y * coef);
                glm::vec2 coordB(borderSize.x, borderSize.y * coef);

                stackRenderer.pushLine(borderPos + coordA, borderPos + coordB, whiteColor);
            }

        } // dividers

        //
        //

        for (unsigned core = 0; core < cores.statesHistory.size(); ++core)
        {
            const auto& stateHistory = cores.statesHistory[core];

            float widthStep = borderSize.x / Data::Logic::Cores::maxStateHistory;

            for (unsigned int ii = 0; ii + 1 < Data::Logic::Cores::maxStateHistory; ++ii)
            {
                unsigned int prevIndex = (ii + cores.currHistoryIndex) % Data::Logic::Cores::maxStateHistory;
                unsigned int currIndex = (ii + 1 + cores.currHistoryIndex) % Data::Logic::Cores::maxStateHistory;

                const auto& prevState = stateHistory[prevIndex];
                const auto& currState = stateHistory[currIndex];

                //

                // float prevRatio = float(prevState.delta) / commonMaxDelta;
                // float currRatio = float(currState.delta) / commonMaxDelta;
                float prevRatio = float(prevState.delta) / maxVal;
                float currRatio = float(currState.delta) / maxVal;

                glm::vec2 prevCoord(ii * widthStep, borderSize.y * prevRatio);
                glm::vec2 currCoord((ii + 1) * widthStep, borderSize.y * currRatio);
                // glm::vec3 color = glm::mix(greenColor, redColor, currRatio);
                glm::vec3 color = whiteColor;

                stackRenderer.pushLine(borderPos + prevCoord, borderPos + currCoord, color);
            }
        }

        stackRenderer.flush();

    } // cores history graphics

    auto&       stackRenderer = graphic.stackRenderer;
    const auto& shader = *graphic.shaders.stackRenderer;

    shader.bind();
    shader.setUniform("u_composedMatrix", graphic.camera.matrices.hud_ortho);

    { // progresses curve

        const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);

        const glm::vec2 borderPos(8, 160);
        const glm::vec2 borderSize(150, 75);

        stackRenderer.pushQuad(glm::vec3(borderPos + borderSize * 0.5f, -0.1f), borderSize, glm::vec4(0,0,0, 0.75f));
        stackRenderer.pushRectangle(borderPos, borderSize, whiteColor);

        const auto& allStats = logic.fitnessStats.allStats;

        if (allStats.size() >= 2)
        {
            float maxFitness = 0.0f;
            for (float stat : allStats)
                maxFitness = std::max(maxFitness, stat);

            float stepWidth = borderSize.x / (allStats.size() - 1);

            for (unsigned int ii = 1; ii < allStats.size(); ++ii)
            {
                const float prevData = allStats[ii - 1];
                const float currData = allStats[ii];

                glm::vec2 prevPos = {
                    stepWidth * (ii - 1),
                    (prevData / maxFitness) * borderSize.y,
                };
                glm::vec2 currPos = {
                    stepWidth * ii,
                    (currData / maxFitness) * borderSize.y,
                };

                stackRenderer.pushLine(borderPos + prevPos, borderPos + currPos, whiteColor);
            }
        }

    } // progresses curve

    { // topology

        if (logic.leaderCar.index >= 0)
        {
            const auto& vSize = graphic.camera.viewportSize;

            const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);
            const glm::vec4 redColor(1.0f,0.0f,0.0f,0.85f);
            const glm::vec4 blueColor(0.5f,0.5f,1.0f,0.85f);

            const glm::vec2 borderSize(175, 150);
            const glm::vec2 borderPos(vSize.x - borderSize.x - 10, 10);

            stackRenderer.pushRectangle(borderPos, borderSize, whiteColor);

            std::vector<unsigned int> topologyArray;
            topologyArray.push_back(logic.annTopology.getInput());
            for (const auto& hidden : logic.annTopology.getHiddens())
                topologyArray.push_back(hidden);
            topologyArray.push_back(logic.annTopology.getOutput());

            const NeuralNetworks& neuralNetworks = logic.simulation->getGeneticAlgorithm().getNeuralNetworks();

            std::vector<float> connectionsWeights;
            neuralNetworks[logic.leaderCar.index]->getWeights(connectionsWeights);

            std::vector<std::vector<glm::vec2>> allNeuronPos;
            allNeuronPos.resize(topologyArray.size());

            glm::vec2 neuronSize(7, 7);

            for (unsigned int ii = 0; ii < topologyArray.size(); ++ii)
            {
                unsigned int actualSize = topologyArray[ii];

                glm::vec2 currPos = borderPos;
                currPos.y += borderSize.y - borderSize.y / topologyArray.size() * (float(ii) + 0.5f);

                allNeuronPos[ii].reserve(actualSize); // pre-allocate

                for (unsigned int jj = 0; jj < actualSize; ++jj)
                {
                    currPos.x += borderSize.x / (actualSize + 1);

                    allNeuronPos[ii].emplace_back(currPos);
                }
            }

            // draw neurons
            for (const auto& layer : allNeuronPos)
                for (const auto& neuron : layer)
                    stackRenderer.pushQuad(neuron, neuronSize, glm::vec4(whiteColor, 1));

            // draw connections

            int connectionIndex = 0;

            for (unsigned int ii = 1; ii < allNeuronPos.size(); ++ii)
            {
                const std::vector<glm::vec2>& prevLayer = allNeuronPos[ii - 1];
                const std::vector<glm::vec2>& currLayer = allNeuronPos[ii];

                for (const auto& prevNeuron : prevLayer)
                {
                    for (const auto& currNeuron : currLayer)
                    {
                        const float weight = connectionsWeights[connectionIndex++];

                        if (weight > 0.0f)
                        {
                            const float thickness = 1.0f + weight * +2.0f;
                            stackRenderer.pushThickTriangleLine(prevNeuron, currNeuron, thickness, redColor);
                        }
                        else
                        {
                            const float thickness = 1.0f + weight * -2.0f;
                            stackRenderer.pushThickTriangleLine(prevNeuron, currNeuron, thickness, blueColor);
                        }
                    }
                }
            }
        }

    } // topology

    { // leader's eye

        if (logic.leaderCar.index >= 0)
        {
            const auto& vSize = graphic.camera.viewportSize;

            const auto& leader = logic.simulation->getCarResult(logic.leaderCar.index);

            const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);
            const glm::vec3 greenColor(0.0f, 1.0f, 0.0f);
            const glm::vec3 redColor(1.0f, 0.0f, 0.0f);

            const glm::vec2 borderSize(100, 60);
            const glm::vec2 borderPos(vSize.x - borderSize.x - 10, 290);

            stackRenderer.pushRectangle(borderPos, borderSize, whiteColor);

            //
            //

            const unsigned int totalInputs = data.logic.annTopology.getInput();
            const unsigned int layerSize = 5; // <= hardcoded :(
            const unsigned int layerCount = totalInputs / layerSize;

            //
            //

            std::vector<glm::vec2> allPositions;
            allPositions.reserve(totalInputs); // pre-allocate
            for (unsigned int ii = 0; ii < layerCount; ++ii)
                for (unsigned int jj = 0; jj < layerSize; ++jj)
                {
                    glm::vec2 currPos = borderPos;

                    currPos.x += borderSize.x * ((float(jj) + 0.5f) / layerSize);
                    currPos.y += borderSize.y * ((float(ii) + 0.5f) / layerCount);

                    allPositions.push_back(currPos);
                }

            glm::vec2 eyeSize = {19, 19};

            for (unsigned int ii = 0; ii < allPositions.size(); ++ii)
            {
                const auto& position = allPositions[ii];

                stackRenderer.pushRectangle(position - eyeSize * 0.5f, eyeSize, whiteColor);

                glm::vec3 color = glm::mix(redColor, greenColor, leader.eyeSensors[ii].value);

                stackRenderer.pushQuad(position, eyeSize, glm::vec4(color, 1.0f));
            }
        }

    } // leader's eye

    stackRenderer.flush();

    glEnable(GL_DEPTH_TEST);
}

void Scene::_renderHUD_thirdPerson()
{
    auto& data = Data::get();
    auto& logic = data.logic;
    const auto& leaderCar = logic.leaderCar;

    // valid leading car?
    if (logic.isAccelerated || leaderCar.index < 0)
        return;

    auto& camera = data.graphic.camera;
    const auto& matrices = camera.matrices;

    const auto& viewportSize = camera.viewportSize;

    // const float divider = 5.25f; // ratio of the viewport current size
    // const glm::vec2 thirdPViewportSize = viewportSize * (1.0f / divider);
    // const glm::vec2 thirdPViewportPos = { thirdPViewportSize.x * (divider - 1.05f), thirdPViewportSize.y * 0.85f };
    const glm::vec2 thirdPViewportSize(150, 110);
    // const glm::vec2 thirdPViewportPos = { viewportSize.x - thirdPViewportSize.x - 10, 100 };
    const glm::vec2 thirdPViewportPos = { viewportSize.x - thirdPViewportSize.x - 10, 170 };

    glEnable(GL_SCISSOR_TEST);
    glScissor(thirdPViewportPos.x, thirdPViewportPos.y, thirdPViewportSize.x, thirdPViewportSize.y);

    glViewport(thirdPViewportPos.x, thirdPViewportPos.y, thirdPViewportSize.x, thirdPViewportSize.y);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 modelView = matrices.thirdPerson.view * matrices.thirdPerson.model;
    camera.frustumCulling.calculateFrustum(matrices.thirdPerson.projection, modelView);

    Scene::_renderLeadingCarSensors(matrices.thirdPerson.composed);
    Scene::_renderParticles(matrices.thirdPerson.composed);

    Scene::_renderFloor(matrices.thirdPerson.composed);

    Scene::_renderCars(matrices.thirdPerson.composed);

    Scene::_renderWireframesGeometries(matrices.thirdPerson.composed);
    Scene::_renderAnimatedCircuit(matrices.thirdPerson.composed);

    glDisable(GL_SCISSOR_TEST);
    glViewport(0, 0, viewportSize.x, viewportSize.y);
}

void Scene::_renderHUD()
{
    auto& data = Data::get();
    auto& graphic = data.graphic;

    { // render in framebuffer

        graphic.hudComponents.frameBuffer.bind();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // alpha must also be 0
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Scene::_renderHUD_ortho();
        Scene::_renderHUD_thirdPerson();

        FrameBuffer::unbind();

    } // render in framebuffer

    { // render framebuffer texture in curved geometry

        const auto& shader = *graphic.shaders.simpleTexture;

        shader.bind();
        shader.setUniform("u_composedMatrix", graphic.camera.matrices.hud_perspective);

        glDisable(GL_DEPTH_TEST);

        graphic.hudComponents.colorTexture.bind();

        graphic.geometries.hudPerspective.geometry.render();

        glEnable(GL_DEPTH_TEST);

    } // render framebuffer texture in curved geometry
}
