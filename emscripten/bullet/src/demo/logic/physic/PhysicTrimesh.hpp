
#pragma once

#include "thirdparty/GLMath.hpp"

#include <vector>

class btTriangleIndexVertexArray;
class btBvhTriangleMeshShape;
class btDefaultMotionState;
class btRigidBody;

class PhysicWorld;

class PhysicTrimesh
{
private:
    friend PhysicWorld;

private:
    float*  _vertices = nullptr;
    int*    _indices = nullptr;
    int     _index = -1;

    struct bullet
    {
        btTriangleIndexVertexArray* indexVertexArrays = nullptr;
        btBvhTriangleMeshShape*     shape = nullptr;
        btDefaultMotionState*       motionState = nullptr;
        btRigidBody*                body = nullptr;
    }
    _bullet;

private:
    PhysicTrimesh(const std::vector<glm::vec3>& vertices,
                  const std::vector<int>& indices,
                  int index);

    ~PhysicTrimesh();

public:
    int    getIndex() const;

};
