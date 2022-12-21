
#include "PhysicBodyManager.hpp"

#include "framework/physic/PhysicWorld.hpp"

#include "framework/helpers/internals/BulletPhysics.hpp"

PhysicBodyManager::PhysicBodyManager(PhysicWorld& physicWorld)
  : _physicWorld(physicWorld) {
  _bodies.pre_allocate(1024);
}

PhysicBodyManager::~PhysicBodyManager() { clear(); }

void PhysicBodyManager::clear() {
  while (!_bodies.is_empty())
    destroyBody(_bodies.get(0));
  _bodies.clear();
}

PhysicBodyManager::BodyWeakRef
PhysicBodyManager::createBody(const PhysicBodyDef& def) {
  return _bodies.acquire(def);
}

PhysicBodyManager::BodyWeakRef
PhysicBodyManager::createAndAddBody(const PhysicBodyDef& def) {
  auto ref = createBody(def);
  addBody(ref, def.group, def.mask);
  return ref;
}

void PhysicBodyManager::destroyBody(PhysicBodyManager::BodyWeakRef ref) {
  if (!ref)
    return;

  removeBody(ref);
  _bodies.release(ref);
}

void PhysicBodyManager::addBody(PhysicBodyManager::BodyWeakRef ref, short group,
                                short mask) {
  if (!ref)
    return;
  PhysicBody* implementation = reinterpret_cast<PhysicBody*>(ref.get());
  if (implementation->_isAdded)
    return;
  _physicWorld._bullet.dynamicsWorld->addRigidBody(implementation->_bullet.body,
                                                   group, mask);
  implementation->_isAdded = true;
}

void PhysicBodyManager::removeBody(PhysicBodyManager::BodyWeakRef ref) {
  if (!ref)
    return;
  PhysicBody* implementation = reinterpret_cast<PhysicBody*>(ref.get());
  if (!implementation->_isAdded)
    return;
  _physicWorld._bullet.dynamicsWorld->removeRigidBody(
    implementation->_bullet.body);
  implementation->_isAdded = false;
}

PhysicBodyManager::BodyWeakRef PhysicBodyManager::getBody(unsigned int index) {
  return _bodies.get(index);
}

const PhysicBodyManager::BodyWeakRef
PhysicBodyManager::getBody(unsigned int index) const {
  return _bodies.get(index);
}

std::size_t PhysicBodyManager::bodySize() const { return _bodies.size(); }

bool PhysicBodyManager::bodyEmpty() const { return _bodies.is_empty(); }
