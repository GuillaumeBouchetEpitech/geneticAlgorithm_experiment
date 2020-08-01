
#include "Scene.hpp"

#include "demo/states/StateManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/logic/graphic/wrappers/Shader.hpp"

#include "demo/utilities/TraceLogger.hpp"

#include "./utilities/sceneToScreen.hpp"

#include <memory> // <= make_unique
#include <cstring> // <= std::memcpy

#include <iomanip>
#include <sstream>

namespace /*anonymous*/
{

std::string writeTime(unsigned int time)
{
    std::stringstream sstr;
    sstr << std::setw(5) << std::fixed << std::setprecision(1);

    if (time < 1000)
        sstr << time << "us";
    else if (time < 1000000)
        sstr << (float(time) / 1000) << "ms";
    else
        sstr << (float(time) / 1000000) << "s";

    return sstr.str();
}

}

void Scene::renderSimple()
{
    Scene::updateMatrices();

    Scene::clear();

    { // scene

        const auto& scene = Data::get().graphic.camera.matrices.scene;

        Scene::renderWireframesGeometries(scene.composed, false); // circuit only
    }

    { // HUD

        Scene::renderHUD();
    }

    Shader::unbind();
}

void Scene::renderAll()
{
    Scene::updateMatrices();

    Scene::clear();

    { // scene


        auto& data = Data::get();
        auto& logic = data.logic;
        const auto& leaderCar = logic.leaderCar;
        auto& camera = data.graphic.camera;
        const auto& matrices = camera.matrices;

        camera.frustumCulling.calculateFrustum(matrices.scene.projection, matrices.scene.modelView);

        if (!Data::get().logic.isAccelerated)
            Scene::renderLeadingCarSensors(matrices.scene.composed);

        Scene::renderParticles(matrices.scene.composed);
        Scene::renderCars(matrices.scene.composed);
        Scene::renderWireframesGeometries(matrices.scene.composed);
        Scene::renderAnimatedCircuit(matrices.scene.composed);

        // valid leading car?
        if (!logic.isAccelerated && leaderCar.index >= 0)
        {
            const auto& viewportSize = camera.viewportSize;

            const float divider = 5.0f;
            const glm::vec2 thirdPViewportSize = viewportSize * (1.0f / divider);
            const glm::vec2 thirdPViewportPos = { thirdPViewportSize.x * (divider - 1.0f), thirdPViewportSize.y * 0.75f };

            glViewport(thirdPViewportPos.x, thirdPViewportPos.y, thirdPViewportSize.x, thirdPViewportSize.y);

            glEnable(GL_SCISSOR_TEST);
            glScissor(thirdPViewportPos.x, thirdPViewportPos.y, thirdPViewportSize.x, thirdPViewportSize.y);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            camera.frustumCulling.calculateFrustum(matrices.thirdPerson.projection, matrices.thirdPerson.modelView);

            Scene::renderLeadingCarSensors(matrices.thirdPerson.composed);
            Scene::renderParticles(matrices.thirdPerson.composed);
            Scene::renderCars(matrices.thirdPerson.composed);
            Scene::renderWireframesGeometries(matrices.thirdPerson.composed);
            Scene::renderAnimatedCircuit(matrices.thirdPerson.composed);

            glDisable(GL_SCISSOR_TEST);
            glViewport(0, 0, viewportSize.x, viewportSize.y);
        }

    }

    { // HUD

        Scene::renderHUD();
    }

    Shader::unbind();
}

