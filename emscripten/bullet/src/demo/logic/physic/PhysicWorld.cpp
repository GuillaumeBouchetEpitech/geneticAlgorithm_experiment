
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
	std::vector<PhysicVehicle*>	oldLiveVehicles;
	for (auto* vehicle : _liveVehicles)
		oldLiveVehicles.push_back(vehicle);
	_liveVehicles.clear();

	for (auto* vehicle : oldLiveVehicles)
	{
		auto* bbHandle = vehicle->_bullet.carChassis->getBroadphaseHandle();
		_bullet.broadphase
					->getOverlappingPairCache()
					->cleanProxyFromPairs(bbHandle, _bullet.dispatcher);

		_bullet.dynamicsWorld->removeRigidBody(vehicle->_bullet.carChassis);
		_bullet.dynamicsWorld->removeVehicle(vehicle->_bullet.vehicle);

		delete vehicle;
	}

	for (auto* trimesh : _wallsTrimesh)
	{
		_bullet.dynamicsWorld->removeRigidBody(trimesh->_bullet.body);
		delete trimesh;
	}

	for (auto* trimesh : _groundsTrimesh)
	{
		_bullet.dynamicsWorld->removeRigidBody(trimesh->_bullet.body);
		delete trimesh;
	}

	delete _bullet.dynamicsWorld;
	delete _bullet.solver;
	delete _bullet.collisionConfiguration;
	delete _bullet.broadphase;
	delete _bullet.dispatcher;
}

void	PhysicWorld::step()
{
	const int	maxSubSteps = 0; // <= so it's "deterministic"
	const float	fixedTimeStep = 1.0f / 60.0f;

	_bullet.dynamicsWorld->stepSimulation(fixedTimeStep, maxSubSteps, fixedTimeStep);
}

//

// void	PhysicWorld::removeAll()
// {
// 	for (auto& trimesh : _trimeshes)
// 		_bullet.dynamicsWorld->removeRigidBody(trimesh->_bullet.body);

// 	for (auto& vehicle : _liveVehicles)
// 	// {
// 	// 	_bullet.dynamicsWorld->removeRigidBody(vehicle->_bullet.carChassis);
// 	// 	_bullet.dynamicsWorld->removeVehicle(vehicle->_bullet.vehicle);
// 	// }
// 		removeVehicle(*vehicle);
// }

// void	PhysicWorld::reinsertAll()
// {
// 	_bullet.broadphase->resetPool(_bullet.dispatcher);

// 	_bullet.solver->reset();

// 	for (auto* trimesh : _trimeshes)
// 		_bullet.dynamicsWorld->addRigidBody(trimesh->_bullet.body, trimesh->getGroup(), trimesh->getMask());

// 	for (auto* vehicle : _vehicles)
// 		addVehicle(*vehicle);
// }

void	PhysicWorld::reset()
{
	// for (auto& vehicle : _liveVehicles)
	// 	removeVehicle(vehicle);
	// while (!_liveVehicles.empty())
	// 	removeVehicle(*_liveVehicles.begin());

	std::vector<PhysicVehicle*>	oldLiveVehicles;
	for (auto* vehicle : _liveVehicles)
		oldLiveVehicles.push_back(vehicle);
	_liveVehicles.clear();

	// for (auto* vehicle : oldLiveVehicles)
	// {
	// 	auto* bbHandle = vehicle->_bullet.carChassis->getBroadphaseHandle();
	// 	_bullet.broadphase
	// 				->getOverlappingPairCache()
	// 				->cleanProxyFromPairs(bbHandle, _bullet.dispatcher);

	// 	_bullet.dynamicsWorld->removeRigidBody(vehicle->_bullet.carChassis);
	// 	_bullet.dynamicsWorld->removeVehicle(vehicle->_bullet.vehicle);
	// }


	// _bullet.broadphase->resetPool(_bullet.dispatcher);
	// _bullet.solver->reset();

	// const int	maxSubSteps = 0; // <= so it's "deterministic"
	// const float	fixedTimeStep = 0.0f;
	// _bullet.dynamicsWorld->stepSimulation(fixedTimeStep, maxSubSteps, fixedTimeStep);

	for (auto* vehicle : _vehicles)
		addVehicle(vehicle);
}

