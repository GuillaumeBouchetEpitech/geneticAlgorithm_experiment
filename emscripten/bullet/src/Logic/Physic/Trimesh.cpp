

#include "Trimesh.hpp"

#include <Bullet/btBulletDynamicsCommon.h>


namespace	Physic
{


Trimesh::Trimesh(const std::vector<float>& buffer, const std::vector<int>& indices, int id)
	: m_id(id)
{
	unsigned int vertNumber = buffer.size();
	m_pArr_Vertices = new float[buffer.size()];
	memcpy(m_pArr_Vertices, &buffer[0], buffer.size() * sizeof(float));
	int vertStride = 3 * sizeof(float);

	unsigned int triangleNumber = indices.size() / 3;
	m_pArr_Indices = new int[indices.size()];
	memcpy(m_pArr_Indices, &indices[0], indices.size() * sizeof(int));
	int indexStride = 3 * sizeof(int);

	m_pIndexVertexArrays = new btTriangleIndexVertexArray(
		triangleNumber, m_pArr_Indices, indexStride,
		vertNumber, m_pArr_Vertices, vertStride
	);

	bool useQuantizedAabbCompression = true;
	m_pShape = new btBvhTriangleMeshShape(m_pIndexVertexArrays, useQuantizedAabbCompression);

	m_pMotionState = new btDefaultMotionState(btTransform(btQuaternion(0.0, 0.0, 0.0, 1.0), btVector3(0, 0, 0)));
	
	btScalar mass = 0;
	btVector3 localInertia(0,0,0);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, m_pMotionState, m_pShape, localInertia);

	m_pBbody = new btRigidBody(rbInfo);
}

Trimesh::~Trimesh()
{
	delete m_pBbody;
	delete m_pMotionState;
	delete m_pShape;
	delete m_pIndexVertexArrays;

	delete[] m_pArr_Indices;
	delete[] m_pArr_Vertices;
}


};

