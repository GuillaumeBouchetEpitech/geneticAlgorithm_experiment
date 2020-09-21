
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

#include "sounds/OpenALSoundManager.hpp"

#include "demo/simulation/AbstactSimulation.hpp"

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
    void initialiseSounds();
    void initialiseCircuit();
    void initialiseSimulationCallbacks();

public:

    struct t_graphic
    {
        struct t_cameraData
        {
            glm::vec2 viewportSize = { 800.0f, 600.0f };

            // glm::vec2 rotations = { -2.5f, 0.5f };
            struct t_rotations
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

            struct t_matricesData
            {
                struct t_matrices
                {
                    glm::mat4 projection;
                    glm::mat4 model;
                    glm::mat4 view;
                    glm::mat4 composed;
                };

                t_matrices scene;
                t_matrices thirdPerson;

                glm::mat4 hud;
            }
            matrices;

            FrustumCulling frustumCulling;
        }
        camera;

        struct t_shaders
        {
            std::unique_ptr<Shader> stackRenderer;
            std::unique_ptr<Shader> wireframes;
            std::unique_ptr<Shader> animatedCircuit;
            std::unique_ptr<Shader> hudText;
            std::unique_ptr<Shader> particles;
            std::unique_ptr<Shader> model;
        }
        shaders;

        struct t_textures
        {
            Texture textFont;
        }
        textures;

        struct t_geometries
        {
            struct t_particles
            {
                Geometry firework;
            }
            particles;

            struct t_stackRenderer
            {
                Geometry lines;
                Geometry triangles;
            }
            stackRenderer;

            struct t_wireframes
            {
                Geometry circuitSkelton;

                struct t_wheelsTrail
                {
                    std::array<Geometry, 4> wheels;
                };

                std::array<t_wheelsTrail, 5> bestNewCarsTrails;

                Geometry leaderCarTrail;
            }
            wireframes;

            struct t_animatedCircuit
            {
                Geometry ground;
                Geometry walls;
            }
            animatedCircuit;

            struct t_hudText
            {
                Geometry letters;
            }
            hudText;

            struct t_model
            {
                Geometry car;
                Geometry wheel;
            }
            model;
        }
        geometries;

        StackRenderer stackRenderer;

        ParticleManager particleManager;

        struct t_hudText
        {
            const glm::vec2 textureSize = { 256, 256 };
            const glm::vec2 gridSize = { 16, 16 };

            TextRenderer renderer;
        }
        hudText;
    }
    graphic;

    //

    struct t_sound
    {
        OpenALSoundManager manager;
    }
    sounds;

    //

    struct t_logic
    {
        struct t_state
        {
            StateManager::States previousState;
            int countdown = 0;
        }
        state;

        struct t_metrics
        {
            unsigned int updateTime = 0;
            unsigned int renderTime = 0;
        }
        metrics;

        NeuralNetworkTopology annTopology;

        std::unique_ptr<AbstactSimulation> simulation = nullptr;

        struct t_cores
        {
            typedef std::vector<AbstactSimulation::t_coreState> t_statesData;
            typedef std::vector<t_statesData> t_statesHistory;

            const unsigned int maxStateHistory = 60;
            unsigned int currHistoryIndex = 0;
            t_statesData statesData;
            t_statesHistory statesHistory;

            unsigned int genomesPerCore = 0;
            unsigned int totalCores = 0;
            unsigned int totalCars = 0;
        }
        cores;

        bool isPaused = false;
        bool isAccelerated = false;

        struct t_leaderCarData
        {
            float timeoutUntilNewLeader = 0;
            int index = -1;
            float totalTimeAsLeader = 0;
        }
        leaderCar;

        struct t_carsTrails
        {
            std::unordered_map<unsigned int, unsigned int> genomeIndexMap;

            struct t_wheelsTrail
            {
                std::array<std::vector<glm::vec3>, 4> wheels;
            };
            std::vector<t_wheelsTrail> allWheelsTrails;

            unsigned int currentTrailIndex = 0;
        }
        carsTrails;

        struct t_circuitAnimation
        {
            float targetValue = 0.0f;
            float lowerValue = 0.0f;
            float upperValue = 0.0f;
            float maxUpperValue = 0.0f;
            int maxPrimitiveCount = 0;

            struct t_boundaries
            {
                glm::vec3 min;
                glm::vec3 max;
                glm::vec3 center;
            }
            boundaries;
        }
        circuitAnimation;

        struct t_hudText
        {
            std::string header;
            std::string pthreadWarning;
        }
        hudText;

        struct t_fitnessStats
        {
            constexpr static unsigned int maxStats = 10;
            std::vector<float> allStats;
        }
        fitnessStats;
    }
    logic;

    struct t_input
    {
        std::unordered_map<int, bool> keys;

        struct t_mouse
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
