
#pragma once

#include "framework/containers/weak_ref_data_pool.hpp"

#include "PhysicVehicle.hpp"
#include "PhysicVehicleDef.hpp"

class PhysicWorld;

class PhysicVehicleManager {
  friend PhysicWorld;

public:
  using VehiclesPool =
    weak_ref_data_pool<PhysicVehicle, AbstractPhysicVehicle, 256, false>;
  using VehicleWeakRef = VehiclesPool::weak_ref;

private:
  PhysicWorld& _physicWorld;
  VehiclesPool _vehicles;

private:
  PhysicVehicleManager(PhysicWorld& physicWorld);
  ~PhysicVehicleManager();

private:
  void clear();

public:
  VehicleWeakRef createVehicle(const PhysicVehicleDef& def);
  VehicleWeakRef createAndAddVehicle(const PhysicVehicleDef& def);
  void destroyVehicle(VehicleWeakRef ref);

  void addVehicle(VehicleWeakRef ref);
  void removeVehicle(VehicleWeakRef ref);

  VehicleWeakRef getVehicle(unsigned int index);
  const VehicleWeakRef getVehicle(unsigned int index) const;
  std::size_t vehicleSize() const;
  bool vehicleEmpty() const;
};