void Scene::updateMatrices()
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

        // clamp vertical rotation [-pi/2..+pi/2]
        const float verticalLimit = 3.14f * 0.5f;
        camera.rotations.phi = std::max(-verticalLimit, std::min(verticalLimit, camera.rotations.phi));

        camera.eye = {
            camera.distance * std::cos(camera.rotations.phi) * std::cos(camera.rotations.theta),
            camera.distance * std::cos(camera.rotations.phi) * std::sin(camera.rotations.theta),
            camera.distance * std::sin(camera.rotations.phi),
        };
        glm::vec3 center = { 0.0f, 0.0f, 0.0f };
        glm::vec3 upAxis = { 0.0f, 0.0f, 1.0f };
        glm::mat4 viewMatrix = glm::lookAt(camera.eye, center, upAxis);

        // camera.front = glm::normalize(center - camera.eye);
        camera.front = glm::normalize(camera.eye - center);

        glm::mat4 modelMatrix = glm::mat4(1.0f); // <= identity matrix
        modelMatrix = glm::translate(modelMatrix, -camera.center);

        matrices.scene.modelView = viewMatrix * modelMatrix;

        matrices.scene.composed = matrices.scene.projection * matrices.scene.modelView;
    }

    { // third person

        matrices.thirdPerson.projection = matrices.scene.projection;

        if (logic.leaderCar.index < 0)
        {
            camera.thirdPersonCenter = camera.center;

            matrices.thirdPerson.modelView = matrices.scene.modelView;
            matrices.thirdPerson.composed = matrices.scene.composed;
        }
        else
        {
            const auto& carResult = simulation.getCarResult(logic.leaderCar.index);

            glm::vec3 carOrigin = carResult.transform * glm::vec4(0.0f, 0.0f, 2.5f, 1.0f);

            if (// do not update the third person camera if not in a correct state
                (StateManager::get()->getState() == StateManager::States::eRunning ||
                 StateManager::get()->getState() == StateManager::States::eStartGeneration) &&
                // do not update the third person camera if too close from the target
                glm::distance(carOrigin, camera.thirdPersonCenter) > 0.1f)
            {
                // simple lerp to setup the third person camera
                const float lerpRatio = 0.1f;
                camera.thirdPersonCenter += (carOrigin - camera.thirdPersonCenter) * lerpRatio;
            }

            glm::vec3 eye = camera.thirdPersonCenter;
            glm::vec3 center = carOrigin;
            glm::vec3 upAxis = { 0.0f, 0.0f, 1.0f };
            matrices.thirdPerson.modelView = glm::lookAt(eye, center, upAxis);

            matrices.thirdPerson.composed = matrices.thirdPerson.projection * matrices.thirdPerson.modelView;
        }
    }

    { // hud

        const auto& vSize = camera.viewportSize;
        glm::mat4 projection = glm::ortho(0.0f, vSize.x, 0.0f, vSize.y, -1.0f, 1.0f);

        glm::vec3 eye = { 0.0f, 0.0f, 0.5f };
        glm::vec3 center = { 0.0f, 0.0f, 0.0f };
        glm::vec3 upAxis = { 0.0f, 1.0f, 0.0f };
        glm::mat4 viewMatrix = glm::lookAt(eye, center, upAxis);

        matrices.hud = projection * viewMatrix;
    }
}

