
#pragma once

#include "thirdparty/GLMath.hpp"

#include <vector>
#include <set>
#include <functional>

class	btBroadphaseInterface;
class	btDefaultCollisionConfiguration;
class	btCollisionDispatcher;
class	btSequentialImpulseConstraintSolver;
class	btDiscreteDynamicsWorld;
class	btIDebugDraw;

#define D_GROUP_ALL		(-1)
#define D_GROUP_SENSOR	(1 << 0)
#define D_GROUP_GROUND	(1 << 1)
#define D_GROUP_WALL	(1 << 2)
#define D_GROUP_VEHICLE	(1 << 3)

#define D_MASK_SENSOR	(D_GROUP_GROUND | D_GROUP_WALL)
#define D_MASK_GROUND	(D_GROUP_ALL)
#define D_MASK_WALL		(D_GROUP_ALL)
#define D_MASK_VEHICLE	(D_GROUP_GROUND)

class PhysicTrimesh;
class PhysicVehicle;

class PhysicWorld
{
public:
	typedef std::function<void(const glm::vec3&, const glm::vec3&)>	t_onContact;

	typedef std::vector<glm::vec3>	t_vertices;
	typedef std::vector<int>		t_indices;

private:

	struct bullet
	{
		btBroadphaseInterface*					broadphase = nullptr;
		btDefaultCollisionConfiguration*		collisionConfiguration = nullptr;
		btCollisionDispatcher*					dispatcher = nullptr;
		btSequentialImpulseConstraintSolver*	solver = nullptr;
		btDiscreteDynamicsWorld*				dynamicsWorld = nullptr;
	}
	_bullet;

	// t_onContact	m_onContact;

public:
	PhysicWorld();
	~PhysicWorld();

	//
	//
	// world

public:
	void	step();

// public:
// 	void	removeAll();
// 	void	reinsertAll();

public:
	void	reset();

	// inline void	setOnContact(const t_onContact& onContact)
	// {
	// 	m_onContact = onContact;
	// }

	//
	//
	// trimesh

private:
	std::vector<PhysicTrimesh*>	_groundsTrimesh;
	std::vector<PhysicTrimesh*>	_wallsTrimesh;
public:
	void	createGround(const t_vertices& vertices, const t_indices& indices, int id);
	void	createWall(const t_vertices& vertices, const t_indices& indices);

	//
	//
	// vehicle

private:
	std::vector<PhysicVehicle*>	_vehicles;
	std::set<PhysicVehicle*>	_liveVehicles;
public:
	PhysicVehicle*	createVehicle();
	void			destroyVehicle(PhysicVehicle* vehicle);
	void			addVehicle(PhysicVehicle* vehicle);
	void			removeVehicle(PhysicVehicle* vehicle);

	const std::vector<PhysicVehicle*>&	getVehicles() const;

	//
	//
	// raycast

public:
	struct t_raycastParams
	{
		glm::vec3	from;
		glm::vec3	to;

		short	collisionGroup;
		short	collisionMask;

		struct t_result
		{
			bool		hasHit = false;
			glm::vec3	impactPoint;
			int			impactIndex = -1;
		}
		result;

		t_raycastParams(const glm::vec3& rayFrom,
						const glm::vec3& rayTo,
						short group = D_GROUP_ALL,
						short mask = D_GROUP_ALL)
			: from(rayFrom)
			, to(rayTo)
			, collisionGroup(group)
			, collisionMask(mask)
		{}
	};

public:
	bool	raycast(t_raycastParams& params);

};
