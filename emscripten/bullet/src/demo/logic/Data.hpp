
#pragma once

#include "thirdparty/OpenGLES.hpp"
#include "thirdparty/GLMath.hpp"

#include "graphic/utilities/StackRenderer.hpp"
#include "graphic/utilities/TextRenderer.hpp"
#include "graphic/utilities/ParticleManager.hpp"
#include "graphic/utilities/FrustumCulling.hpp"

#include "graphic/wrappers/Geometry.hpp"
#include "graphic/wrappers/Texture.hpp"
#include "graphic/wrappers/Shader.hpp"

#include "demo/logic/simulation/AbstactSimulation.hpp"

#include "demo/states/StateManager.hpp"

#include "demo/utilities/NonCopyable.hpp"

#include <string>
#include <list>
#include <unordered_map>
#include <array>
#include <memory> // <= unique_ptr / make_unique

class Data
    : public NonCopyable
{

    //
    //
    // singleton

private:
    static Data* _instance;

    Data() = default;
    ~Data();
private:
    void initialise();

public:
    static void create();
    static void destroy();
    static Data& get();

    // singleton
    //
    //

private:
    void initialiseShaders();
    void initialiseGeometries();
    // void initialiseSounds();
    void initialiseCircuit();
    void initialiseSimulationCallbacks();

public:

    struct Graphic
    {
        struct CameraData
        {
            glm::vec2 viewportSize = { 800.0f, 600.0f };

            // glm::vec2 rotations = { -2.5f, 0.5f };
            struct Rotations
            {
                float theta = -2.5f;
                float phi = 0.5f;
            }
            rotations;

            glm::vec3 center = { 0.0f, 0.0f, 0.0f };
            float distance = 0.0f;

            glm::vec3 eye = { 0.0f, 0.0f, 0.0f };
            glm::vec3 front = { 1.0f, 0.0f, 0.0f };

            glm::vec3 thirdPersonCenter = { 0.0f, 0.0f, 0.0f };
            glm::vec3 thirdPersonUpAxis = { 0.0f, 0.0f, 1.0f };

            struct MatricesData
            {
                struct Matrices
                {
                    glm::mat4 projection;
                    glm::mat4 model;
                    glm::mat4 view;
                    glm::mat4 composed;
                };

                Matrices scene;
                Matrices thirdPerson;

                glm::mat4 hud;
            }
            matrices;

            FrustumCulling frustumCulling;
        }
        camera;

        struct Shaders
        {
            std::unique_ptr<Shader> stackRenderer;
            std::unique_ptr<Shader> wireframes;
            std::unique_ptr<Shader> animatedCircuit;
            std::unique_ptr<Shader> hudText;
            std::unique_ptr<Shader> particles;
            std::unique_ptr<Shader> model;
        }
        shaders;

        struct Textures
        {
            Texture textFont;
        }
        textures;

        struct Geometries
        {
            struct Particles
            {
                Geometry firework;
            }
            particles;

            struct StackRenderer
            {
                Geometry lines;
                Geometry triangles;
            }
            stackRenderer;

            struct Wireframes
            {
                Geometry circuitSkelton;

                struct WheelsTrail
                {
                    std::array<Geometry, 4> wheels;
                };

                std::array<WheelsTrail, 5> bestNewCarsTrails;

                Geometry leaderCarTrail;
            }
            wireframes;

            struct AnimatedCircuit
            {
                Geometry ground;
                Geometry walls;
            }
            animatedCircuit;

            struct HudText
            {
                Geometry letters;
            }
            hudText;

            struct Model
            {
                Geometry car;
                Geometry wheel;
            }
            model;
        }
        geometries;

        StackRenderer stackRenderer;

        ParticleManager particleManager;

        struct HudText
        {
            const glm::vec2 textureSize = { 256, 256 };
            const glm::vec2 gridSize = { 16, 16 };

            TextRenderer renderer;
        }
        hudText;
    }
    graphic;

    //

    struct Logic
    {
        struct State
        {
            StateManager::States previousState;
            int countdown = 0;
        }
        state;

        struct Metrics
        {
            unsigned int updateTime = 0;
            unsigned int renderTime = 0;
        }
        metrics;

        NeuralNetworkTopology annTopology;

        std::unique_ptr<AbstactSimulation> simulation = nullptr;

        struct Cores
        {
            using StatesData = std::vector<AbstactSimulation::CoreState> ;
            using StatesHistory = std::vector<StatesData>;

            const unsigned int maxStateHistory = 60;
            unsigned int currHistoryIndex = 0;
            StatesData statesData;
            StatesHistory statesHistory;

            unsigned int genomesPerCore = 0;
            unsigned int totalCores = 0;
            unsigned int totalCars = 0;
        }
        cores;

        bool isAccelerated = false;

        struct LeaderCarData
        {
            float timeoutUntilNewLeader = 0;
            int index = -1;
            float totalTimeAsLeader = 0;
        }
        leaderCar;

        struct CarsTrails
        {
            std::unordered_map<unsigned int, unsigned int> genomeIndexMap;

            struct WheelsTrail
            {
                std::array<std::vector<glm::vec3>, 4> wheels;
            };
            std::vector<WheelsTrail> allWheelsTrails;

            unsigned int currentTrailIndex = 0;
        }
        carsTrails;

        struct CircuitAnimation
        {
            float targetValue = 0.0f;
            float lowerValue = 0.0f;
            float upperValue = 0.0f;
            float maxUpperValue = 0.0f;
            int maxPrimitiveCount = 0;

            struct Boundaries
            {
                glm::vec3 min;
                glm::vec3 max;
                glm::vec3 center;
            }
            boundaries;
        }
        circuitAnimation;

        struct HudText
        {
            std::string header;
            std::string pthreadWarning;
        }
        hudText;

        struct FitnessStats
        {
            constexpr static unsigned int maxStats = 10;
            std::vector<float> allStats;
        }
        fitnessStats;
    }
    logic;

    struct Input
    {
        std::unordered_map<int, bool> keys;

        struct Mouse
        {
            glm::ivec2 position = {0, 0};
            glm::ivec2 delta = {0, 0};
            bool tracking = false;
            bool wasTracking = false;
        }
        mouse;
    }
    inputs;
};
