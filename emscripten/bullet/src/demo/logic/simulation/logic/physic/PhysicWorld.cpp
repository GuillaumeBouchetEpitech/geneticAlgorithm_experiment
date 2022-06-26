
#include "PhysicWorld.hpp"

#include "PhysicTrimesh.hpp"
#include "PhysicVehicle.hpp"

#include "helpers/BulletPhysics.hpp"

#include <algorithm>

PhysicWorld::PhysicWorld()
{
    _initialise();
}

PhysicWorld::~PhysicWorld()
{
    _dispose();
}

void PhysicWorld::_initialise()
{
    _bullet.broadphase = new btDbvtBroadphase();
    _bullet.collisionConfiguration = new btDefaultCollisionConfiguration();
    _bullet.dispatcher = new btCollisionDispatcher(_bullet.collisionConfiguration);
    _bullet.solver = new btSequentialImpulseConstraintSolver;
    _bullet.dynamicsWorld = new btDiscreteDynamicsWorld(
    // _bullet.dynamicsWorld = new btDiscreteDynamicsWorldMt(
        _bullet.dispatcher,
        _bullet.broadphase,
        _bullet.solver,
        _bullet.collisionConfiguration
    );
    _bullet.dynamicsWorld->setGravity(btVector3(0, 0, -10));

    { // ground plane

        _bullet.ground.shape = new btStaticPlaneShape(btVector3(0, 0, 1), 1.0);

        btScalar mass = 0;
        btVector3 localInertia(0, 0, 0);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, nullptr, _bullet.ground.shape, localInertia);

        _bullet.ground.body = new btRigidBody(rbInfo);

        _bullet.ground.body->setFriction(1.0f);

        short group = asValue(PhysicWorld::Groups::ground);
        short mask = asValue(PhysicWorld::Masks::ground);

        _bullet.dynamicsWorld->addRigidBody(_bullet.ground.body, group, mask);

    } // ground plane
}

void PhysicWorld::_dispose(bool emptyAllContainers /* = true */)
{
    if (emptyAllContainers)
    {
        for (auto* vehicle : _vehicles)
        {
            if (_liveVehicles.count(vehicle) > 0)
                _bullet.dynamicsWorld->removeVehicle(vehicle->_bullet.vehicle);

            _bullet.dynamicsWorld->removeRigidBody(vehicle->_bullet.carChassis);

            delete vehicle;
        }
        _liveVehicles.clear();
        _vehicles.clear();

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
    }

    _bullet.dynamicsWorld->removeRigidBody(_bullet.ground.body);
    delete _bullet.ground.body;
    delete _bullet.ground.shape;

    delete _bullet.dynamicsWorld;
    delete _bullet.solver;
    delete _bullet.collisionConfiguration;
    delete _bullet.broadphase;
    delete _bullet.dispatcher;
}

void PhysicWorld::step(float elapsedTime)
{
    const int maxSubSteps = 0; // <= so it's "deterministic"
    const float fixedTimeStep = 1.0f / 30.0f;

    _bullet.dynamicsWorld->stepSimulation(elapsedTime, maxSubSteps, fixedTimeStep);
}

void PhysicWorld::reset()
{
    { // remove

        for (PhysicVehicle* vehicle : _vehicles)
        {
            removeVehicle(*vehicle);
            vehicle->_dispose();
        }

        for (PhysicTrimesh* trimesh : _wallsTrimesh)
            _bullet.dynamicsWorld->removeRigidBody(trimesh->_bullet.body);
        for (PhysicTrimesh* trimesh : _groundsTrimesh)
            _bullet.dynamicsWorld->removeRigidBody(trimesh->_bullet.body);

        // dispose only the physic world
        // => we want to keep the vehicles container full
        _dispose(false);

    } // remove

    { // rebuild

        _initialise();

        for (PhysicTrimesh* trimesh : _groundsTrimesh)
        {
            short group = asValue(PhysicWorld::Groups::ground);
            short mask = asValue(PhysicWorld::Masks::ground);

            _bullet.dynamicsWorld->addRigidBody(trimesh->_bullet.body, group, mask);
        }

        for (PhysicTrimesh* trimesh : _wallsTrimesh)
        {
            short group = asValue(PhysicWorld::Groups::wall);
            short mask = asValue(PhysicWorld::Masks::wall);

            _bullet.dynamicsWorld->addRigidBody(trimesh->_bullet.body, group, mask);
        }

        for (PhysicVehicle* vehicle : _vehicles)
        {
            short group = asValue(PhysicWorld::Groups::vehicle);
            short mask = asValue(PhysicWorld::Masks::vehicle);

            vehicle->_initialise(*_bullet.dynamicsWorld, group, mask);

            addVehicle(*vehicle);

            vehicle->reset();
        }

    } // rebuild
}

//

