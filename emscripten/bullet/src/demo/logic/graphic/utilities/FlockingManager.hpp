
#pragma once

#include "demo/utilities/types.hpp"

#include <vector>
#include <unordered_set>

#include "thirdparty/GLMath.hpp"

class FlockingManager
{
public:
    FlockingManager();
public:
    void initialise();
    void addKnot(const glm::vec3& knot);
    void update(float delta);

    void render(const glm::mat4& sceneMatrix) const;

private:
    struct Boid
    {
        glm::vec3 pos{0, 0, 0};
        glm::vec3 acc{0, 0, 0};
        glm::vec3 vel{0, 0, 0};
    };

    std::vector<Boid> _boids;
    std::vector<glm::vec3> _circuitKnots;

    std::unordered_set<int> _alreadyPropagated;

};
