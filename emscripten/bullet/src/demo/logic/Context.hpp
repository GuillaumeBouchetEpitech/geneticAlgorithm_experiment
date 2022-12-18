
#pragma once

#include "demo/logic/simulation/AbstactSimulation.hpp"
#include "demo/states/StateManager.hpp"

#include "graphic/renderers/TrianglesStackRenderer.hpp"
#include "graphic/renderers/WireframesStackRenderer.hpp"

#include "helpers/CarWheelsTrails.hpp"
#include "helpers/FitnessStats.hpp"
#include "helpers/LeaderCar.hpp"
#include "helpers/ProfileData.hpp"

#include "graphic/postProcess/PostProcess.hpp"
#include "graphic/renderers/hud/AnimationManager.hpp"
#include "graphic/renderers/hud/CoreUsageRenderer.hpp"
#include "graphic/renderers/hud/TextRenderer.hpp"
#include "graphic/renderers/hud/ThirdPersonCamera.hpp"
#include "graphic/renderers/hud/TopologyRenderer.hpp"

#include "graphic/renderers/scene/AnimatedCircuitRenderer.hpp"
#include "graphic/renderers/scene/BackGroundTorusRenderer.hpp"
#include "graphic/renderers/scene/CarTailsRenderer.hpp"
#include "graphic/renderers/scene/FlockingManager.hpp"
#include "graphic/renderers/scene/FloorRenderer.hpp"
#include "graphic/renderers/scene/ModelsRenderer.hpp"
#include "graphic/renderers/scene/ParticleManager.hpp"

#include "framework/NonCopyable.hpp"
#include "framework/graphic/ResourceManager.hpp"
#include "framework/graphic/camera/Camera.hpp"
#include "framework/helpers/GLMath.hpp"

#include <array>
#include <list>
#include <memory> // <= unique_ptr / make_unique
#include <string>
#include <unordered_map>

class Context : public NonCopyable {

  //
  //
  // singleton

private:
  static Context* _instance;

  Context() = default;
  ~Context();

private:
  void initialise(unsigned int width, unsigned int height,
                  unsigned int totalCores, unsigned int genomesPerCore);

public:
  static void create(unsigned int width, unsigned int height,
                     unsigned int totalCores, unsigned int genomesPerCore);
  static void destroy();
  static Context& get();

  // singleton
  //
  //

private:
  void initialiseGraphicResource();
  void initialiseSimulation(unsigned int totalCores,
                            unsigned int genomesPerCore);
  void initialiseSimulationCallbacks();

public:
  struct Graphic {
    // TODO: move in a class
    struct CameraData {
      glm::vec2 viewportSize = {800.0f, 600.0f};

      struct SceneData {
        struct Rotations {
          float theta = -2.5f;
          float phi = 0.5f;
        } rotations;

        glm::vec3 center = {0.0f, 0.0f, 0.0f};
        float distance = 0.0f;

        Camera scene;
        Camera hud;
      } main;

      struct ThirdPersonData {
        glm::vec3 eye = {0.0f, 0.0f, 0.0f};
        glm::vec3 upAxis = {0.0f, 0.0f, 1.0f};

        Camera scene;
      } thirdPerson;
    } camera;

    struct StackRenderers {
      WireframesStackRenderer wireframes;
      TrianglesStackRenderer triangles;
    };

    struct Hud {
      StackRenderers stackRenderers;
      TextRenderer textRenderer;
      TopologyRenderer topologyRenderer;
      CoreUsageRenderer coreUsageRenderer;
      ThirdPersonCamera thirdPersonCamera;
      AnimationManager animationManager;
      PostProcess postProcess;
    } hud;

    struct Scene {
      StackRenderers stackRenderers;
      ParticleManager particleManager;
      FloorRenderer floorRenderer;
      BackGroundTorusRenderer backGroundTorusRenderer;
      AnimatedCircuitRenderer animatedCircuitRenderer;
      ModelsRenderer modelsRenderer;
      FlockingManager flockingManager;
      CarTailsRenderer carTailsRenderer;
    } scene;

    ResourceManager resourceManager;
  } graphic;

  //

  struct Logic {
    struct Metrics {
      unsigned int updateTime = 0;
      unsigned int renderTime = 0;
    } metrics;

    NeuralNetworkTopology annTopology;

    std::unique_ptr<AbstactSimulation> simulation = nullptr;

    struct Cores {
      ProfileData profileData;

      unsigned int genomesPerCore = 0;
      unsigned int totalCores = 0;
      unsigned int totalCars = 0;
    } cores;

    bool isAccelerated = false;
    float timeSinceLastFrame = 0.0f;

    LeaderCar leaderCar;

    CarWheelsTrails carWheelsTrails;

    struct CircuitDimension {
      glm::vec3 min;
      glm::vec3 max;
      glm::vec3 center;
    } circuitDimension;

    struct HudText {
      std::string header;
    } hudText;

    FitnessStats fitnessStats;
  } logic;

  struct Input {
    std::unordered_map<int, bool> keys;

    struct Mouse {
      glm::vec2 position = {0, 0};
      glm::vec2 delta = {0, 0};
      bool tracking = false;
    } mouse;
  } inputs;
};
