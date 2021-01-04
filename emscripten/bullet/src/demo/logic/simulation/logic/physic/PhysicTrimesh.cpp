
#include "PhysicTrimesh.hpp"

#include "thirdparty/BulletPhysics.hpp"

PhysicTrimesh::PhysicTrimesh(const std::vector<glm::vec3>& vertices,
                             const std::vector<int>& indices,
                             int index)
    : _index(index)
{
    unsigned int vertNumber = vertices.size() * 3;
    _vertices.reset(new float[vertices.size() * 3]);
    std::memcpy(_vertices.get(), vertices.data(), vertices.size() * sizeof(glm::vec3));
    int vertStride = sizeof(glm::vec3);

    unsigned int triangleNumber = indices.size() / 3;
    _indices.reset(new int[indices.size()]);
    std::memcpy(_indices.get(), indices.data(), indices.size() * sizeof(int));
    int indexStride = 3 * sizeof(int);

    _bullet.indexVertexArrays = new btTriangleIndexVertexArray(
        triangleNumber, _indices.get(), indexStride,
        vertNumber, _vertices.get(), vertStride
    );

    bool useQuantizedAabbCompression = true;
    _bullet.shape = new btBvhTriangleMeshShape(_bullet.indexVertexArrays, useQuantizedAabbCompression);

    btScalar mass = 0;
    btVector3 localInertia(0, 0, 0);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, nullptr, _bullet.shape, localInertia);

    _bullet.body = new btRigidBody(rbInfo);

    _bullet.body->setFriction(1.0f);
}

PhysicTrimesh::~PhysicTrimesh()
{
    delete _bullet.body;
    delete _bullet.shape;
    delete _bullet.indexVertexArrays;
}

int PhysicTrimesh::getIndex() const
{
    return _index;
}
