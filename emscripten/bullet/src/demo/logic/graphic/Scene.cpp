
#include "Scene.hpp"

#include "demo/states/StateManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/logic/graphic/wrappers/Shader.hpp"

#include "demo/utilities/TraceLogger.hpp"

#include "./utilities/sceneToScreen.hpp"


#include "thirdparty/GLMath.hpp"

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

        Scene::renderWireframesGeometries(false); // circuit only
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

        { // checkerboard floor

            // auto& data = *Data::get();
            // auto& logic = data.logic;
            // auto& graphic = data.graphic;
            // // auto& stackRenderer = graphic.stackRenderer;
            // const auto& shader = *graphic.shaders.stackRenderer;

            // shader.bind();

            // const auto& sceneMatrix = graphic.camera.matrices.scene;
            // GLint composedMatrixLoc = shader.getUniform("u_composedMatrix");
            // glUniformMatrix4fv(composedMatrixLoc, 1, false, glm::value_ptr(sceneMatrix));

            // glm::vec3 minVal = logic.circuitAnimation.boundaries.min;
            // glm::vec3 maxVal = logic.circuitAnimation.boundaries.max;

            // minVal += glm::vec3(-50, -50, 0); // add extra
            // maxVal += glm::vec3(+50, +50, 0); // add extra

            // const glm::vec3 color = { 0.25f, 0.25f, 0.25f };
            // const glm::vec3 vertices[4] = {
            //     // { +50.0f, +50.0f, 0.0f },
            //     // { -50.0f, +50.0f, 0.0f },
            //     // { +50.0f, -50.0f, 0.0f },
            //     // { -50.0f, -50.0f, 0.0f },
            //     { maxVal.x, maxVal.y, -0.1f },
            //     { minVal.x, maxVal.y, -0.1f },
            //     { maxVal.x, minVal.y, -0.1f },
            //     { minVal.x, minVal.y, -0.1f },
            // };

            // stackRenderer.pushTriangle(vertices[indices[0]], vertices[indices[1]], vertices[indices[2]], color);
            // stackRenderer.pushTriangle(vertices[indices[3]], vertices[indices[4]], vertices[indices[5]], color);

            // glEnable(GL_CULL_FACE);

            // const glm::vec3 colorA = { 0.25f, 0.25f, 0.25f };
            // const glm::vec3 colorB = { 0.00f, 0.00f, 0.00f };

            // std::array<int, 6> indices = {{ 0,1,2, 2,1,3, }};

            // bool isColorA = true;

            // for (float yy = minVal.y; yy < maxVal.y; yy += 20.0f)
            // for (float xx = minVal.x; xx < maxVal.x; xx += 20.0f)
            // {
            //     const glm::vec3 vertices[4] = {
            //         { xx + 10.0f, yy + 10.0f, -0.1f },
            //         { xx - 10.0f, yy + 10.0f, -0.1f },
            //         { xx + 10.0f, yy - 10.0f, -0.1f },
            //         { xx - 10.0f, yy - 10.0f, -0.1f },
            //     };

            //     stackRenderer.pushTriangle(vertices[indices[0]], vertices[indices[1]], vertices[indices[2]], isColorA ? colorA : colorB);
            //     stackRenderer.pushTriangle(vertices[indices[3]], vertices[indices[4]], vertices[indices[5]], isColorA ? colorA : colorB);

            //     isColorA = !isColorA;
            // }

            // stackRenderer.flushTriangles();

            // glDisable(GL_CULL_FACE);

        } // checkerboard floor


        if (!Data::get().logic.isAccelerated)
            Scene::renderLeadingCarSensors();

        Scene::renderParticles();
        Scene::renderCars();
        // Scene::renderCircuitSkeleton();
        // Scene::renderBestCarsTrails();
        Scene::renderWireframesGeometries();
        Scene::renderAnimatedCircuit();
    }

    { // HUD

        Scene::renderHUD();
    }

    Shader::unbind();
}

void Scene::updateMatrices()
{
    auto&    camera = Data::get().graphic.camera;
    auto&    matrices = camera.matrices;

    { // scene

        const float fovy = glm::radians(70.0f);
        const float aspectRatio = float(camera.viewportSize.x) / camera.viewportSize.y;

        matrices.projection = glm::perspective(fovy, aspectRatio, 1.0f, 1000.f);
        glm::mat4 viewMatrix = glm::mat4(1.0f); // <= identity matrix
        glm::mat4 modelMatrix = glm::mat4(1.0f); // <= identity matrix

        // clamp vertical rotation [0..PI]
        camera.rotations.y = std::max(0.0f, std::min(3.14f, camera.rotations.y));

        viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -camera.distance));
        viewMatrix = glm::rotate(viewMatrix, camera.rotations.y, glm::vec3(-1.0f, 0.0f, 0.0f));
        viewMatrix = glm::rotate(viewMatrix, camera.rotations.x, glm::vec3(0.0f, 0.0f, 1.0f));

        modelMatrix = glm::translate(modelMatrix, camera.center);

        matrices.modelView = viewMatrix * modelMatrix;

        matrices.scene = matrices.projection * matrices.modelView;

        camera.frustumCulling.calculateFrustum(matrices.projection, matrices.modelView);
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

