
#pragma once

#include "demo/utilities/types.hpp"

#include <vector>
#include <unordered_set>

#include "demo/helpers/GLMath.hpp"

class FlockingManager
{
public:
    FlockingManager();
public:
    void initialise();
    void addKnot(const glm::vec3& knot);
    void update(float elapsedTime);

    void render(const glm::mat4& sceneMatrix) const;

private:
    struct Boid
    {
        glm::vec3 pos{0, 0, 0};
        glm::vec3 acc{0, 0, 0};
        glm::vec3 vel{0, 0, 0};

        bool activated = false;
        float activationTime = 0.0f;
        Boid* parent = nullptr;
    };

    struct BufferAttributes
    {
        glm::vec3   position;
        float       scale;
        glm::vec3   color;

        BufferAttributes(const glm::vec3& position, float scale, const glm::vec3& color)
            : position(position)
            , scale(scale)
            , color(color)
        {}
    };

    std::vector<Boid> _boids;
    std::vector<glm::vec3> _circuitKnots;

    std::unordered_set<int> _alreadyPropagated;

    std::vector<BufferAttributes> _particlesInstances;
};
