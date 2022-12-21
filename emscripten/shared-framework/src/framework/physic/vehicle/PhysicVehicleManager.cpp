
#include "PhysicVehicleManager.hpp"

#include "framework/physic/PhysicWorld.hpp"

#include "framework/helpers/internals/BulletPhysics.hpp"

PhysicVehicleManager::PhysicVehicleManager(PhysicWorld& physicWorld)
  : _physicWorld(physicWorld) {
  _vehicles.pre_allocate(1024);
}

PhysicVehicleManager::~PhysicVehicleManager() { clear(); }

void PhysicVehicleManager::clear() {
  while (!_vehicles.is_empty())
    destroyVehicle(_vehicles.get(0));
  _vehicles.clear();
}

PhysicVehicleManager::VehicleWeakRef
PhysicVehicleManager::createVehicle(const PhysicVehicleDef& def) {
  return _vehicles.acquire(*_physicWorld._bullet.dynamicsWorld, def);
}

PhysicVehicleManager::VehicleWeakRef
PhysicVehicleManager::createAndAddVehicle(const PhysicVehicleDef& def) {
  auto ref = createVehicle(def);
  addVehicle(ref);
  return ref;
}

void PhysicVehicleManager::destroyVehicle(
  PhysicVehicleManager::VehicleWeakRef ref) {
  if (!ref)
    return;

  removeVehicle(ref);
  _vehicles.release(ref);
}

void PhysicVehicleManager::addVehicle(
  PhysicVehicleManager::VehicleWeakRef ref) {
  if (!ref)
    return;
  PhysicVehicle* implementation = reinterpret_cast<PhysicVehicle*>(ref.get());
  if (implementation->_isAdded)
    return;
  _physicWorld._bullet.dynamicsWorld->addVehicle(
    implementation->_bullet.vehicle);
  implementation->_isAdded = true;
}

void PhysicVehicleManager::removeVehicle(
  PhysicVehicleManager::VehicleWeakRef ref) {
  if (!ref)
    return;
  PhysicVehicle* implementation = reinterpret_cast<PhysicVehicle*>(ref.get());
  if (!implementation->_isAdded)
    return;
  _physicWorld._bullet.dynamicsWorld->removeVehicle(
    implementation->_bullet.vehicle);
  implementation->_isAdded = false;
}

PhysicVehicleManager::VehicleWeakRef
PhysicVehicleManager::getVehicle(unsigned int index) {
  return _vehicles.get(index);
}

const PhysicVehicleManager::VehicleWeakRef
PhysicVehicleManager::getVehicle(unsigned int index) const {
  return _vehicles.get(index);
}

std::size_t PhysicVehicleManager::vehicleSize() const {
  return _vehicles.size();
}

bool PhysicVehicleManager::vehicleEmpty() const { return _vehicles.is_empty(); }