void Scene::clear()
{
    const auto& viewportSize = Data::get().graphic.camera.viewportSize;

    glViewport(0, 0, viewportSize.x, viewportSize.y);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene::renderLeadingCarSensors(const glm::mat4& sceneMatrix)
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

    shader.bind();

    GLint composedMatrixLoc = shader.getUniform("u_composedMatrix");
    glUniformMatrix4fv(composedMatrixLoc, 1, false, glm::value_ptr(sceneMatrix));

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

void Scene::renderParticles(const glm::mat4 &sceneMatrix)
{
    // instanced geometrie(s)

    const auto& graphic = Data::get().graphic;

    {
        const auto& shader = *graphic.shaders.particles;
        const auto& geometry = graphic.geometries.particles.firework;

        shader.bind();

        GLint composedMatrixLoc = shader.getUniform("u_composedMatrix");
        glUniformMatrix4fv(composedMatrixLoc, 1, false, glm::value_ptr(sceneMatrix));

        geometry.render();
    }
}

void Scene::renderCars(const glm::mat4& sceneMatrix)
{
    // instanced geometrie(s)

    auto& graphic = Data::get().graphic;

    {
        graphic.shaders.model->bind();

        GLint composedMatrixLoc = graphic.shaders.model->getUniform("u_composedMatrix");
        glUniformMatrix4fv(composedMatrixLoc, 1, false, glm::value_ptr(sceneMatrix));

        {
            const auto& logic = Data::get().logic;
            const auto& simulation = *logic.simulation;

            unsigned int totalCars = simulation.getTotalCars();

            struct t_attributes
            {
                glm::mat4   tranform;
                glm::vec3   color;
            };

            std::vector<t_attributes> modelsChassisMatrices;
            std::vector<t_attributes> modelWheelsMatrices;

            modelsChassisMatrices.reserve(totalCars); // pre-allocate
            modelWheelsMatrices.reserve(totalCars * 4); // pre-allocate

            glm::vec3 modelHeight(0.0f, 0.0f, 0.2f);

            const glm::vec3 leaderColor(1, 1, 1);
            const glm::vec3 lifeColor(0, 1, 0);
            const glm::vec3 deathColor(1.0f, 0.0f, 0.0f);

            for (unsigned int ii = 0; ii < totalCars; ++ii)
            {
                const auto& carData = simulation.getCarResult(ii);

                if (!carData.isAlive)
                    continue;

                //
                // 3d clipping

                glm::mat4 chassisTransform = glm::translate(carData.transform, modelHeight);
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

                modelsChassisMatrices.push_back({ chassisTransform, color });
                for (const auto& wheelTransform : carData.wheelsTransform)
                    modelWheelsMatrices.push_back({ wheelTransform, color });
            }

            graphic.geometries.model.car.updateBuffer(1, modelsChassisMatrices);
            graphic.geometries.model.car.setInstancedCount(modelsChassisMatrices.size());

            graphic.geometries.model.wheel.updateBuffer(1, modelWheelsMatrices);
            graphic.geometries.model.wheel.setInstancedCount(modelWheelsMatrices.size());

        }

        graphic.geometries.model.car.render();
        graphic.geometries.model.wheel.render();
    }

}

void Scene::renderWireframesGeometries(const glm::mat4& sceneMatrix, bool trails /*= true*/)
{
    // static geometrie(s) (mono color)

    const auto& graphic = Data::get().graphic;
    const auto& shader = *graphic.shaders.wireframes;
    const auto& wireframes = graphic.geometries.wireframes;

    shader.bind();

    GLint composedMatrixLoc = shader.getUniform("u_composedMatrix");
    glUniformMatrix4fv(composedMatrixLoc, 1, false, glm::value_ptr(sceneMatrix));

    GLint colorLoc = shader.getUniform("u_color");

    glUniform4f(colorLoc, 0.6f, 0.6f, 0.6f, 1.0f);

    wireframes.circuitSkelton.render();

    if (!trails)
        return;

    glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);

    for (const auto& currentCarTrail : wireframes.bestNewCarsTrails)
        for (const auto& wheelTrail : currentCarTrail.wheels)
            wheelTrail.render();
}

void Scene::renderAnimatedCircuit(const glm::mat4& sceneMatrix)
{
    // static geometrie(s) (animated)

    const auto& data = Data::get();
    const auto& graphic = data.graphic;
    const auto& shader = *graphic.shaders.animatedCircuit;
    const auto& animatedCircuit = graphic.geometries.animatedCircuit;
    const auto& circuitAnimation = data.logic.circuitAnimation;

    shader.bind();

    GLint composedMatrixLoc = shader.getUniform("u_composedMatrix");
    glUniformMatrix4fv(composedMatrixLoc, 1, false, glm::value_ptr(sceneMatrix));

    GLint lowerLimitLoc = shader.getUniform("u_lowerLimit");
    GLint upperLimitLoc = shader.getUniform("u_upperLimit");
    glUniform1f(lowerLimitLoc, circuitAnimation.lowerValue);
    glUniform1f(upperLimitLoc, circuitAnimation.upperValue);

    GLint alphaLoc = shader.getUniform("u_alpha");
    glUniform1f(alphaLoc, 0.8f);

    animatedCircuit.ground.render();

    glDisable(GL_DEPTH_TEST); // <= prevent "blending artifact"

    glUniform1f(alphaLoc, 0.2f);

    animatedCircuit.walls.render();

    glEnable(GL_DEPTH_TEST);
}

