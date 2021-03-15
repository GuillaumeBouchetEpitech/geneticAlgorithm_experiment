
#include "Data.hpp"

#include "demo/utilities/math/RandomNumberGenerator.hpp"

#include <limits> // <= std::numeric_limits<T>::max()

namespace /*anonymous*/
{

    struct AnimatedVertex
    {
        glm::vec3 postion;
        glm::vec3 color;
        glm::vec3 normal; // <= animation
        float limitId; // <= animation

        AnimatedVertex() = default;

        AnimatedVertex(
            const glm::vec3& postion,
            const glm::vec3& color,
            const glm::vec3& normal,
            float limitId
        )
            : postion(postion)
            , color(color)
            , normal(normal)
            , limitId(limitId)

        {}
    };

    using AnimatedVertices = std::vector<AnimatedVertex>;

};

void Data::initialiseCircuit()
{
    std::vector<glm::vec3> skeletonVertices;
    AnimatedVertices groundVertices;
    AnimatedVertices wallsVertices;

    // pre-allocattions
    skeletonVertices.reserve(1024);
    groundVertices.reserve(4 * 1024);
    wallsVertices.reserve(groundVertices.capacity() * 2); // walls twice bigger that ground geometry

    const glm::vec3 greyColor = { 0.6f, 0.6f, 0.6f };
    const glm::vec3 whiteColor = { 1.0f, 1.0f, 1.0f };

    const float maxFloat = std::numeric_limits<float>::max();
    auto& boundaries = logic.circuitAnimation.boundaries;
    boundaries.min = glm::vec3(+maxFloat, +maxFloat, +maxFloat);
    boundaries.max = glm::vec3(-maxFloat, -maxFloat, -maxFloat);

    auto onSkeletonPatch = [
        &boundaries,
        &skeletonVertices
    ](
        const CircuitBuilder::Vec3Array& vertices,
        const CircuitBuilder::Indices& indices
    ) -> void
    {
        for (int index : indices)
        {
            const glm::vec3& vertex = vertices[index];

            skeletonVertices.push_back(vertex);

            if (vertex.x < boundaries.min.x) boundaries.min.x = vertex.x;
            if (vertex.y < boundaries.min.y) boundaries.min.y = vertex.y;
            if (vertex.z < boundaries.min.z) boundaries.min.z = vertex.z;

            if (vertex.x > boundaries.max.x) boundaries.max.x = vertex.x;
            if (vertex.y > boundaries.max.y) boundaries.max.y = vertex.y;
            if (vertex.z > boundaries.max.z) boundaries.max.z = vertex.z;
        }
    };

    float latestSize = 0;
    constexpr float maxDeformation = 0.5f;

    auto onGroundPatch = [
        this,
        &latestSize,
        &whiteColor,
        &groundVertices
    ](
        const CircuitBuilder::Vec3Array& vertices,
        const CircuitBuilder::Vec3Array& colors,
        const CircuitBuilder::Vec3Array& normals,
        const CircuitBuilder::Indices& indices
    ) -> void
    {
        // save it for "onWallPatch" bellow
        latestSize = float(groundVertices.size());

        float       limitValue = latestSize / indices.size();
        const float limitStep = 1.0f / indices.size();

        for (int index : indices)
        {
            bool firstLine = (index < 2); // hacky

            const auto& color = (firstLine ? whiteColor : colors[index]);

            glm::vec3 deformation = {
                RNG::getRangedValue(-maxDeformation, +maxDeformation),
                RNG::getRangedValue(-maxDeformation, +maxDeformation),
                RNG::getRangedValue(-maxDeformation, +maxDeformation)
            };

            glm::vec3 normal = (normals[index] + deformation) * 4.0f;

            groundVertices.emplace_back(vertices[index], color, normal, limitValue);

            limitValue += limitStep;
        }

        logic.circuitAnimation.maxUpperValue += 1.0f;

        { // flockingManager

            glm::vec3 sum{0, 0, 0};
            int total = 0;

            for (const glm::vec3& vertex : vertices)
            {
                sum += vertex;
                ++total;
            }

            if (total > 0)
            {
                graphic.flockingManager.addKnot(sum / float(total));
            }

        } // flockingManager
    };

    auto onWallPatch = [
        &latestSize,
        &whiteColor,
        &greyColor,
        &wallsVertices
    ](
        const CircuitBuilder::Vec3Array& vertices,
        const CircuitBuilder::Vec3Array& colors,
        const CircuitBuilder::Vec3Array& normals,
        const CircuitBuilder::Indices& indices
    ) -> void
    {
        static_cast<void>(colors); // <= unused

        float limitValue = latestSize / indices.size();
        float limitStep = 1.0f / indices.size();

        for (int index : indices)
        {
            bool firstLine = (index < 2); // hacky

            const auto& color = (firstLine ? whiteColor : greyColor);

            glm::vec3 deformation = {
                RNG::getRangedValue(-maxDeformation, +maxDeformation),
                RNG::getRangedValue(-maxDeformation, +maxDeformation),
                RNG::getRangedValue(-maxDeformation, +maxDeformation)
            };

            glm::vec3 normal = (normals[index] + deformation) * 4.0f;

            wallsVertices.emplace_back(vertices[index], color, normal, limitValue);

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

    AbstactSimulation::Definition simulationDef;
    simulationDef.filename = "assets/circuits/default.txt";
    simulationDef.genomesPerCore = logic.cores.genomesPerCore;
    simulationDef.totalCores = logic.cores.totalCores;
    simulationDef.neuralNetworkTopology = logic.annTopology;
    simulationDef.onSkeletonPatch = onSkeletonPatch; // callback
    simulationDef.onNewGroundPatch = onGroundPatch; // callback
    simulationDef.onNewWallPatch = onWallPatch; // callback

    logic.simulation->initialise(simulationDef);

    //
    // the callbacks have now been called

    logic.cores.statesData.resize(simulationDef.totalCores);
    logic.cores.statesHistory.resize(simulationDef.totalCores);
    for (auto& stateHistory : logic.cores.statesHistory)
        stateHistory.resize(Data::Logic::Cores::maxStateHistory);

    boundaries.center = boundaries.min + (boundaries.max - boundaries.min) * 0.5f;

    graphic.camera.center = logic.simulation->getStartPosition();
    graphic.camera.distance = 200.0f;

    { // compute circuit skeleton wireframe geometry

        auto& wireframes = graphic.geometries.wireframes;

        wireframes.circuitSkelton.updateBuffer(0, skeletonVertices);
        wireframes.circuitSkelton.setPrimitiveCount(skeletonVertices.size());

    } // compute circuit skeleton wireframe geometry

    { // compute circuit ground and walls geometries

        auto& animatedCircuit = graphic.geometries.animatedCircuit;

        animatedCircuit.ground.updateBuffer(0, groundVertices);
        animatedCircuit.ground.setPrimitiveCount(groundVertices.size());

        animatedCircuit.walls.updateBuffer(0, wallsVertices);
        animatedCircuit.walls.setPrimitiveCount(wallsVertices.size());

        logic.circuitAnimation.maxPrimitiveCount = groundVertices.size();

    } // compute circuit ground and walls geometries
}
