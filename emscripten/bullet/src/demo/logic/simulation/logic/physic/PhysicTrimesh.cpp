
#include "PhysicTrimesh.hpp"

#include "demo/helpers/BulletPhysics.hpp"

PhysicTrimesh::PhysicTrimesh(const std::vector<glm::vec3>& vertices,
                             const std::vector<int>& indices,
                             int index)
    : _index(index)
{
    const unsigned int vertNumber = vertices.size() * 3;
    _verticesData = std::make_unique<float[]>(vertices.size() * 3);
    const unsigned int verticesSizeInBytes = vertices.size() * sizeof(glm::vec3);
    std::memcpy(_verticesData.get(), vertices.data(), verticesSizeInBytes);
    int verticesStride = sizeof(glm::vec3);

    const unsigned int triangleNumber = indices.size() / 3;
    _indicesData = std::make_unique<int[]>(indices.size());
    const unsigned int indicesSizeInBytes = indices.size() * sizeof(int);
    std::memcpy(_indicesData.get(), indices.data(), indicesSizeInBytes);
    int indicesStride = 3 * sizeof(int);

    _bullet.indexVertexArrays = new btTriangleIndexVertexArray(
        triangleNumber, _indicesData.get(), indicesStride,
        vertNumber, _verticesData.get(), verticesStride);

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
