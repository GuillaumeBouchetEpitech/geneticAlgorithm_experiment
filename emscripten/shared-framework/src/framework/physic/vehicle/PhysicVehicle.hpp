
#pragma once

#include "PhysicVehicleDef.hpp"

#include "framework/physic/body/PhysicBodyManager.hpp"

class PhysicWorld;
class PhysicVehicleManager;
class PhysicBody;

class btDiscreteDynamicsWorld;
class btDefaultVehicleRaycaster;
class btRaycastVehicle;

class AbstractPhysicVehicle {
public:
  AbstractPhysicVehicle() = default;
  virtual ~AbstractPhysicVehicle() = default;
  AbstractPhysicVehicle(AbstractPhysicVehicle& other) = delete;
  AbstractPhysicVehicle& operator=(const AbstractPhysicVehicle& other) = delete;

public:
  virtual void applyEngineForce(int index, float force) = 0;
  virtual void applyBrake(int index, float force) = 0;
  virtual void setSteeringValue(int index, float steering) = 0;
  virtual void reset() = 0;

public:
  virtual int getNumWheels() const = 0;
  virtual const glm::mat4& getWheelTransform(int index,
                                             glm::mat4& mat4x4) const = 0;
  virtual float getCurrentSpeedKmHour() const = 0;
  virtual PhysicBodyManager::BodyWeakRef getPhysicBody() = 0;
};

class PhysicVehicle : public AbstractPhysicVehicle {
  friend PhysicWorld;
  friend PhysicVehicleManager;

private:
  struct Bullet {
    btDefaultVehicleRaycaster* vehicleRayCaster = nullptr;
    btRaycastVehicle* vehicle = nullptr;
  } _bullet;

  PhysicBodyManager::BodyWeakRef _body;

  bool _isAdded = false;

public:
  PhysicVehicle(btDiscreteDynamicsWorld& dynamicsWorld,
                const PhysicVehicleDef& def);
  virtual ~PhysicVehicle();

  PhysicVehicle(PhysicVehicle&& other);
  PhysicVehicle& operator=(PhysicVehicle&& other);

public:
  virtual void applyEngineForce(int index, float force) override;
  virtual void applyBrake(int index, float force) override;
  virtual void setSteeringValue(int index, float steering) override;
  virtual void reset() override;

public:
  virtual int getNumWheels() const override;
  virtual const glm::mat4& getWheelTransform(int index,
                                             glm::mat4& mat4x4) const override;
  virtual float getCurrentSpeedKmHour() const override;
  virtual PhysicBodyManager::BodyWeakRef getPhysicBody() override;
};
