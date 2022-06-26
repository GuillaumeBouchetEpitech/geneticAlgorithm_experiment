
#include "Scene.hpp"

#include "demo/states/StateManager.hpp"

#include "demo/logic/graphic/wrappers/ShaderProgram.hpp"

#include "demo/utilities/TraceLogger.hpp"

// #include <memory> // <= make_unique
#include <cstring> // <= std::memcpy

#include <iomanip>
#include <sstream>

namespace /*anonymous*/
{

    constexpr float thirdPViewportWidth = 175.0f;
    constexpr float thirdPViewportHeight = 150.0f;


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

    struct TopologyRenderer
    {
        glm::vec2 position;
        glm::vec2 size;

        void render()
        {
            auto& data = Data::get();
            auto& logic = data.logic;
            auto& graphic = data.graphic;

            const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);
            const glm::vec4 redColor(1.0f,0.0f,0.0f,0.85f);
            const glm::vec4 blueColor(0.5f,0.5f,1.0f,0.85f);

            graphic.stackRenderer.pushQuad(glm::vec3(position + size * 0.5f, -0.1f), size, glm::vec4(0,0,0, 0.75f));
            graphic.stackRenderer.pushRectangle(position, size, whiteColor);

            std::vector<unsigned int> topologyArray;
            topologyArray.push_back(logic.annTopology.getInput());
            for (const auto& hidden : logic.annTopology.getHiddens())
                topologyArray.push_back(hidden);
            topologyArray.push_back(logic.annTopology.getOutput());

            const NeuralNetworks& neuralNetworks = logic.simulation->getGeneticAlgorithm().getNeuralNetworks();

            std::vector<float> connectionsWeights;
            neuralNetworks[logic.leaderCar.index]->getWeights(connectionsWeights);

            struct NeuronData
            {
                glm::vec2 position;
                float value;
            };
            using Layer = std::vector<NeuronData>;

            std::vector<Layer> layersData;

            { // setup layersData container

                layersData.resize(topologyArray.size());

                std::vector<float> neuronsValues;
                neuralNetworks[logic.leaderCar.index]->getNeuronsValues(neuronsValues);

                int neuronIndex = 0;
                for (std::size_t ii = 0; ii < topologyArray.size(); ++ii)
                {
                    const unsigned int actualSize = topologyArray[ii];

                    glm::vec2 currPos = position;
                    currPos.y += size.y - size.y / topologyArray.size() * (float(ii) + 0.5f);

                    layersData[ii].reserve(actualSize); // pre-allocate
                    for (unsigned int jj = 0; jj < actualSize; ++jj)
                    {
                        currPos.x += size.x / (actualSize + 1);

                        const float neuronsValue = glm::clamp(neuronsValues[neuronIndex++], 0.0f, 1.0f);

                        layersData[ii].push_back({ currPos, neuronsValue });
                    }
                }

            } // setup layersData container

            { // show input/output values

                constexpr float valueHeight = 12.0f;;

                const auto& inputlayer = layersData.front();
                for (auto neuron : inputlayer)
                {
                    neuron.position.y += 3.0f;

                    const glm::vec2 start(neuron.position.x, neuron.position.y + valueHeight);
                    constexpr float thickness = 10.0f;
                    const glm::vec4 color = glm::mix(redColor, glm::vec4(0,1,0,1), neuron.value);

                    graphic.stackRenderer.pushThickTriangleLine(
                        start,
                        neuron.position,
                        thickness,
                        1.0f,
                        color,
                        +0.2f);
                }

                const auto& outputlayer = layersData.back();
                for (auto neuron : outputlayer)
                {
                    neuron.position.y -= 3.0f;

                    glm::vec2 start = glm::vec2(neuron.position.x, neuron.position.y - valueHeight);

                    if (neuron.value > 0.0f)
                    {
                        const float thickness = 2.0f + neuron.value * +10.0f;
                        graphic.stackRenderer.pushThickTriangleLine(
                            start,
                            neuron.position,
                            thickness,
                            12.0f,
                            redColor
                            +0.2f);
                    }
                    else
                    {
                        const float thickness = 2.0f + neuron.value * -10.0f;
                        graphic.stackRenderer.pushThickTriangleLine(
                            start,
                            neuron.position,
                            thickness,
                            12.0f,
                            blueColor,
                            +0.2f);
                    }
                }

            } // show input/output values