void Scene::renderHUD()
{
    auto& data = Data::get();
    auto& graphic = data.graphic;
    auto& logic = data.logic;

    glDisable(GL_DEPTH_TEST); // <= not useful for a HUD

    { // texts

        const auto& shader = *graphic.shaders.hudText;
        auto&       textRenderer = graphic.hudText.renderer;
        const auto& simulation = *logic.simulation;
        const auto& hudText = logic.hudText;

        shader.bind();

        const auto& hudMatrix = graphic.camera.matrices.hud;
        GLint composedMatrixLoc = shader.getUniform("u_composedMatrix");
        glUniformMatrix4fv(composedMatrixLoc, 1, false, glm::value_ptr(hudMatrix));

        graphic.textures.textFont.bind();

        textRenderer.clear();
        textRenderer.push({ 8, 600 - 16 - 8 }, hudText.header, 1.0f);

        if (!hudText.pthreadWarning.empty())
            textRenderer.push({ 800 - 26 * 16 - 8, 3 * 16 - 8 }, hudText.pthreadWarning, 1.0f);

        {
            std::stringstream sstr;

            sstr
                << "update: " << writeTime(logic.metrics.updateTime) << std::endl
                << "render: " << writeTime(logic.metrics.renderTime);

            std::string str = sstr.str();

            textRenderer.push({ 8, 600 - 5 * 16 - 8 }, str, 1.0f);
        }

        {
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
        }

        { // advertise  a new leader

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
                    const glm::vec3 carPos = carData.transform * glm::vec4(0, 0, 0, 1);

                    glm::vec3 screenCoord;

                    const glm::vec2 viewportPos(0, 0); // hardcoded :(
                    const glm::vec2 viewportSize(800, 600); // hardcoded :(

                    sceneToScreen(
                        carPos,
                        scene.modelView,
                        scene.projection,
                        viewportPos,
                        viewportSize,
                        screenCoord
                    );

                    if (screenCoord.z < 1.0f) // <= is false when out of range
                    {
                        glm::vec2 textPos = { screenCoord.x + 50, screenCoord.y + 50 };

                        textRenderer.push(textPos, "NEW LEADER", 1.1f);

                        auto& stackRenderer = graphic.stackRenderer;
                        stackRenderer.pushLine(screenCoord, textPos, {1, 1, 1});
                    }
                }
            }

        } // advertise  a new leader

        { // show cores status

            std::stringstream sstr;

            for (unsigned int ii = 0; ii < logic.cores.statesData.size(); ++ii)
            {
                const auto& coreState = logic.cores.statesData[ii];

#if defined D_WEB_WEBWORKER_BUILD

                sstr
                    << "WORKER_" << (ii + 1) << std::endl
                    << "> " << writeTime(coreState.delta * 1000);

#else

                sstr
                    << "THREAD_" << (ii + 1) << std::endl
                    << "> " << writeTime(coreState.delta);

#endif

                sstr
                    << std::endl
                    << "> " << std::setw(2) << coreState.genomesAlive
                    << " car(s)" << std::endl
                    << std::endl
                    << std::endl
                    << std::endl
                    << std::endl;
            }

            std::string str = sstr.str();

            textRenderer.push({ 8, 8 + 23 * 16 }, str, 1.0f);

        } // show cores status

        { // big titles

#if defined D_WEB_WEBWORKER_BUILD
            if (StateManager::get()->getState() == StateManager::States::eWorkersLoading)
            {
                float scale = 2.0f;

                std::stringstream sstr;
                sstr
                    << "WEB WORKERS" << std::endl
                    << "  LOADING  " << std::endl;
                std::string message = sstr.str();

                textRenderer.push({ 400 - 5 * 16 * scale, 300 - 8 * scale }, message, scale);
            }
#endif

            if (StateManager::get()->getState() == StateManager::States::ePaused)
            {
                float scale = 5.0f;
                textRenderer.push({ 400 - 3 * 16 * scale, 300 - 8 * scale }, "PAUSED", scale);
            }

            if (StateManager::get()->getState() == StateManager::States::eStartGeneration)
            {
                float scale = 2.0f;

                std::stringstream sstr;
                sstr << "Generation: " << simulation.getGenerationNumber();
                std::string message = sstr.str();

                textRenderer.push({ 400 - float(message.size()) / 2 * 16 * scale, 300 - 8 * scale }, message, scale);
            }

        } // big titles

        { // volume

            const auto& hudText = graphic.hudText;
            const glm::vec2 letterSize = hudText.textureSize / hudText.gridSize;

            const float scale = 1.0f;
            const float messageHeight = 4.0f * letterSize.y * scale;
            const float messageHalfHeight = messageHeight * 0.25f;
            float messageSize = 0.0f;
            std::stringstream sstr;

            if (data.sounds.manager.isEnabled())
            {
                sstr
                    << "*-------*" << std::endl
                    << "| SOUND |" << std::endl
                    << "|ENABLED|" << std::endl
                    << "*-------*";

                messageSize = 9.0f;
            }
            else
            {
                sstr
                    << "*--------*" << std::endl
                    << "| SOUND  |" << std::endl
                    << "|DISABLED|" << std::endl
                    << "*--------*";

                messageSize = 10.0f;
            }

            const float messageHalfSize = messageSize * 0.5f * scale;

            const glm::vec2 center(720, 125);

            std::string message = sstr.str();
            textRenderer.push({ center.x - messageHalfSize * letterSize.x, 600 - center.y + messageHalfHeight }, message, scale);

        } // volume

        textRenderer.render();

    } // texts

    { // graphics

        auto& stackRenderer = graphic.stackRenderer;
        const auto& shader = *graphic.shaders.stackRenderer;

        shader.bind();

        const auto& hudMatrix = graphic.camera.matrices.hud;
        GLint composedMatrixLoc = shader.getUniform("u_composedMatrix");
        glUniformMatrix4fv(composedMatrixLoc, 1, false, glm::value_ptr(hudMatrix));

        const glm::vec3    whiteColor(1.0f, 1.0f, 1.0f);

        const glm::vec2 borderPos(8, 18 * 16 + 7);
        const glm::vec2 borderSize(150, 48);
        const float     borderStep = 4 * 16.0f;

        const auto& cores = data.logic.cores;

        unsigned int commonMaxDelta = 0;
        for (auto& stateHistory : cores.statesHistory)
            for (auto& history : stateHistory)
                commonMaxDelta = std::max(commonMaxDelta, history.delta);

        for (unsigned core = 0; core < cores.statesHistory.size(); ++core)
        {
            const auto& stateHistory = cores.statesHistory[core];

            //
            //

            const glm::vec2 currPos(
                borderPos.x,
                borderPos.y - core * (borderSize.y + borderStep)
            );

            stackRenderer.pushRectangle(currPos, borderSize, whiteColor);

            //
            //

            float widthStep = borderSize.x / cores.maxStateHistory;

            for (unsigned int ii = 0; ii + 1 < cores.maxStateHistory; ++ii)
            {
                unsigned int prevIndex = (ii + cores.currHistoryIndex) % cores.maxStateHistory;
                unsigned int currIndex = (ii + 1 + cores.currHistoryIndex) % cores.maxStateHistory;

                const auto& prevState = stateHistory[prevIndex];
                const auto& currState = stateHistory[currIndex];

                //

                float prevRatio = float(prevState.delta) / commonMaxDelta;
                float currRatio = float(currState.delta) / commonMaxDelta;

                glm::vec2 prevCoord(ii * widthStep, borderSize.y * prevRatio);
                glm::vec2 currCoord((ii + 1) * widthStep, borderSize.y * currRatio);
                // glm::vec3 color = glm::mix(greenColor, redColor, currRatio);
                glm::vec3 color = whiteColor;

                stackRenderer.pushLine(currPos + prevCoord, currPos + currCoord, color);
            }
        }

        stackRenderer.flush();

    } // graphics

    /**/
    {
        auto&       stackRenderer = graphic.stackRenderer;
        const auto& shader = *graphic.shaders.stackRenderer;

        shader.bind();

        const auto& hudMatrix = graphic.camera.matrices.hud;
        GLint composedMatrixLoc = shader.getUniform("u_composedMatrix");
        glUniformMatrix4fv(composedMatrixLoc, 1, false, glm::value_ptr(hudMatrix));

        //
        // show progresses here
        {
            const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);

            const glm::vec2 borderPos(10, 400);
            const glm::vec2 borderSize(150, 75);

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
        }
        // show progresses here
        //

        //
        // show topology here
        {
            if (logic.leaderCar.index >= 0)
            {

                // const auto& bestGenome = logic.simulation->getBestGenome();

                // bestGenome.weights;

                const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);
                const glm::vec3 greenColor(0.0f, 0.75f, 0.0f);

                const glm::vec2 borderPos(690, 215);
                const glm::vec2 borderSize(100, 100);

                stackRenderer.pushRectangle(borderPos, borderSize, whiteColor);

                std::vector<unsigned int> topologyArray;
                topologyArray.push_back(logic.annTopology.getInput());
                for (const auto& hidden : logic.annTopology.getHiddens())
                    topologyArray.push_back(hidden);
                topologyArray.push_back(logic.annTopology.getOutput());

                std::vector<std::vector<glm::vec2>> allNeuronPos;
                allNeuronPos.resize(topologyArray.size());

                glm::vec2 neuronSize(7, 7);

                for (unsigned int ii = 0; ii < topologyArray.size(); ++ii)
                {
                    unsigned int actualSize = topologyArray[ii];

                    glm::vec2 currPos = borderPos;
                    currPos.y += borderSize.y - borderSize.y / topologyArray.size() * (float(ii) + 0.5f);

                    allNeuronPos[ii].reserve(actualSize);

                    for (unsigned int jj = 0; jj < actualSize; ++jj)
                    {
                        currPos.x += borderSize.x / (actualSize + 1);

                        allNeuronPos[ii].push_back(currPos);
                    }
                }

                // draw neurons
                for (unsigned int ii = 0; ii < allNeuronPos.size(); ++ii)
                    for (unsigned int jj = 0; jj < allNeuronPos[ii].size(); ++jj)
                        stackRenderer.pushRectangle(allNeuronPos[ii][jj] - neuronSize * 0.5f, neuronSize, whiteColor);

                // draw connections
                for (unsigned int ii = 1; ii < allNeuronPos.size(); ++ii)
                    for (unsigned int jj = 0; jj < allNeuronPos[ii - 1].size(); ++jj)
                        for (unsigned int kk = 0; kk < allNeuronPos[ii].size(); ++kk)
                            stackRenderer.pushLine(allNeuronPos[ii - 1][jj], allNeuronPos[ii][kk], greenColor);
            }
        }
        // show topology here
        //

        //
        // show leader car's eye here
        {
            if (logic.leaderCar.index >= 0)
            {
                const auto& leader = logic.simulation->getCarResult(logic.leaderCar.index);

                const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);
                const glm::vec3 greenColor(0.0f, 1.0f, 0.0f);
                const glm::vec3 redColor(1.0f, 0.0f, 0.0f);

                const glm::vec2 borderPos(690, 320);
                const glm::vec2 borderSize(100, 60);

                stackRenderer.pushRectangle(borderPos, borderSize, whiteColor);

                //
                //

                const unsigned int layerSize = 5; // <= hardcoded :(
                const unsigned int layerCount = data.logic.annTopology.getInput() / layerSize;

                //
                //

                std::vector<glm::vec2> allPositions;
                allPositions.reserve(layerCount * layerSize);
                for (unsigned int ii = 0; ii < layerCount; ++ii)
                    for (unsigned int jj = 0; jj < layerSize; ++jj)
                    {
                        glm::vec2 currPos = borderPos;

                        currPos.x += borderSize.x * ((float(jj) + 0.5f) / layerSize);
                        currPos.y += borderSize.y * ((float(ii) + 0.5f) / layerCount);

                        allPositions.push_back(currPos);
                    }

                glm::vec2 eyeSize = {19, 19};

                const glm::vec3 vertices[4] = {
                    { +eyeSize.x * 0.5f, +eyeSize.y * 0.5f, 0.0f },
                    { -eyeSize.x * 0.5f, +eyeSize.y * 0.5f, 0.0f },
                    { +eyeSize.x * 0.5f, -eyeSize.y * 0.5f, 0.0f },
                    { -eyeSize.x * 0.5f, -eyeSize.y * 0.5f, 0.0f },
                };
                std::array<int, 6> indices = {{ 0,1,2, 2,1,3, }};

                for (unsigned int ii = 0; ii < allPositions.size(); ++ii)
                {
                    const auto& position = allPositions[ii];

                    stackRenderer.pushRectangle(position - eyeSize * 0.5f, eyeSize, whiteColor);

                    glm::vec3 color = glm::mix(redColor, greenColor, leader.eyeSensors[ii].value);

                    glm::vec3 tmpPos = glm::vec3(position, 0.0f);

                    stackRenderer.pushTriangle(vertices[indices[0]] + tmpPos, vertices[indices[1]] + tmpPos, vertices[indices[2]] + tmpPos, color);
                    stackRenderer.pushTriangle(vertices[indices[3]] + tmpPos, vertices[indices[4]] + tmpPos, vertices[indices[5]] + tmpPos, color);
                }

                // logic.simulation->getBestGenome().id;
                // stackRenderer.pushRectangle(borderPos, borderSize, whiteColor);
            }
        }
        // show leader car's eye here
        //

        stackRenderer.flush();
    }
    //*/

    glEnable(GL_DEPTH_TEST);
}
