
#include "PhysicWorld.hpp"

#include "PhysicTrimesh.hpp"
#include "PhysicVehicle.hpp"

#include "thirdparty/BulletPhysics.hpp"

#include <algorithm>

PhysicWorld::PhysicWorld()
{
	_bullet.broadphase = new btDbvtBroadphase();
	_bullet.collisionConfiguration = new btDefaultCollisionConfiguration();
	_bullet.dispatcher = new btCollisionDispatcher(_bullet.collisionConfiguration);
	_bullet.solver = new btSequentialImpulseConstraintSolver;
	_bullet.dynamicsWorld = new btDiscreteDynamicsWorld(
		_bullet.dispatcher,
		_bullet.broadphase,
		_bullet.solver,
		_bullet.collisionConfiguration
	);
	_bullet.dynamicsWorld->setGravity(btVector3(0, 0, -10));

	// auto onContact = [](btManifoldPoint& contactPoint, void* pBody0, void* pBody1) -> bool {

	// 	if (!m_onContact)
	// 		return false;

	// 	auto position = contactPoint.m_positionWorldOnB;
	// 	auto normal = contactPoint.m_normalWorldOnB;

	// 	m_onContact({ position[0], position[1], position[2] }, { normal[0], normal[1], normal[2] });

	// 	return true;
	// };

	// gContactProcessedCallback = onContact;
}

PhysicWorld::~PhysicWorld()
{
	for (auto& trimesh : _trimeshes)
		delete trimesh;

	for (auto& vehicles : _vehicles)
		delete vehicles;

	delete _bullet.dynamicsWorld;
	delete _bullet.solver;
	delete _bullet.collisionConfiguration;
	delete _bullet.dispatcher;
	delete _bullet.broadphase;
}

void	PhysicWorld::step()
{
	const int	maxSubSteps = 1; // <= so it's "deterministic"
	const float	fixedTimeStep = 1.0f / 60.0f;

	_bullet.dynamicsWorld->stepSimulation(fixedTimeStep, maxSubSteps, fixedTimeStep);
}

//

void	PhysicWorld::removeAll()
{
	for (auto& trimesh : _trimeshes)
		_bullet.dynamicsWorld->removeRigidBody(trimesh->_bullet.body);

	for (auto& vehicle : _liveVehicles)
	{
		_bullet.dynamicsWorld->removeRigidBody(vehicle->_bullet.carChassis);
		_bullet.dynamicsWorld->removeVehicle(vehicle->_bullet.vehicle);
	}
}

void	PhysicWorld::reinsertAll()
{
	_bullet.broadphase->resetPool(_bullet.dispatcher);

	_bullet.solver->reset();

	for (auto& vehicle : _vehicles)
	{
		vehicle->fullBrake();

		vehicle->_bullet.vehicle->resetSuspension();
		vehicle->_bullet.vehicle->updateVehicle(0);

		vehicle->_bullet.carChassis->forceActivationState(ACTIVE_TAG);
		vehicle->_bullet.carChassis->setDeactivationTime(0);
	}

	for (auto& trimesh : _trimeshes)
		_bullet.dynamicsWorld->addRigidBody(trimesh->_bullet.body, trimesh->getGroup(), trimesh->getMask());

	for (auto& vehicle : _vehicles)
	{
		_bullet.dynamicsWorld->addRigidBody(vehicle->_bullet.carChassis, D_GROUP_VEHICLE, D_MASK_VEHICLE);
		_bullet.dynamicsWorld->addVehicle(vehicle->_bullet.vehicle);
	}
}


// void	PhysicWorld::setDebugDrawer(btIDebugDraw* pDebugDraw)
// {
// 	_bullet.dynamicsWorld->setDebugDrawer(pDebugDraw);
// }

// void	PhysicWorld::debugDrawWorld()
// {
// 	_bullet.dynamicsWorld->debugDrawWorld();
// }

//

void	PhysicWorld::createGround(const t_vertices& vertices, const t_indices& indices, int id)
{
	PhysicTrimesh* trimesh = new PhysicTrimesh(vertices, indices, id, D_GROUP_GROUND, D_MASK_GROUND);
	btRigidBody* body = trimesh->_bullet.body;

	_bullet.dynamicsWorld->addRigidBody(body, trimesh->getGroup(), trimesh->getMask());

	body->setUserPointer((void*)trimesh);

	_trimeshes.push_back(trimesh);
}

void	PhysicWorld::createWall(const t_vertices& vertices, const t_indices& indices)
{
	PhysicTrimesh* trimesh = new PhysicTrimesh(vertices, indices, -1, D_GROUP_WALL, D_MASK_WALL);
	btRigidBody* body = trimesh->_bullet.body;

	_bullet.dynamicsWorld->addRigidBody(body, trimesh->getGroup(), trimesh->getMask());

	body->setUserPointer((void*)trimesh);

	_trimeshes.push_back(trimesh);
}

//

PhysicVehicle*	PhysicWorld::createVehicle()
{
	PhysicVehicle* vehicle = new PhysicVehicle(_bullet.dynamicsWorld);

	_vehicles.push_back(vehicle);

	return vehicle;
}

void	PhysicWorld::addVehicle(PhysicVehicle& vehicle)
{
	auto it = std::find(_liveVehicles.begin(), _liveVehicles.end(), &vehicle);
	if (it != _liveVehicles.end())
		return;

	_bullet.dynamicsWorld->addRigidBody(vehicle._bullet.carChassis, D_GROUP_VEHICLE, D_MASK_VEHICLE);
	_bullet.dynamicsWorld->addVehicle(vehicle._bullet.vehicle);
	_liveVehicles.push_back(&vehicle);
}

void	PhysicWorld::removeVehicle(PhysicVehicle& vehicle)
{
	auto it = std::find(_liveVehicles.begin(), _liveVehicles.end(), &vehicle);
	if (it == _liveVehicles.end())
		return;

	_bullet.dynamicsWorld->removeRigidBody(vehicle._bullet.carChassis);
	_bullet.dynamicsWorld->removeVehicle(vehicle._bullet.vehicle);
	_liveVehicles.erase(it);
}

const std::vector<PhysicVehicle*>&	PhysicWorld::getVehicles() const
{
	return _vehicles;
}

//

bool	PhysicWorld::raycast(PhysicWorld::t_raycastParams& params)
{
	btVector3 rayFrom(params.from.x, params.from.y, params.from.z);
	btVector3 rayTo(params.to.x, params.to.y, params.to.z);

	btCollisionWorld::ClosestRayResultCallback	rayCallback(rayFrom, rayTo);

	rayCallback.m_collisionFilterGroup = params.collisionGroup;
	rayCallback.m_collisionFilterMask = params.collisionMask;

	_bullet.dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);

	params.result.hasHit = rayCallback.hasHit();

	if (!rayCallback.hasHit())
		return false;

	const btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);

	if (!body || !body->hasContactResponse())
		return false;

	void* userdata = body->getUserPointer();

	if (userdata)
		params.result.impactIndex = static_cast<PhysicTrimesh*>(userdata)->getIndex();

	const auto& impact = rayCallback.m_hitPointWorld;
	params.result.impactPoint = { impact[0], impact[1], impact[2] };

	return true;
}
