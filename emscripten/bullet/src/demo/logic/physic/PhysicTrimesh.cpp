
#include "PhysicTrimesh.hpp"

#include "thirdparty/BulletPhysics.hpp"

PhysicTrimesh::PhysicTrimesh(const std::vector<glm::vec3>& vertices,
							 const std::vector<int>& indices,
							 int index/*, int group, int mask*/)
	: _index(index)
	// , _group(group)
	// , _mask(mask)
{
	unsigned int vertNumber = vertices.size() * 3;
	_vertices = new float[vertices.size() * 3];
	std::memcpy(_vertices, vertices.data(), vertices.size() * sizeof(glm::vec3));
	int vertStride = sizeof(glm::vec3);

	unsigned int triangleNumber = indices.size() / 3;
	_indices = new int[indices.size()];
	std::memcpy(_indices, indices.data(), indices.size() * sizeof(int));
	int indexStride = 3 * sizeof(int);

	_bullet.indexVertexArrays = new btTriangleIndexVertexArray(
		triangleNumber, _indices, indexStride,
		vertNumber, _vertices, vertStride
	);

	bool useQuantizedAabbCompression = true;
	_bullet.shape = new btBvhTriangleMeshShape(_bullet.indexVertexArrays, useQuantizedAabbCompression);

	// _bullet.motionState = new btDefaultMotionState(btTransform(btQuaternion(0.0, 0.0, 0.0, 1.0), btVector3(0, 0, 0)));
	_bullet.motionState = nullptr;

	btScalar mass = 0;
	btVector3 localInertia(0,0,0);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, _bullet.motionState, _bullet.shape, localInertia);

	_bullet.body = new btRigidBody(rbInfo);

	_bullet.body->setFriction(1.0f);
}

PhysicTrimesh::~PhysicTrimesh()
{
	delete _bullet.body;
	delete _bullet.motionState;
	delete _bullet.shape;
	delete _bullet.indexVertexArrays;

	delete[] _indices;
	delete[] _vertices;
}

int	PhysicTrimesh::getIndex() const
{
	return _index;
}

// int	PhysicTrimesh::getGroup() const
// {
// 	return _group;
// }

// int	PhysicTrimesh::getMask() const
// {
// 	return _mask;
// }
