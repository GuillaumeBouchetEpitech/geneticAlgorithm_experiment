
#include "Context.hpp"

#include "framework/system/math/RandomNumberGenerator.hpp"

#include <limits> // <= std::numeric_limits<T>::max()

void
Context::initialiseSimulation(
  unsigned int totalCores, unsigned int genomesPerCore) {
  std::vector<glm::vec3> skeletonVertices;
  AnimatedCircuitRenderer::AnimatedVertices groundVertices;
  AnimatedCircuitRenderer::AnimatedVertices wallsVertices;

  // pre-allocattions
  skeletonVertices.reserve(1024);
  groundVertices.reserve(4 * 1024);
  wallsVertices.reserve(
    groundVertices.capacity() * 2); // walls twice bigger that ground geometry

  const glm::vec3 greyColor = {0.7f, 0.7f, 0.7f};
  const glm::vec3 whiteColor = {1.0f, 1.0f, 1.0f};

  const float maxFloat = std::numeric_limits<float>::max();
  auto& circuitDimension = logic.circuitDimension;
  circuitDimension.min = glm::vec3(+maxFloat, +maxFloat, +maxFloat);
  circuitDimension.max = glm::vec3(-maxFloat, -maxFloat, -maxFloat);

  auto onSkeletonPatch = [&circuitDimension, &skeletonVertices](
                           const CircuitBuilder::Vec3Array& vertices,
                           const CircuitBuilder::Indices& indices) -> void {
    for (int index : indices) {
      const glm::vec3& vertex = vertices.at(index);

      skeletonVertices.push_back(vertex);

      circuitDimension.min.x = std::min(circuitDimension.min.x, vertex.x);
      circuitDimension.min.y = std::min(circuitDimension.min.y, vertex.y);
      circuitDimension.min.z = std::min(circuitDimension.min.z, vertex.z);
      circuitDimension.max.x = std::max(circuitDimension.max.x, vertex.x);
      circuitDimension.max.y = std::max(circuitDimension.max.y, vertex.y);
      circuitDimension.max.z = std::max(circuitDimension.max.z, vertex.z);
    }
  };

  float latestSize = 0;
  float maxUpperValue = 0.0f;
  constexpr float k_maxDeformation = 0.5f;

  auto onGroundPatchCallback =
    [&latestSize, &whiteColor, &groundVertices, &maxUpperValue](
      const CircuitBuilder::Vec3Array& vertices,
      const CircuitBuilder::Vec3Array& colors,
      const CircuitBuilder::Vec3Array& normals,
      const CircuitBuilder::Indices& indices) -> void {
    // save it for "onWallPatch" bellow
    latestSize = float(groundVertices.size());

    float limitValue = latestSize / indices.size();
    const float limitStep = 1.0f / indices.size();

    for (int index : indices) {
      const bool firstLine = (index < 2); // hacky

      const auto& color = (firstLine ? whiteColor : colors.at(index));

      const glm::vec3 deformation = {
        RNG::getRangedValue(-k_maxDeformation, +k_maxDeformation),
        RNG::getRangedValue(-k_maxDeformation, +k_maxDeformation),
        RNG::getRangedValue(-k_maxDeformation, +k_maxDeformation)};

      const glm::vec3 animNormal = (normals.at(index) + deformation) * 4.0f;

      groundVertices.emplace_back(
        vertices.at(index), color, normals.at(index), animNormal, limitValue);

      limitValue += limitStep;
    }

    maxUpperValue += 1.0f;
  };

  auto onWallPatchCallback = [&latestSize, &whiteColor, &greyColor,
                              &wallsVertices](
                               const CircuitBuilder::Vec3Array& vertices,
                               const CircuitBuilder::Vec3Array& colors,
                               const CircuitBuilder::Vec3Array& normals,
                               const CircuitBuilder::Indices& indices) -> void {
    static_cast<void>(colors); // <= unused

    float limitValue = latestSize / indices.size();
    const float limitStep = 1.0f / indices.size();

    for (int index : indices) {
      const bool firstLine = (index < 2); // hacky

      const auto& color = (firstLine ? whiteColor : greyColor);

      const glm::vec3 deformation = {
        RNG::getRangedValue(-k_maxDeformation, +k_maxDeformation),
        RNG::getRangedValue(-k_maxDeformation, +k_maxDeformation),
        RNG::getRangedValue(-k_maxDeformation, +k_maxDeformation)};

      const glm::vec3 animNormal = (normals.at(index) + deformation) * 4.0f;

      wallsVertices.emplace_back(
        vertices.at(index), color, normals.at(index), animNormal, limitValue);

      limitValue += limitStep;
    }
  };

  logic.annTopology.init({16, 5, 2}, /*useBiasNeuron =*/true);

  logic.cores.genomesPerCore = genomesPerCore;
  logic.cores.totalCores = totalCores;
  logic.cores.totalCars = logic.cores.totalCores * logic.cores.genomesPerCore;

  AbstactSimulation::Definition simulationDef;
  simulationDef.filename = "assets/circuits/default.txt";
  simulationDef.genomesPerCore = logic.cores.genomesPerCore;
  simulationDef.totalCores = logic.cores.totalCores;
  simulationDef.neuralNetworkTopology = logic.annTopology;
  simulationDef.onSkeletonPatch = onSkeletonPatch;        // callback
  simulationDef.onNewGroundPatch = onGroundPatchCallback; // callback
  simulationDef.onNewWallPatch = onWallPatchCallback;     // callback

  logic.simulation->initialise(simulationDef);

  //
  // the callbacks have now been called

  logic.cores.profileData.initialise(simulationDef.totalCores, 60);

  circuitDimension.center =
    circuitDimension.min + (circuitDimension.max - circuitDimension.min) * 0.5f;

  // graphic.camera.center = logic.simulation->getStartPosition();
  graphic.camera.center = {0, 0, 0};
  graphic.camera.distance = 200.0f;

  graphic.scene.animatedCircuitRenderer.initialise(
    skeletonVertices, groundVertices, wallsVertices, maxUpperValue);
}
