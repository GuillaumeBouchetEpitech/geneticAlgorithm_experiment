

#include "World.hpp"

#include "Trimesh.hpp"
#include "Vehicle.hpp"



#include <Bullet/btBulletDynamicsCommon.h>


#define D_GROUP_SENSOR	(1 << 0)
#define D_GROUP_WALL	(1 << 1)



namespace	Physic
{


World::World()
{
	m_pBroadphase = new btDbvtBroadphase();
	m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
	m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);
	m_pSolver = new btSequentialImpulseConstraintSolver;
	m_pDynamicsWorld = new btDiscreteDynamicsWorld(
		m_pDispatcher,
		m_pBroadphase,
		m_pSolver,
		m_pCollisionConfiguration
	);
	m_pDynamicsWorld->setGravity(btVector3(0, 0, -10));
}

World::~World()
{
	for (auto& trimesh : m_arr_Trimesh)
		delete trimesh;

	for (auto& vehicles : m_arr_Vehicles)
		delete vehicles;

	delete m_pDynamicsWorld;
	delete m_pSolver;
	delete m_pCollisionConfiguration;
	delete m_pDispatcher;
	delete m_pBroadphase;
}

void	World::step(float elapsed_time)
{
	const int	maxSubSteps = 0; // <= elapsed_time MUST be constant if not using substeps
	m_pDynamicsWorld->stepSimulation(elapsed_time, maxSubSteps);
}

void	World::setDebugDrawer(btIDebugDraw* pDebugDraw)
{
	m_pDynamicsWorld->setDebugDrawer(pDebugDraw);
}

void	World::debugDrawWorld()
{
	m_pDynamicsWorld->debugDrawWorld();
}

//

void	World::createGround(const std::vector<float>& b, const std::vector<int>& i, int id)
{
	Trimesh*	m_pTrimesh = new Trimesh(b, i, id);
	m_pDynamicsWorld->addRigidBody( m_pTrimesh->m_pBbody, D_GROUP_SENSOR, -1 );

	m_pTrimesh->m_pBbody->setUserPointer( (void*)m_pTrimesh );

	m_arr_Trimesh.push_back(m_pTrimesh);
}

void	World::createWall(const std::vector<float>& b, const std::vector<int>& i)
{
	Trimesh*	m_pTrimesh = new Trimesh(b, i, -1);
	m_pDynamicsWorld->addRigidBody( m_pTrimesh->m_pBbody, D_GROUP_SENSOR, D_GROUP_WALL );

	m_pTrimesh->m_pBbody->setUserPointer( (void*)m_pTrimesh );

	m_arr_Trimesh.push_back(m_pTrimesh);
}

//

Vehicle*	World::createVehicle()
{
	Vehicle*	pVehicle = new Vehicle(m_pDynamicsWorld);
	m_pDynamicsWorld->addRigidBody( pVehicle->m_pCarChassis, 0, 0 );
	m_pDynamicsWorld->addVehicle( pVehicle->m_pVehicle );

	m_arr_Vehicles.push_back(pVehicle);

	return pVehicle;
}

//

bool	World::raycast(const float* pVec3_from, const float* pVec3_to, Trimesh** out_ppTarget, float* out_pVec3)
{
	btVector3	from, to;
	from[0] = pVec3_from[0];
	from[1] = pVec3_from[1];
	from[2] = pVec3_from[2];
	to[0] = pVec3_to[0];
	to[1] = pVec3_to[1];
	to[2] = pVec3_to[2];

	btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);

	rayCallback.m_collisionFilterGroup = D_GROUP_WALL;
	rayCallback.m_collisionFilterMask = D_GROUP_SENSOR;

	m_pDynamicsWorld->rayTest(from, to, rayCallback);

	if (rayCallback.hasHit())
	{
		const btRigidBody* pBody = btRigidBody::upcast(rayCallback.m_collisionObject);

		if (pBody && pBody->hasContactResponse())
		{
			void*	pUserdata = pBody->getUserPointer();

			if (out_ppTarget)
				*out_ppTarget = static_cast<Trimesh*>(pUserdata);
			if (out_pVec3)
			{
				out_pVec3[0] = rayCallback.m_hitPointWorld[0];
				out_pVec3[1] = rayCallback.m_hitPointWorld[1];
				out_pVec3[2] = rayCallback.m_hitPointWorld[2];
			}

			return true;
		}
	}

	return false;
}

};