//

void	PhysicWorld::createGround(const t_vertices& vertices, const t_indices& indices, int id)
{
	// PhysicTrimesh* trimesh = new PhysicTrimesh(vertices, indices, id, D_GROUP_GROUND, D_MASK_GROUND);
	PhysicTrimesh* trimesh = new PhysicTrimesh(vertices, indices, id);
	btRigidBody* body = trimesh->_bullet.body;

	// _bullet.dynamicsWorld->addRigidBody(body, trimesh->getGroup(), trimesh->getMask());
	_bullet.dynamicsWorld->addRigidBody(body, D_GROUP_GROUND, D_MASK_GROUND);

	body->setUserPointer((void*)trimesh);

	_groundsTrimesh.push_back(trimesh);
}

void	PhysicWorld::createWall(const t_vertices& vertices, const t_indices& indices)
{
	// PhysicTrimesh* trimesh = new PhysicTrimesh(vertices, indices, -1, D_GROUP_WALL, D_MASK_WALL);
	PhysicTrimesh* trimesh = new PhysicTrimesh(vertices, indices, -1);
	btRigidBody* body = trimesh->_bullet.body;

	// _bullet.dynamicsWorld->addRigidBody(body, trimesh->getGroup(), trimesh->getMask());
	_bullet.dynamicsWorld->addRigidBody(body, D_GROUP_WALL, D_MASK_WALL);


	body->setUserPointer((void*)trimesh);

	_wallsTrimesh.push_back(trimesh);
}

//

PhysicVehicle*	PhysicWorld::createVehicle()
{
	PhysicVehicle* vehicle = new PhysicVehicle(*_bullet.dynamicsWorld);

	// vehicle->reset();

	// _bullet.dynamicsWorld->addRigidBody(vehicle->_bullet.carChassis, D_GROUP_VEHICLE, D_MASK_VEHICLE);
	// _bullet.dynamicsWorld->addVehicle(vehicle->_bullet.vehicle);

	// _liveVehicles.insert(vehicle);

	addVehicle(vehicle);

	_vehicles.push_back(vehicle);

	return vehicle;
}

void	PhysicWorld::destroyVehicle(PhysicVehicle* vehicle)
{
	auto it = std::find(_vehicles.begin(), _vehicles.end(), vehicle);
	if (it != _vehicles.end())
		return; // no found

	removeVehicle(vehicle);

	_bullet.broadphase->getOverlappingPairCache()->cleanProxyFromPairs(vehicle->_bullet.carChassis->getBroadphaseHandle(), _bullet.dispatcher);

	_bullet.dynamicsWorld->removeVehicle(vehicle->_bullet.vehicle);
	_bullet.dynamicsWorld->removeRigidBody(vehicle->_bullet.carChassis);

	_vehicles.erase(it);

	delete vehicle;
}

void	PhysicWorld::addVehicle(PhysicVehicle* vehicle)
{
	if (vehicle == nullptr || _liveVehicles.count(vehicle) > 0)
		return;

	vehicle->reset();

	_bullet.dynamicsWorld->addRigidBody(vehicle->_bullet.carChassis, D_GROUP_VEHICLE, D_MASK_VEHICLE);
	_bullet.dynamicsWorld->addVehicle(vehicle->_bullet.vehicle);

	_liveVehicles.insert(vehicle);
}

void	PhysicWorld::removeVehicle(PhysicVehicle* vehicle)
{
	if (vehicle == nullptr || _liveVehicles.count(vehicle) == 0)
		return;

	_bullet.broadphase->getOverlappingPairCache()->cleanProxyFromPairs(vehicle->_bullet.carChassis->getBroadphaseHandle(), _bullet.dispatcher);

	_bullet.dynamicsWorld->removeRigidBody(vehicle->_bullet.carChassis);
	_bullet.dynamicsWorld->removeVehicle(vehicle->_bullet.vehicle);

	_liveVehicles.erase(vehicle);
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
