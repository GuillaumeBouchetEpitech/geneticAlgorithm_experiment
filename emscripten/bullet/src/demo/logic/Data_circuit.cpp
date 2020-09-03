
#include "Data.hpp"

#include "demo/utilities/math/RandomNumberGenerator.hpp"

#include <limits> // <= std::numeric_limits<T>::max()

struct t_animatedVertex
{
    glm::vec3 postion;
    glm::vec3 color;
    glm::vec3 normal; // <= animation
    float limitId; // <= animation
};
typedef std::vector<t_animatedVertex> t_animatedVertices;

void Data::initialiseCircuit()
{
    std::vector<glm::vec3>  skeletonVertices;
    t_animatedVertices      groundVertices;
    t_animatedVertices      wallsVertices;

    skeletonVertices.reserve(128); // pre-allocate
    groundVertices.reserve(2048); // pre-allocate
    wallsVertices.reserve(groundVertices.capacity() * 2); // pre-allocate, <= 1 ground 2 walls

     const glm::vec3    greyColor = { 0.6f, 0.6f, 0.6f };
    const glm::vec3     whiteColor = { 1.0f, 1.0f, 1.0f };

    const float maxFloat = std::numeric_limits<float>::max();
    auto& boundaries = logic.circuitAnimation.boundaries;
    boundaries.min = glm::vec3(+maxFloat, +maxFloat, +maxFloat);
    boundaries.max = glm::vec3(-maxFloat, -maxFloat, -maxFloat);

    auto onSkeletonPatch = [&](const CircuitBuilder::t_vec3Array& vertices,
                               const CircuitBuilder::t_indices& indices) -> void {

        for (int index : indices)
        {
            const glm::vec3& vertex = vertices[index];

            skeletonVertices.push_back({ vertex.x, vertex.y, vertex.z });

            if (vertex.x < boundaries.min.x) boundaries.min.x = vertex.x;
            if (vertex.y < boundaries.min.y) boundaries.min.y = vertex.y;
            if (vertex.z < boundaries.min.z) boundaries.min.z = vertex.z;

            if (vertex.x > boundaries.max.x) boundaries.max.x = vertex.x;
            if (vertex.y > boundaries.max.y) boundaries.max.y = vertex.y;
            if (vertex.z > boundaries.max.z) boundaries.max.z = vertex.z;
        }
    };

    float       latestSize = 0;
    const float maxDeformation = 0.3f;

    auto onGroundPatch = [&](const CircuitBuilder::t_vec3Array& vertices,
                             const CircuitBuilder::t_vec3Array& colors,
                             const CircuitBuilder::t_vec3Array& normals,
                             const CircuitBuilder::t_indices& indices) -> void {

        // save it for "onWallPatch" bellow
        latestSize = float(groundVertices.size());

        float       limitValue = latestSize / indices.size();
        const float limitStep = 1.0f / indices.size();

        for (int index : indices)
        {
            bool firstLine = (index < 2);

            const auto& color = (firstLine ? whiteColor : colors[index]);

            glm::vec3 deformation = {
                t_RNG::getRangedValue(-maxDeformation, maxDeformation),
                t_RNG::getRangedValue(-maxDeformation, maxDeformation),
                t_RNG::getRangedValue(-maxDeformation, maxDeformation)
            };

            glm::vec3 normal = (normals[index] + deformation) * 4.0f;

            groundVertices.push_back({ vertices[index], color, normal, limitValue });

            limitValue += limitStep;
        }

        logic.circuitAnimation.maxUpperValue += 1.0f;
    };

    auto onWallPatch = [&](const CircuitBuilder::t_vec3Array& vertices,
                           const CircuitBuilder::t_vec3Array& colors,
                           const CircuitBuilder::t_vec3Array& normals,
                           const CircuitBuilder::t_indices& indices) -> void {

        static_cast<void>(colors); // <= unused

        float limitValue = latestSize / indices.size();
        float limitStep = 1.0f / indices.size();

        for (int index : indices)
        {
            bool firstLine = (index < 2);

            const auto& color = (firstLine ? whiteColor : greyColor);

            glm::vec3 deformation = {
                t_RNG::getRangedValue(-0.2f, 0.2f),
                t_RNG::getRangedValue(-0.2f, 0.2f),
                t_RNG::getRangedValue(-0.2f, 0.2f)
            };

            glm::vec3 normal = (normals[index] + deformation) * 4.0f;

            wallsVertices.push_back({ vertices[index], color, normal, limitValue });

            limitValue += limitStep;
        }
    };

    logic.annTopology.init({15, 5, 2}, /*useBiasNeuron =*/ true);

#if defined D_WEB_BUILD
    logic.cores.genomesPerCore = EM_ASM_INT(return window.genomesPerCore || 30);
#else
    logic.cores.genomesPerCore = 90;
#endif

    logic.cores.totalCores = 3;
    logic.cores.totalCars = logic.cores.totalCores * logic.cores.genomesPerCore;

    AbstactSimulation::t_def simulationDef;
    simulationDef.filename = "assets/circuits/default.txt";
    simulationDef.genomesPerCore = logic.cores.genomesPerCore;
    simulationDef.totalCores = logic.cores.totalCores;
    simulationDef.neuralNetworkTopology = logic.annTopology;
    simulationDef.onSkeletonPatch = onSkeletonPatch;
    simulationDef.onNewGroundPatch = onGroundPatch;
    simulationDef.onNewWallPatch = onWallPatch;


    logic.cores.statesData.resize(simulationDef.totalCores);
    logic.cores.statesHistory.resize(simulationDef.totalCores);
    for (auto& stateHistory : logic.cores.statesHistory)
        stateHistory.resize(logic.cores.maxStateHistory);


    logic.simulation->initialise(simulationDef);



    boundaries.center = boundaries.min + (boundaries.max - boundaries.min) * 0.5f;

    graphic.camera.center = boundaries.center;
    graphic.camera.distance = 200.0f;


    //

    {
        auto& wireframes = graphic.geometries.wireframes;

        wireframes.circuitSkelton.updateBuffer(0, skeletonVertices);
        wireframes.circuitSkelton.setPrimitiveCount(skeletonVertices.size());
    }

    //

    {
        auto& animatedCircuit = graphic.geometries.animatedCircuit;

        animatedCircuit.ground.updateBuffer(0, groundVertices);
        animatedCircuit.ground.setPrimitiveCount(groundVertices.size());

        animatedCircuit.walls.updateBuffer(0, wallsVertices);
        animatedCircuit.walls.setPrimitiveCount(wallsVertices.size());

        logic.circuitAnimation.maxPrimitiveCount = groundVertices.size();
    }
}
