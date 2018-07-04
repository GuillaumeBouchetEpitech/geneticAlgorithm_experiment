

#pragma once



#include <vector>


class	btTriangleIndexVertexArray;
class	btBvhTriangleMeshShape;
class	btDefaultMotionState;
class	btRigidBody;


namespace	Physic
{


class World;

class Trimesh
{
private:
	friend World;

private:
	float*	m_pArr_Vertices = nullptr;
	int*	m_pArr_Indices = nullptr;

	int 	m_id = -1;

	btTriangleIndexVertexArray*	m_pIndexVertexArrays = nullptr;
	btBvhTriangleMeshShape*		m_pShape = nullptr;
	btDefaultMotionState*	m_pMotionState = nullptr;
	btRigidBody*	m_pBbody = nullptr;

private:
	Trimesh(const std::vector<float>& b, const std::vector<int>& i, int id);
	~Trimesh();

public:
	inline int	getID() const { return m_id; }

};


};

