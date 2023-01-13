
#pragma once

#include "PhysicBodyDef.hpp"

#include "framework/helpers/GLMath.hpp"
#include "framework/physic/shape/PhysicShape.hpp"
#include "framework/system/containers/dynamic_heap_array.hpp"
#include "framework/system/containers/weak_ref_data_pool.hpp"

// #include <vector>
#include <cstdint>
#include <functional>
#include <memory>

class btDefaultMotionState;
class btRigidBody;

class PhysicWorld;
class PhysicBodyManager;
class PhysicVehicle;
// class PhysicHingeConstraint;
// class PhysicFixedConstraint;

class AbstractPhysicBody {
public:
  AbstractPhysicBody() = default;
  virtual ~AbstractPhysicBody() = default;
  AbstractPhysicBody(AbstractPhysicBody& other) = delete;
  AbstractPhysicBody& operator=(const AbstractPhysicBody& other) = delete;

public:
  virtual bool isDynamic() const = 0;

  virtual glm::vec3 getPosition() const = 0;
  virtual glm::mat4& getTransform(glm::mat4& mat4x4) const = 0;

  virtual void setPosition(const glm::vec3& pos) = 0;
  virtual void setRotation(const glm::vec4& rot) = 0;
  virtual void setTransform(const glm::mat4& mat4x4) = 0;

  virtual glm::vec3 getEulerXYZ() const = 0;
  virtual void setEulerXYZ(float x, float y, float z) = 0;
  virtual void setEulerXYZ(const glm::vec3& euler) = 0;

  virtual void setAngularFactor(float x, float y, float z) = 0;
  virtual void setAngularFactor(const glm::vec3& factor) = 0;

  virtual void clearForces() = 0;

  virtual glm::vec3 getLinearVelocity() const = 0;
  virtual glm::vec3 getAngularVelocity() const = 0;
  virtual void setLinearVelocity(float x, float y, float z) = 0;
  virtual void setLinearVelocity(const glm::vec3& vel) = 0;
  virtual void setAngularVelocity(float x, float y, float z) = 0;
  virtual void setAngularVelocity(const glm::vec3& vel) = 0;

  virtual void setGravity(float x, float y, float z) = 0;
  virtual void setGravity(const glm::vec3& grav) = 0;
  virtual glm::vec3 getGravity() const = 0;

  virtual void setFriction(float friction) = 0;
  virtual float getFriction() const = 0;

  virtual void setUserValue(int userValue) = 0;
  virtual int getUserValue() const = 0;

  virtual void setUserData(void* userData) = 0;
  virtual const void* getUserData() const = 0;
  virtual void* getUserData() = 0;

  virtual void applyCentralImpulse(const glm::vec3& acc) = 0;
  virtual void applyImpulse(const glm::vec3& force,
                            const glm::vec3& rel_pos) = 0;
  virtual void applyCentralForce(const glm::vec3& force) = 0;
  virtual void applyForce(const glm::vec3& force, const glm::vec3& rel_pos) = 0;
  virtual void disableSleep() = 0;
  virtual void forceActivate() = 0;

  virtual void disableContactResponse() = 0;
  virtual void enableContactResponse() = 0;
};

class PhysicBody : public AbstractPhysicBody {
  // private:
  friend PhysicWorld;
  friend PhysicBodyManager;
  friend PhysicVehicle;

  // friend weak_ref_data_pool<PhysicBody>;
  // friend dynamic_heap_array<PhysicBody>;
  // friend std::allocator<PhysicBody>;
  // friend std::allocator<weak_ref_data_pool<PhysicBody>::internal_data>;

  // friend PhysicHingeConstraint;
  // friend PhysicFixedConstraint;

private:
  struct Bullet {
    btDefaultMotionState* motionState = nullptr;
    btRigidBody* body = nullptr;
  } _bullet;

  PhysicShape* _shape{nullptr};

  float _mass = 0.0f;

  // std::vector<ContactPoint> _contacts;
  // std::function<void()> _contactCallback;

  int _userValue = 0;
  void* _userData = nullptr;

  bool _isAdded = false;

  // private:
public:
  PhysicBody(const PhysicBodyDef& def);
  virtual ~PhysicBody();

  PhysicBody(PhysicBody& other) = delete;
  PhysicBody(PhysicBody&& other);
  PhysicBody& operator=(const PhysicBody& other) = delete;
  PhysicBody& operator=(PhysicBody&& other) = delete;

  void applySwap(PhysicBody& other);

public:
  virtual bool isDynamic() const override;

  virtual glm::vec3 getPosition() const override;
  virtual glm::mat4& getTransform(glm::mat4& mat4x4) const override;

  virtual void setPosition(const glm::vec3& pos) override;
  virtual void setRotation(const glm::vec4& rot) override;
  virtual void setTransform(const glm::mat4& mat4x4) override;

  virtual glm::vec3 getEulerXYZ() const override;
  virtual void setEulerXYZ(float x, float y, float z) override;
  virtual void setEulerXYZ(const glm::vec3& euler) override;

  virtual void setAngularFactor(float x, float y, float z) override;
  virtual void setAngularFactor(const glm::vec3& factor) override;

  virtual void clearForces() override;

  virtual glm::vec3 getLinearVelocity() const override;
  virtual glm::vec3 getAngularVelocity() const override;
  virtual void setLinearVelocity(float x, float y, float z) override;
  virtual void setLinearVelocity(const glm::vec3& vel) override;
  virtual void setAngularVelocity(float x, float y, float z) override;
  virtual void setAngularVelocity(const glm::vec3& vel) override;

  virtual void setGravity(float x, float y, float z) override;
  virtual void setGravity(const glm::vec3& grav) override;
  virtual glm::vec3 getGravity() const override;

  virtual void setFriction(float friction) override;
  virtual float getFriction() const override;

  virtual void setUserValue(int userValue) override;
  virtual int getUserValue() const override;

  virtual void setUserData(void* userData) override;
  virtual const void* getUserData() const override;
  virtual void* getUserData() override;

  virtual void applyCentralImpulse(const glm::vec3& acc) override;
  virtual void applyImpulse(const glm::vec3& force,
                            const glm::vec3& rel_pos) override;
  virtual void applyCentralForce(const glm::vec3& force) override;
  virtual void applyForce(const glm::vec3& force,
                          const glm::vec3& rel_pos) override;
  virtual void disableSleep() override;
  virtual void forceActivate() override;

  virtual void disableContactResponse() override;
  virtual void enableContactResponse() override;

  // const std::vector<ContactPoint>& getContacts() const;
  // void setContactCallback(const std::function<void()>& callback);
};