            { // draw neurons

                const glm::vec4 whiteColor(1,1,1,1);

                for (const auto& layer : layersData)
                {
                    for (const auto& neuron : layer)
                    {
                        const float coef = glm::clamp(neuron.value, 0.0f, 1.0f);
                        const float radius = 2.0f + coef * 2.0f;

                        graphic.stackRenderer.pushCircle(glm::vec3(neuron.position, +0.2f), radius, whiteColor);
                    }
                }

            } // draw neurons

            { // draw connections

                struct ValueRange
                {
                    float min;
                    float max;
                    float alpha;
                };

                std::array<ValueRange, 4> valueRanges
                {{
                    { 0.75f, 1.00f, 1.00f },
                    { 0.50f, 0.75f, 0.85f },
                    { 0.25f, 0.50f, 0.70f },
                    { 0.00f, 0.25f, 0.55f },
                }};

                for (ValueRange range : valueRanges)
                {
                    const glm::vec4 positiveColor = glm::vec4(glm::vec3(redColor), range.alpha);
                    const glm::vec4 negativeColor = glm::vec4(glm::vec3(blueColor), range.alpha);

                    int connectionIndex = 0;
                    for (std::size_t ii = 1; ii < layersData.size(); ++ii)
                    {
                        const auto& prevLayer = layersData[ii - 1];
                        const auto& currLayer = layersData[ii];

                        for (const auto& prevNeuron : prevLayer)
                        {
                            for (const auto& currNeuron : currLayer)
                            {
                                const float weight = connectionsWeights[connectionIndex++];

                                if (!(prevNeuron.value >= range.min && prevNeuron.value <= range.max))
                                    continue;

                                constexpr float initialThickness = 1.0f;
                                constexpr float extraThickness = 2.5f;

                                const float targetThickness = initialThickness + weight * (weight > 0.0f
                                    ? +extraThickness
                                    : -extraThickness);

                                const glm::vec4 targetColor (weight > 0.0f ? positiveColor : negativeColor);

                                graphic.stackRenderer.pushThickTriangleLine(
                                    prevNeuron.position,
                                    currNeuron.position,
                                    targetThickness,
                                    targetThickness,
                                    targetColor,
                                    +0.0f);
                            }
                        }
                    }
                }

            } // draw connections

        }
    };

    struct LeaderEyeRenderer
    {
        glm::vec2 position;
        glm::vec2 size;

        void render()
        {
            auto& data = Data::get();
            auto& logic = data.logic;
            auto& graphic = data.graphic;

            const auto& leader = logic.simulation->getCarResult(logic.leaderCar.index);

            const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);
            const glm::vec3 greenColor(0.0f, 1.0f, 0.0f);
            const glm::vec3 redColor(1.0f, 0.0f, 0.0f);

            graphic.stackRenderer.pushRectangle(position, size, whiteColor);

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
                    glm::vec2 currPos = position;

                    currPos.y += size.y;

                    currPos.x += size.x * ((float(jj) + 0.5f) / layerSize);
                    currPos.y -= size.y * ((float(ii) + 0.5f) / layerCount);

                    allPositions.push_back(currPos);
                }

            glm::vec2 eyeSize = {19, 19};

            for (std::size_t ii = 0; ii < allPositions.size(); ++ii)
            {
                const auto& position = allPositions[ii];

                graphic.stackRenderer.pushRectangle(position - eyeSize * 0.5f, eyeSize, whiteColor);

                glm::vec3 color = glm::mix(redColor, greenColor, leader.eyeSensors[ii].value);

                graphic.stackRenderer.pushQuad(position, eyeSize, glm::vec4(color, 1.0f));
            }
        }
    };

    struct BackGroundCylindersRenderer
    {
        static void render(const Camera& camera)
        {
            auto& data = Data::get();
            auto& graphic = data.graphic;
            const auto& shader = *graphic.shaders.simpleTexture;

            const auto& boundaries = data.logic.circuitAnimation.boundaries;

            for (std::size_t ii = 0; ii < graphic.textures.cylinders.size(); ++ii)
            {
                glm::mat4 model = glm::identity<glm::mat4>();

                model = glm::translate(model, boundaries.center);
                model = glm::translate(model, camera.getTarget());
                model = glm::rotate(model, graphic.cylinderAnimationTime * 0.1f * (ii + 1), glm::vec3(+1,0,0));

                const auto& camMatrices = camera.getSceneMatricsData();
                glm::mat4 composed = camMatrices.projection * camMatrices.view * model;

                shader.setUniform("u_composedMatrix", composed);

                graphic.textures.cylinders[ii].bind();
                graphic.geometries.ground.cylinders[ii].render();
            }
        }
    };

    struct CoreUsageRenderer
    {
        glm::vec2 position;
        glm::vec2 size;

        void renderWireframe()
        {

            auto& data = Data::get();
            auto& graphic = data.graphic;

            auto& stackRenderer = graphic.stackRenderer;

            const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);

            const glm::vec2 borderPos = { position.x, position.y + 16 + 8 };
            const glm::vec2 borderSize = { size.x, size.y - 16  * 3 };

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

                    const glm::vec2 coordA(0, borderSize.y * coef);
                    const glm::vec2 coordB(borderSize.x, borderSize.y * coef);

                    stackRenderer.pushLine(borderPos + coordA, borderPos + coordB, whiteColor);
                }

            } // dividers

            //
            //

            for (std::size_t core = 0; core < cores.statesHistory.size(); ++core)
            {
                const auto& stateHistory = cores.statesHistory[core];

                float widthStep = borderSize.x / Data::Logic::Cores::maxStateHistory;

                for (unsigned int ii = 0; ii + 1 < Data::Logic::Cores::maxStateHistory; ++ii)
                {
                    const unsigned int prevIndex = (ii + cores.currHistoryIndex) % Data::Logic::Cores::maxStateHistory;
                    const unsigned int currIndex = (ii + 1 + cores.currHistoryIndex) % Data::Logic::Cores::maxStateHistory;

                    const auto& prevState = stateHistory[prevIndex];
                    const auto& currState = stateHistory[currIndex];

                    //

                    const float prevRatio = float(prevState.delta) / maxVal;
                    const float currRatio = float(currState.delta) / maxVal;

                    const glm::vec2 prevCoord(ii * widthStep, borderSize.y * prevRatio);
                    const glm::vec2 currCoord((ii + 1) * widthStep, borderSize.y * currRatio);

                    stackRenderer.pushLine(borderPos + prevCoord, borderPos + currCoord, whiteColor);
                }
            }

            stackRenderer.flush();

        }

        void renderHudText()
        {

            auto& data = Data::get();
            auto& graphic = data.graphic;
            auto& logic = data.logic;
            auto& textRenderer = graphic.hudText.renderer;

            {
                std::stringstream sstr;

#if defined D_WEB_WEBWORKER_BUILD

                sstr << "WORKERS: " << logic.cores.statesData.size() << std::endl;

#else

                sstr << "THREADS: " << logic.cores.statesData.size() << std::endl;

#endif

                std::string str = sstr.str();
                textRenderer.push({ position.x, position.y }, str, glm::vec3(1), 1.0f);
            }

            {

                unsigned int globalDelta = 0;
                for (std::size_t ii = 0; ii < logic.cores.statesData.size(); ++ii)
                {
                    const auto& coreState = logic.cores.statesData[ii];

                    globalDelta += coreState.delta;
                }

                std::stringstream sstr;

                sstr << "Total: " << writeTime(globalDelta);

                std::string str = sstr.str();

                unsigned int commonMaxDelta = 0;
                for (auto& stateHistory : logic.cores.statesHistory)
                    for (auto& history : stateHistory)
                        commonMaxDelta = std::max(commonMaxDelta, history.delta);

                glm::vec3 textColor = commonMaxDelta > 16000 ? glm::vec3(1,0,0) : glm::vec3(1,1,1);

                textRenderer.push({ position.x, position.y - 16 }, str, textColor, 1.0f);
            }

        }
    };

    struct NewLeaderRenderer
    {
    private:
        bool _isVisible = false;
        glm::vec3 _screenCoord = { 0, 0, 0 };

    public:
        void compute()
        {
            auto& data = Data::get();
            auto& graphic = data.graphic;
            auto& logic = data.logic;

            const auto& simulation = *logic.simulation;

            if (logic.leaderCar.index >= 0 &&
                logic.leaderCar.totalTimeAsLeader < 1.0f)
            {
                const Camera& scene = graphic.camera.scene.instance;

                const auto& leaderCarData = simulation.getCarResult(logic.leaderCar.index);

                if (// we don't advertise a dead leader
                    leaderCarData.isAlive &&
                    // we don't advertise an early leader
                    leaderCarData.fitness > 5.0f &&
                    // we don't advertise a dying leader
                    leaderCarData.groundSensor.value < 0.5f)
                {
                    const glm::vec3 carPos = leaderCarData.liveTransforms.chassis * glm::vec4(0, 0, 0, 1);

                    const bool isVisible = scene.sceneToHudCoord(carPos, _screenCoord);

                    if (isVisible &&
                        // out of range?
                        _screenCoord.z < 1.0f)
                    {
                        _isVisible = true;
                    }
                }
            }
        }

        void renderWireframe()
        {
            if (!_isVisible)
                return;

            auto& data = Data::get();
            auto& graphic = data.graphic;

            glm::vec2 textPos = { _screenCoord.x + 50, _screenCoord.y + 50 };

            graphic.stackRenderer.pushLine(_screenCoord, textPos, {1, 1, 1});
        }

        void renderHudText()
        {
            if (!_isVisible)
                return;

            auto& data = Data::get();
            auto& graphic = data.graphic;
            auto& textRenderer = graphic.hudText.renderer;

            glm::vec2 textPos = { _screenCoord.x + 50, _screenCoord.y + 50 };

            textRenderer.push(textPos, "NEW\nLEADER", glm::vec3(1), 1.1f);
        }
    };

    float clamp(float value, float minVal, float maxVal)
    {
        return std::min(std::max(value, minVal), maxVal);
    }


};

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

        const Camera& scene = Data::get().graphic.camera.scene.instance;

        Scene::_renderFloor(scene);

        Scene::_renderWireframesGeometries(scene.getSceneMatricsData().composed, false); // circuit only
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
        const Camera& camInstance = camera.scene.instance;

        Scene::_renderFloor(camInstance);

        if (!logic.isAccelerated)
            Scene::_renderLeadingCarSensors(camInstance);

        Scene::_renderParticles(camInstance);
        Scene::_renderCars(camInstance);

        const glm::mat4& composed = camInstance.getSceneMatricsData().composed;

        Scene::_renderWireframesGeometries(composed);
        Scene::_renderAnimatedCircuit(composed);
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
    // auto& matrices = camera.matrices;

    { // scene

        // clamp vertical rotation [-70..+70]
        const float verticalLimit = glm::radians(70.0f);
        camera.scene.rotations.phi = glm::clamp(camera.scene.rotations.phi, -verticalLimit, verticalLimit);

        const float cosPhi = std::cos(camera.scene.rotations.phi);
        const glm::vec3 eye =
        {
            camera.scene.center.x + camera.scene.distance * cosPhi * std::cos(camera.scene.rotations.theta),
            camera.scene.center.y + camera.scene.distance * cosPhi * std::sin(camera.scene.rotations.theta),
            camera.scene.center.z + camera.scene.distance * std::sin(camera.scene.rotations.phi)
        };
        const glm::vec3 upAxis = { 0.0f, 0.0f, 1.0f };

        camera.scene.instance.setSize(camera.viewportSize.x, camera.viewportSize.y);
        camera.scene.instance.lookAt(eye, camera.scene.center, upAxis);
        camera.scene.instance.computeMatrices();

    } // scene

    { // third person

        if (logic.leaderCar.index >= 0)
        {
            const auto& carResult = simulation.getCarResult(logic.leaderCar.index);

            glm::vec3 carOrigin = carResult.liveTransforms.chassis * glm::vec4(0.0f, 0.0f, 2.5f, 1.0f);
            glm::vec3 carUpAxis = carResult.liveTransforms.chassis * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

            StateManager::States currentState = StateManager::get()->getState();

            if (// do not update the third person camera if not in a correct state
                (currentState == StateManager::States::Running ||
                 currentState == StateManager::States::StartGeneration) &&
                // do not update the third person camera if too close from the target
                glm::distance(carOrigin, camera.thirdPerson.eye) > 0.25f)
            {
                // simple lerp to setup the third person camera
                const float lerpRatio = 0.1f * 60.0f * elapsedTime;
                camera.thirdPerson.eye += (carOrigin - camera.thirdPerson.eye) * lerpRatio;
                camera.thirdPerson.upAxis += (carUpAxis - camera.thirdPerson.upAxis) * lerpRatio;
            }

            const glm::vec3 eye = camera.thirdPerson.eye;
            const glm::vec3 target = carOrigin;
            const glm::vec3 upAxis = camera.thirdPerson.upAxis;

            camera.thirdPerson.instance.setSize(thirdPViewportWidth, thirdPViewportHeight);
            camera.thirdPerson.instance.lookAt(eye, target, upAxis);
            camera.thirdPerson.instance.computeMatrices();
        }

    } // third person

    { // hud ortho

        // const auto& vSize = camera.viewportSize;

        // glm::mat4 projection = glm::ortho(0.0f, vSize.x, 0.0f, vSize.y, -1.0f, 1.0f);

        // glm::vec3 eye = { 0.0f, 0.0f, 0.5f };
        // glm::vec3 center = { 0.0f, 0.0f, 0.0f };
        // glm::vec3 upAxis = { 0.0f, 1.0f, 0.0f };
        // glm::mat4 viewMatrix = glm::lookAt(eye, center, upAxis);

        // matrices.hud_ortho = projection * viewMatrix;

    } // hud ortho

    { // hud_perspective

        // const auto& vSize = camera.viewportSize;

        {

            // const float fovy = glm::radians(70.0f);
            // const float aspectRatio = float(vSize.x) / vSize.y;
            // glm::mat4 projection = glm::perspective(fovy, aspectRatio, 1.0f, 1000.f);

            // const glm::vec2 halfViewportSize = vSize * 0.5f;
            // glm::vec3 eye = { halfViewportSize.x, halfViewportSize.y, 425.0f };
            // glm::vec3 center = { halfViewportSize.x, halfViewportSize.y, 0.0f };
            // glm::vec3 upAxis = { 0.0f, 1.0f, 0.0f };
            // glm::mat4 viewMatrix = glm::lookAt(eye, center, upAxis);

            // matrices.hud_perspective = projection * viewMatrix;

        }

        {

            // glm::mat4 projection = glm::ortho(0.0f, vSize.x, 0.0f, vSize.y, -1.0f, 1.0f);

            // glm::vec3 eye = { 0.0f, 0.0f, 0.5f };
            // glm::vec3 center = { 0.0f, 0.0f, 0.0f };
            // glm::vec3 upAxis = { 0.0f, 1.0f, 0.0f };
            // glm::mat4 viewMatrix = glm::lookAt(eye, center, upAxis);

            // matrices.hud_perspective = projection * viewMatrix;

        }

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

void Scene::_renderLeadingCarSensors(const Camera& camera)
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
    shader.setUniform("u_composedMatrix", camera.getSceneMatricsData().composed);


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

void Scene::_renderParticles(const Camera& camera)
{
    // instanced geometrie(s)

    const auto& graphic = Data::get().graphic;
    const auto& shader = *graphic.shaders.particles;
    const auto& geometry = graphic.geometries.particles.firework;

    shader.bind();
    shader.setUniform("u_composedMatrix", camera.getSceneMatricsData().composed);

    geometry.render();
}

void Scene::_renderFloor(const Camera& camera)
{
    const auto& data = Data::get();
    const auto& graphic = data.graphic;

    const auto& shader = *graphic.shaders.simpleTexture;

    shader.bind();

    // hide the floor if the camera is looking from beneath it

    glEnable(GL_CULL_FACE);
    // glDisable(GL_CULL_FACE);

    glDisable(GL_DEPTH_TEST);

    BackGroundCylindersRenderer backGroundCylindersRndr;
    backGroundCylindersRndr.render(camera);

    {
        shader.setUniform("u_composedMatrix", camera.getSceneMatricsData().composed);

        graphic.textures.chessboard.bind();

        graphic.geometries.ground.chessboard.render();
    }

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void Scene::_renderCars(const Camera& camera)
{
    // instanced geometrie(s)

    auto& data = Data::get();
    auto& graphic = data.graphic;
    const auto& logic = data.logic;
    const auto& simulation = *logic.simulation;

    graphic.shaders.model->bind();
    graphic.shaders.model->setUniform("u_composedMatrix", camera.getSceneMatricsData().composed);
    // graphic.shaders.model->setUniform("u_projectionMatrix", camera.getSceneMatricsData().projection);
    // graphic.shaders.model->setUniform("u_modelViewMatrix", camera.getSceneMatricsData().view);

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

        const glm::mat4 chassisTransform = glm::translate(carData.liveTransforms.chassis, modelHeight);
        const glm::vec4 carOrigin = chassisTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        if (!camera.getFrustumCulling().sphereInFrustum(carOrigin, 5.0f))
            continue;

        //
        // color

        const bool isLeader = (logic.leaderCar.index == int(ii));
        const glm::vec3 targetColor = isLeader ? leaderColor : lifeColor;
        const glm::vec3 color = glm::mix(deathColor, targetColor, carData.life);

        //
        // transforms

        modelsChassisMatrices.emplace_back(chassisTransform, color);
        for (const auto& wheelTransform : carData.liveTransforms.wheels)
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

    const GLint alphaLoc = shader.getUniform("u_alpha");
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

    NewLeaderRenderer newLeaderRndr;
    newLeaderRndr.compute();

    CoreUsageRenderer coreUsageRndr;
    coreUsageRndr.position = { 8, 4 * 16 + 7 };
    coreUsageRndr.size = { 150, 100 };

    { // texts

        const auto& shader = *graphic.shaders.hudText;
        auto&       textRenderer = graphic.hudText.renderer;
        const auto& simulation = *logic.simulation;
        const auto& hudText = logic.hudText;

        shader.bind();
        shader.setUniform("u_composedMatrix", graphic.camera.scene.instance.getHudMatricsData().composed);

        graphic.textures.textFont.bind();

        textRenderer.clear();

        const auto& vSize = graphic.camera.viewportSize;

        { // top-left header text

            textRenderer.push({ 8, vSize.y - 16 - 8 }, hudText.header, glm::vec3(1), 1.0f);

        } // top-left header text

        { // top-left performance stats

            std::stringstream sstr;

            sstr
                << "Update: " << writeTime(logic.metrics.updateTime) << std::endl
                << "Render: " << writeTime(logic.metrics.renderTime);

            const std::string str = sstr.str();

            textRenderer.push({ 8, vSize.y - 5 * 16 - 8 }, str,  glm::vec3(1), 1.0f);

        } // top-left performance stats

        { // bottom-left text

            const unsigned int totalCars = logic.cores.totalCars;
            unsigned int carsLeft = 0;
            float localBestFitness = 0.0f;

            for (unsigned int ii = 0; ii < totalCars; ++ii)
            {
                const auto& carData = simulation.getCarResult(ii);

                if (carData.isAlive)
                    ++carsLeft;

                if (localBestFitness < carData.fitness)
                    localBestFitness = carData.fitness;
            }

            {
                std::stringstream sstr;

                sstr << "Generation: " << simulation.getGenerationNumber();

                const std::string str = sstr.str();

                textRenderer.push({ 8, 8 + 2 * 16 }, str, glm::vec3(1), 1.0f);
            }

            {
                std::stringstream sstr;

                sstr
                    << std::fixed << std::setprecision(1)
                    << "Fitness: " << localBestFitness << "/" << simulation.getBestGenome().fitness;

                const std::string str = sstr.str();

                const float bestFitness = simulation.getBestGenome().fitness;
                const float coef = bestFitness == 0.0f
                    ? 0.0f
                    : clamp(localBestFitness / bestFitness, 0.0f, 1.0f);

                const glm::vec3 textColor = glm::mix(glm::vec3(1,0,0), glm::vec3(0,1,0), coef);

                textRenderer.push({ 8, 8 + 1 * 16 }, str, textColor, 1.0f);
            }

            {
                std::stringstream sstr;

                sstr
                    << std::fixed << std::setprecision(1)
                    << "Cars: " << carsLeft << "/" << totalCars;

                const std::string str = sstr.str();

                const float coef = 1.0f - clamp(float(carsLeft) / totalCars, 0.0f, 1.0f);

                const glm::vec3 textColor = glm::mix(glm::vec3(0,1,0), glm::vec3(1,0,0), coef);

                textRenderer.push({ 8, 8 + 0 * 16 }, str, textColor, 1.0f);
            }

            // {
            //     std::stringstream sstr;

            //     sstr
            //         << "Generation: " << simulation.getGenerationNumber() << std::endl
            //         << std::fixed << std::setprecision(1)
            //         << "Fitness: " << localBestFitness << "/" << simulation.getBestGenome().fitness << std::endl
            //         << "Cars: " << carsLeft << "/" << totalCars;

            //     const std::string str = sstr.str();

            //     textRenderer.push({ 8, 8 + 2 * 16 }, str, glm::vec3(1), 1.0f);
            // }

        } // bottom-left text

        { // advertise a new leader

            newLeaderRndr.renderHudText();

        } // advertise a new leader

        { // show cores status

            coreUsageRndr.renderHudText();

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

                textRenderer.push({ vSize.x * 0.5f - 5 * 16 * scale, vSize.y * 0.5f - 8 * scale }, message, glm::vec3(1), scale);
            }
#endif

            if (currentState == StateManager::States::Paused)
            {
                float scale = 5.0f;

                std::string message = "PAUSED";

                textRenderer.push({ vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale, vSize.y * 0.5f - 8 * scale }, message, glm::vec3(1), scale);
            }

            if (currentState == StateManager::States::StartGeneration)
            {
                {
                    const float scale = 3.0f;

                    std::stringstream sstr;
                    sstr << "Generation: " << simulation.getGenerationNumber();
                    const std::string message = sstr.str();

                    textRenderer.push({ vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale, vSize.y * 0.5f + 16 * scale }, message, glm::vec3(1), scale);
                }

                {
                    const float scale = 2.0f;

                    const std::size_t totalStats = logic.fitnessStats.allStats.size();
                    const float prevFitness = logic.fitnessStats.allStats[totalStats - 2];
                    const float currFitness = logic.fitnessStats.allStats[totalStats - 1];

                    if (currFitness > 0.0f)
                    {
                        {
                            std::stringstream sstr;
                            sstr << "Fitness: " << std::fixed << std::setprecision(1) << currFitness;
                            std::string message = sstr.str();

                            textRenderer.push({ vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale, vSize.y * 0.5f + 0 * scale }, message, glm::vec3(1), scale);
                        }

                        if (currFitness != prevFitness)
                        {
                            std::stringstream sstr;

                            if (currFitness > prevFitness)
                                sstr << "Smarter result (+" << std::fixed << std::setprecision(1) << (currFitness - prevFitness) << ")";
                            else if (currFitness < prevFitness)
                                sstr << "Worse result (" << std::fixed << std::setprecision(1) << (currFitness - prevFitness) << ")";

                            const std::string message = sstr.str();

                            textRenderer.push({ vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale, vSize.y * 0.5f - 16 * scale }, message, glm::vec3(1), scale);
                        }
                    }
                }
            }

        } // big titles

        textRenderer.render();

    } // texts

    { // wireframes

        const auto& shader = *graphic.shaders.stackRenderer;

        shader.bind();
        shader.setUniform("u_composedMatrix", graphic.camera.scene.instance.getHudMatricsData().composed);

        { // cores history graphics

            coreUsageRndr.renderWireframe();

        } // cores history graphics

        { // advertise a new leader

            newLeaderRndr.renderWireframe();

        } // advertise a new leader

        auto& stackRenderer = graphic.stackRenderer;

        { // progresses curve

            const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);

            const glm::vec2 borderPos(8, 160);
            const glm::vec2 borderSize(150, 75);

            stackRenderer.pushQuad(glm::vec3(borderPos + borderSize * 0.5f, -0.1f), borderSize, glm::vec4(0,0,0, 0.75f));
            stackRenderer.pushRectangle(borderPos, borderSize, whiteColor);

            const auto& allStats = logic.fitnessStats.allStats;

            float maxFitness = 0.0f;
            for (float stat : allStats)
                maxFitness = std::max(maxFitness, stat);

            float stepWidth = borderSize.x / (allStats.size() - 1);

            for (std::size_t ii = 1; ii < allStats.size(); ++ii)
            {
                const float prevData = allStats[ii - 1];
                const float currData = allStats[ii];

                const glm::vec2 prevPos =
                {
                    stepWidth * (ii - 1),
                    (prevData / maxFitness) * borderSize.y,
                };
                const glm::vec2 currPos =
                {
                    stepWidth * ii,
                    (currData / maxFitness) * borderSize.y,
                };

                stackRenderer.pushLine(borderPos + prevPos, borderPos + currPos, whiteColor);
            }

        } // progresses curve

        if (logic.leaderCar.index >= 0)
        {
            const auto& vSize = graphic.camera.viewportSize;

            { // topology

                TopologyRenderer topologyRndr;
                topologyRndr.size = glm::vec2(150, 125);
                topologyRndr.position = glm::vec2(vSize.x - topologyRndr.size.x - 10, 170);
                topologyRndr.render();

            } // topology

            { // leader's eye

                LeaderEyeRenderer leaderEyeRndr;
                leaderEyeRndr.size = glm::vec2(100, 60);
                leaderEyeRndr.position = glm::vec2(vSize.x - leaderEyeRndr.size.x - 10, 305);
                leaderEyeRndr.render();

            } // leader's eye
        }

        stackRenderer.flush();

    } // wireframes

    // glEnable(GL_DEPTH_TEST);
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

    const auto& viewportSize = camera.viewportSize;
    const glm::vec2 thirdPViewportPos = { viewportSize.x - thirdPViewportWidth - 10, 10 };

    glEnable(GL_SCISSOR_TEST);
    glScissor(thirdPViewportPos.x, thirdPViewportPos.y, thirdPViewportWidth, thirdPViewportHeight);

    glViewport(thirdPViewportPos.x, thirdPViewportPos.y, thirdPViewportWidth, thirdPViewportHeight);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const Camera& camInstance = camera.thirdPerson.instance;
    const glm::mat4& composed = camInstance.getSceneMatricsData().composed;

    Scene::_renderFloor(camInstance);
    Scene::_renderLeadingCarSensors(camInstance);
    Scene::_renderParticles(camInstance);
    Scene::_renderCars(camInstance);
    Scene::_renderWireframesGeometries(composed);
    Scene::_renderAnimatedCircuit(composed);

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
        // shader.setUniform("u_composedMatrix", graphic.camera.matrices.hud_perspective);
        shader.setUniform("u_composedMatrix", graphic.camera.scene.instance.getHudMatricsData().composed);

        glDisable(GL_DEPTH_TEST);

        graphic.hudComponents.colorTexture.bind();

        graphic.geometries.hudPerspective.geometry.render();

        glEnable(GL_DEPTH_TEST);

    } // render framebuffer texture in curved geometry
}
