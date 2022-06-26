
#pragma once

#include "./demo/utilities/NonCopyable.hpp"

#include "helpers/GLMath.hpp"

#include <vector>
#include <memory>

class btTriangleIndexVertexArray;
class btBvhTriangleMeshShape;
class btDefaultMotionState;
class btRigidBody;

class PhysicWorld;

class PhysicTrimesh
    : public NonCopyable
{
private:
    friend PhysicWorld;

private:
    std::unique_ptr<float[]> _verticesData;
    std::unique_ptr<int[]> _indicesData;
    int _index = -1;

    struct bullet
    {
        btTriangleIndexVertexArray* indexVertexArrays = nullptr;
        btBvhTriangleMeshShape*     shape = nullptr;
        btRigidBody*                body = nullptr;
    }
    _bullet;

private:
    PhysicTrimesh(const std::vector<glm::vec3>& vertices,
                  const std::vector<int>& indices,
                  int index);

    ~PhysicTrimesh();

public:
    int getIndex() const;

};
