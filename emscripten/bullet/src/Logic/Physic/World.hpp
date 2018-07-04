

#pragma once


#include <vector>
#include <cstring> // memcpy


class	btBroadphaseInterface;
class	btDefaultCollisionConfiguration;
class	btCollisionDispatcher;
class	btSequentialImpulseConstraintSolver;
class	btDiscreteDynamicsWorld;
class	btIDebugDraw;


namespace	Physic
{


class Trimesh;
class Vehicle;

class World
{
private:
	btBroadphaseInterface*					m_pBroadphase = nullptr;
	btDefaultCollisionConfiguration*		m_pCollisionConfiguration = nullptr;
	btCollisionDispatcher*					m_pDispatcher = nullptr;
	btSequentialImpulseConstraintSolver*	m_pSolver = nullptr;
	btDiscreteDynamicsWorld*				m_pDynamicsWorld = nullptr;

public:
	World();
	~World();

	//
	//
	// world

public:
	void	step(float elapsed_time);
	void	setDebugDrawer(btIDebugDraw* pDebugDraw);
	void	debugDrawWorld();

	//
	//
	// trimesh

private:
	std::vector<Trimesh*>	m_arr_Trimesh;
public:
	void	createGround(const std::vector<float>& b, const std::vector<int>& i, int id);
	void	createWall(const std::vector<float>& v, const std::vector<int>& i);

	//
	//
	// vehicle

private:
	std::vector<Vehicle*>	m_arr_Vehicles;
public:
	void	createVehicle();
	inline Vehicle*	getVehicle(unsigned int index) { return m_arr_Vehicles.at(index); }
	inline unsigned int	getVehicleSize() const { return m_arr_Vehicles.size(); }

	//
	//
	// raycast

public:
	bool	raycast(const float* pVec3_from, const float* pVec3_to, Trimesh** out_ppTarget, float* out_pVec3);

};


};