void Scene::renderLeadingCarSensors()
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

    const auto& sceneMatrix = graphic.camera.matrices.scene;
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

void Scene::renderParticles()
{
    // instanced geometrie(s)

    const auto& graphic = Data::get().graphic;
    const auto& sceneMatrix = graphic.camera.matrices.scene;

    {
        const auto& shader = *graphic.shaders.particles;
        const auto& geometry = graphic.geometries.particles.firework;

        shader.bind();

        GLint composedMatrixLoc = shader.getUniform("u_composedMatrix");
        glUniformMatrix4fv(composedMatrixLoc, 1, false, glm::value_ptr(sceneMatrix));

        geometry.render();
    }
}

void Scene::renderCars()
{
    // instanced geometrie(s)

    // const auto& graphic = Data::get().graphic;
    auto& graphic = Data::get().graphic;
    const auto& sceneMatrix = graphic.camera.matrices.scene;

    {
        // const auto& shader = *graphic.shaders.instanced;
        // const auto& instanced = graphic.geometries.instanced;

        // shader.bind();

        // GLint composedMatrixLoc = shader.getUniform("u_composedMatrix");
        // glUniformMatrix4fv(composedMatrixLoc, 1, false, glm::value_ptr(sceneMatrix));

        // instanced.chassis.render();

        // instanced.wheels.render();
    }

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

            const glm::vec3 leaderColor = {1, 1, 1};
            const glm::vec3 lifeColor = {0, 1, 0};
            const glm::vec3 deathColor(1.0f, 0.0f, 0.0f);

            for (unsigned int ii = 0; ii < totalCars; ++ii)
            {
                const auto& carData = simulation.getCarResult(ii);

                if (!carData.isAlive)
                    continue;

                //
                // color

                const bool isLeader = (logic.leaderCar.index == int(ii));
                glm::vec3 targetColor = isLeader ? leaderColor : lifeColor;
                glm::vec3 color = glm::mix(deathColor, targetColor, carData.life);

                //
                // transforms

                glm::mat4 chassisTransform = glm::translate(carData.transform, modelHeight);
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

void Scene::renderWireframesGeometries(bool trails /*= true*/)
{
    // static geometrie(s) (mono color)

    const auto& graphic = Data::get().graphic;
    const auto& shader = *graphic.shaders.wireframes;
    const auto& wireframes = graphic.geometries.wireframes;

    shader.bind();

    const auto& sceneMatrix = graphic.camera.matrices.scene;
    GLint composedMatrixLoc = shader.getUniform("u_composedMatrix");
    glUniformMatrix4fv(composedMatrixLoc, 1, false, glm::value_ptr(sceneMatrix));

    GLint colorLoc = shader.getUniform("u_color");

    glUniform4f(colorLoc, 0.6f, 0.6f, 0.6f, 1.0f);

    wireframes.circuitSkelton.render();

    if (trails)
    {
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);

        for (const auto& geometry : wireframes.bestCarsTrails)
            geometry.render();
    }
}

void Scene::renderAnimatedCircuit()
{
    // static geometrie(s) (animated)

    const auto& data = Data::get();
    const auto& graphic = data.graphic;
    const auto& shader = *graphic.shaders.animatedCircuit;
    const auto& animatedCircuit = graphic.geometries.animatedCircuit;
    const auto& circuitAnimation = data.logic.circuitAnimation;

    shader.bind();

    const auto& sceneMatrix = graphic.camera.matrices.scene;
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

    {
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

        {
            /**
            {
                const auto& camera = graphic.camera;

                glm::vec3 screenCoord;

                const unsigned int totalCars = logic.cores.totalCars;
                for (unsigned int ii = 0; ii < totalCars; ++ii)
                {
                    const auto& carData = simulation.getCarResult(ii);

                    if (!carData.isAlive || carData.fitness < 5.0f)
                        continue;

                    const bool isLeader = (logic.leaderCar.index == int(ii));
                    if (!isLeader)
                        continue;

                    const glm::vec3 pos = carData.transform * glm::vec4(0,0,0,1);

                    sceneToScreen(
                        pos,
                        camera.matrices.modelView, camera.matrices.projection,
                        glm::vec2(0,0), glm::vec2(800, 600),
                        screenCoord
                    );

                    if (screenCoord.z > 1.0f)
                        continue;

                    // std::stringstream sstr;

                    // sstr
                    //     << "id" << ii << std::endl
                    //     << carData.life << std::endl
                    //     << carData.fitness;

                    // std::string str = sstr.str();

                    // textRenderer.push({ screenCoord.x, screenCoord.y }, str, 0.75f);

                    textRenderer.push({ screenCoord.x + 50, screenCoord.y + 50 }, "Leader", 1.0f);

                    {
                        auto& stackRenderer = graphic.stackRenderer;
                        const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);

                        stackRenderer.pushLine(screenCoord, { screenCoord.x + 50, screenCoord.y + 50 }, whiteColor);
                    }

                }
            }

            // const auto& camera = graphic.camera;

            // glm::vec3 screenCoord;

            // bool result = sceneToScreen(
            //     glm::vec3(0, 0, 0),
            //     camera.modelViewMatrix, camera.matrices.projection,
            //     glm::vec2(0,0), glm::vec2(800, 600),
            //     screenCoord
            // );

            // if (screenCoord.z < 1.0f)
            // {
            //     std::stringstream sstr;

            //     sstr
            //         << "result: " << result << std::endl
            //         << "coord:" << std::endl
            //         << screenCoord.x << std::endl
            //         << screenCoord.y << std::endl
            //         << screenCoord.z;

            //     std::string str = sstr.str();

            //     textRenderer.push({ screenCoord.x, screenCoord.y }, str, 1.0f);
            // }
            //*/
        }

        /**
        {
            std::stringstream sstr;

            for (unsigned int ii = 0; ii < logic.cores.statesData.size(); ++ii)
            {
                const auto& coreState = logic.cores.statesData[ii];

                sstr << "worker: " << ii;

                for (int jj = 0; jj < 6; ++jj)
                    sstr << std::endl;

#if defined D_WEB_WEBWORKER_BUILD
                // sstr << std::setw(3) << coreState.delta << "ms";
                if (coreState.delta < 1000)
                {
                    sstr
                        << std::setw(4)
                        << coreState.delta << "ms";
                }
                else
                {
                    sstr
                        << std::fixed << std::setprecision(1)
                        << std::setw(4)
                        << (float(coreState.delta) / 1000) << "s ";
                }
#else
                if (coreState.delta < 1000)
                {
                    sstr
                        << std::setw(4)
                        << coreState.delta << "us";
                }
                else if (coreState.delta < 1000000)
                {
                    sstr
                        << std::fixed << std::setprecision(1)
                        << std::setw(4)
                        << (float(coreState.delta) / 1000) << "ms";
                }
                else
                {
                    sstr
                        << std::fixed << std::setprecision(1)
                        << std::setw(4)
                        << (float(coreState.delta) / 1000000) << "s";
                }
#endif

                sstr
                    << " "
                    << std::setw(2)
                    << coreState.genomesAlive << "car(s)"
                    << std::endl
                    << std::endl;
            }

            std::string str = sstr.str();

            // textRenderer.push({ 8, 8 + 15 * 16 }, str, 1.0f);
            textRenderer.push({ 16, 8 + 30 * 16 }, str, 1.0f);
        }
        //*/

        {
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
        }

        //
        // big titles

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

        // big titles
        //

        textRenderer.render();
    }

    /**/
    { // graphics

        auto& stackRenderer = graphic.stackRenderer;
        const auto& shader = *graphic.shaders.stackRenderer;

        shader.bind();

        const auto& hudMatrix = graphic.camera.matrices.hud;
        GLint composedMatrixLoc = shader.getUniform("u_composedMatrix");
        glUniformMatrix4fv(composedMatrixLoc, 1, false, glm::value_ptr(hudMatrix));

        const glm::vec3    whiteColor(1.0f, 1.0f, 1.0f);
        // const glm::vec3    redColor(0.75f, 0.0f, 0.0f);
        // const glm::vec3    greenColor(0.0f, 0.75f, 0.0f);

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
    }
    //*/

    /**/
    {
        auto&       stackRenderer = graphic.stackRenderer;
        const auto& shader = *graphic.shaders.stackRenderer;

        shader.bind();

        const auto& hudMatrix = graphic.camera.matrices.hud;
        GLint composedMatrixLoc = shader.getUniform("u_composedMatrix");
        glUniformMatrix4fv(composedMatrixLoc, 1, false, glm::value_ptr(hudMatrix));

        const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);
        // const glm::vec3 redColor(0.75f, 0.0f, 0.0f);
        // const glm::vec3 greenColor(0.0f, 0.75f, 0.0f);

        const glm::vec2 borderPos(10, 400);
        const glm::vec2 borderSize(150, 75);

        stackRenderer.pushRectangle(borderPos, borderSize, whiteColor);

        //
        // show progresses here
        {
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
                    // stackRenderer.pushLine(borderPos, borderPos + borderSize, redColor);
                }
            }

            // stackRenderer.pushLine(borderPos, borderPos + borderSize, redColor);
        }
        // show progresses here
        //

        stackRenderer.flush();
    }
    //*/

    glEnable(GL_DEPTH_TEST);
}