void PhysicWorld::createGround(const Vertices& vertices, const Indices& indices, int groundIndex)
{
    PhysicTrimesh* trimesh = new PhysicTrimesh(vertices, indices, groundIndex);
    btRigidBody* body = trimesh->_bullet.body;

    short group = asValue(PhysicWorld::Groups::ground);
    short mask = asValue(PhysicWorld::Masks::ground);

    _bullet.dynamicsWorld->addRigidBody(body, group, mask);

    body->setUserPointer((void*)trimesh);

    _groundsTrimesh.push_back(trimesh);
}

void PhysicWorld::createWall(const Vertices& vertices, const Indices& indices)
{
    PhysicTrimesh* trimesh = new PhysicTrimesh(vertices, indices, -1);
    btRigidBody* body = trimesh->_bullet.body;

    short group = asValue(PhysicWorld::Groups::wall);
    short mask = asValue(PhysicWorld::Masks::wall);

    _bullet.dynamicsWorld->addRigidBody(body, group, mask);

    body->setUserPointer((void*)trimesh);

    _wallsTrimesh.push_back(trimesh);
}

//

PhysicVehicle* PhysicWorld::createVehicle()
{
    short group = asValue(PhysicWorld::Groups::vehicle);
    short mask = asValue(PhysicWorld::Masks::vehicle);

    PhysicVehicle* vehicle = new PhysicVehicle(*_bullet.dynamicsWorld, group, mask);

    addVehicle(*vehicle);

    _vehicles.push_back(vehicle);

    return vehicle;
}

void PhysicWorld::destroyVehicle(PhysicVehicle* vehicle)
{
    auto it = std::find(_vehicles.begin(), _vehicles.end(), vehicle);
    if (it != _vehicles.end())
        return; // no found

    removeVehicle(*vehicle);

    // _bullet.dynamicsWorld->removeVehicle(vehicle->_bullet.vehicle);
    // _bullet.dynamicsWorld->removeRigidBody(vehicle->_bullet.carChassis);

    _vehicles.erase(it);

    delete vehicle;
}

void PhysicWorld::addVehicle(PhysicVehicle& vehicle)
{
    if (_liveVehicles.count(&vehicle) > 0)
        return;

    vehicle.reset();

    short group = asValue(PhysicWorld::Groups::vehicle);
    short mask = asValue(PhysicWorld::Masks::vehicle);

    _bullet.dynamicsWorld->addRigidBody(vehicle._bullet.carChassis, group, mask);
    _bullet.dynamicsWorld->addVehicle(vehicle._bullet.vehicle);

    _liveVehicles.insert(&vehicle);
}

void PhysicWorld::removeVehicle(PhysicVehicle& vehicle)
{
    if (_liveVehicles.count(&vehicle) == 0)
        return;

    _bullet.dynamicsWorld->removeVehicle(vehicle._bullet.vehicle);
    _bullet.dynamicsWorld->removeRigidBody(vehicle._bullet.carChassis);

    _liveVehicles.erase(&vehicle);
}

const std::vector<PhysicVehicle*>& PhysicWorld::getVehicles() const
{
    return _vehicles;
}

//

bool PhysicWorld::raycastGroundsAndWalls(PhysicWorld::RaycastParamsGroundsAndWalls& params)
{
    btVector3 rayFrom(params.from.x, params.from.y, params.from.z);
    btVector3 rayTo(params.to.x, params.to.y, params.to.z);

    btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);

    rayCallback.m_collisionFilterGroup = asValue(PhysicWorld::Groups::sensor);
    rayCallback.m_collisionFilterMask = asValue(PhysicWorld::Masks::eyeSensor);

    _bullet.dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);

    params.result.hasHit = rayCallback.hasHit();

    if (!rayCallback.hasHit())
        return false;

    const btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);

    if (!body || !body->hasContactResponse())
        return false;

    const auto& impact = rayCallback.m_hitPointWorld;
    params.result.impactPoint = { impact[0], impact[1], impact[2] };

    return true;
}

bool PhysicWorld::raycastGrounds(PhysicWorld::RaycastParamsGroundsOnly& params)
{
    btVector3 rayFrom(params.from.x, params.from.y, params.from.z);
    btVector3 rayTo(params.to.x, params.to.y, params.to.z);

    btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);

    rayCallback.m_collisionFilterGroup = asValue(PhysicWorld::Groups::sensor);
    rayCallback.m_collisionFilterMask = asValue(PhysicWorld::Masks::groundSensor);

    _bullet.dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);

    params.result.hasHit = rayCallback.hasHit();

    if (!rayCallback.hasHit())
        return false;

    const btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);

    if (!body || !body->hasContactResponse())
        return false;

    void* userdata = body->getUserPointer();

    // used by the ground sensor of the car to detect the current checkpoint
    if (userdata)
        params.result.impactIndex = static_cast<PhysicTrimesh*>(userdata)->getIndex();

    const auto& impact = rayCallback.m_hitPointWorld;
    params.result.impactPoint = { impact[0], impact[1], impact[2] };

    return true;
}
