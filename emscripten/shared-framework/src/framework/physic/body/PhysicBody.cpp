
#include "PhysicBody.hpp"

#include "framework/ErrorHandler.hpp"
#include "framework/TraceLogger.hpp"

#include "framework/helpers/internals/BulletPhysics.hpp"

#include <algorithm>

PhysicBody::PhysicBody(const PhysicBodyDef& def) {
  _mass = std::max(def.mass, 0.0f);

  _shape = PhysicShape::create(def.shape, _mass > 0.0f);

  // _bullet.motionState = new
  // btDefaultMotionState(btTransform(btQuaternion(0.0, 0.0, 0.0, 1.0),
  // btVector3(0, 0, 0)));
  _bullet.motionState = nullptr;

  btVector3 localInertia(0, 0, 0);
  _shape->getRawShape()->calculateLocalInertia(_mass, localInertia);

  btRigidBody::btRigidBodyConstructionInfo rbInfo(
    _mass, _bullet.motionState, _shape->getRawShape(), localInertia);

  _bullet.body = new btRigidBody(rbInfo);

  // float continuous_radius = std::max(def.continuous_radius, 0.0f);
  // if (continuous_radius > 0.0f)
  // {
  //     _bullet.body->setCcdMotionThreshold(continuous_radius * 0.25f);
  //     _bullet.body->setCcdSweptSphereRadius(continuous_radius);
  // }

  _bullet.body->setUserPointer(this);
}

PhysicBody::~PhysicBody() {
  delete _bullet.body;
  delete _bullet.motionState;
  delete _shape;
}

PhysicBody::PhysicBody(PhysicBody&& other) {
  std::swap(_bullet.motionState, other._bullet.motionState);
  std::swap(_bullet.body, other._bullet.body);
  std::swap(_shape, other._shape);
  std::swap(_mass, other._mass);
  std::swap(_userValue, other._userValue);
  std::swap(_userData, other._userData);
  std::swap(_isAdded, other._isAdded);
}

PhysicBody& PhysicBody::operator=(PhysicBody&& other) {
  if (this == &other)
    return *this;

  std::swap(_bullet.motionState, other._bullet.motionState);
  std::swap(_bullet.body, other._bullet.body);
  std::swap(_shape, other._shape);
  std::swap(_mass, other._mass);
  std::swap(_userValue, other._userValue);
  std::swap(_userData, other._userData);
  std::swap(_isAdded, other._isAdded);
  return *this;
}

// void PhysicBody::_onContactChange(ContactEvent event, PhysicContactData*
// contactData, bool isB)
// {
//   if (event == ContactEvent::BEGIN)
//   {
//     ContactPoint newContact;
//     newContact.id = contactData->id;
//     newContact.position = contactData->posB;
//     newContact.normal = isB ? -contactData->normalB : +contactData->normalB;

//     btRigidBody* pOtherBody = isB ? contactData->pBodyA :
//     contactData->pBodyB; newContact.other =
//     static_cast<PhysicBody*>(pOtherBody->getUserPointer());

//     _contacts.push_back(newContact);
//   }
//   else if (event == ContactEvent::UPDATE)
//   {
//     auto it = std::find_if(_contacts.begin(), _contacts.end(),
//     [contactData](const ContactPoint& cp)
//     {
//       return cp.id == contactData->id;
//     });
//     if (it == _contacts.end())
//     {
//       D_MYLOG("contact was not found during an update, id=" <<
//       contactData->id); return;
//     }

//     it->position = contactData->posB;
//     it->normal = isB ? -contactData->normalB : +contactData->normalB;
//   }
//   else
//   {
//     auto it = std::find_if(_contacts.begin(), _contacts.end(),
//     [contactData](const ContactPoint& cp)
//     {
//       return cp.id == contactData->id;
//     });
//     if (it == _contacts.end())
//     {
//       D_MYLOG("contact was not found during a delete, id=" <<
//       contactData->id); return;
//     }
//     _contacts.erase(it);
//   }

//   if (_contactCallback)
//     _contactCallback();
// }

bool PhysicBody::isDynamic() const { return _mass > 0.0f; }

glm::vec3 PhysicBody::getPosition() const {
  auto transform = _bullet.body->getWorldTransform();
  auto origin = transform.getOrigin();

  return glm::vec3(origin[0], origin[1], origin[2]);
}

// glm::quat PhysicBody::getOrientation() const
// {
//     auto transform = _bullet.body->getWorldTransform();
//     auto rotation = transform.getRotation();

//     return glm::quat(rotation[0], rotation[1], rotation[2], rotation[3]);
// }

glm::mat4& PhysicBody::getTransform(glm::mat4& mat4x4) const {
  _bullet.body->getWorldTransform().getOpenGLMatrix(glm::value_ptr(mat4x4));
  return mat4x4;
}

void PhysicBody::setPosition(const glm::vec3& pos) {
  auto& transform = _bullet.body->getWorldTransform();
  transform.setOrigin(btVector3(pos.x, pos.y, pos.z));
}

void PhysicBody::setRotation(const glm::vec4& rot) {
  auto& transform = _bullet.body->getWorldTransform();
  transform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
}

void PhysicBody::setTransform(const glm::mat4& mat4x4) {
  _bullet.body->getWorldTransform().setFromOpenGLMatrix(glm::value_ptr(mat4x4));
}

glm::vec3 PhysicBody::getEulerXYZ() const {
  btTransform& transform = _bullet.body->getWorldTransform();
  btQuaternion rotation = transform.getRotation();

  glm::vec3 out;
  rotation.getEulerZYX(out.x, out.y, out.z);
  return out;
}

void PhysicBody::setEulerXYZ(float x, float y, float z) {
  btQuaternion rotation;
  rotation.setEulerZYX(x, y, z);

  auto& transform = _bullet.body->getWorldTransform();
  transform.setRotation(rotation);
}

void PhysicBody::setEulerXYZ(const glm::vec3& euler) {
  setEulerXYZ(euler.x, euler.y, euler.z);
}

void PhysicBody::setAngularFactor(float x, float y, float z) {
  _bullet.body->setAngularFactor(btVector3(x, y, z));
}

void PhysicBody::setAngularFactor(const glm::vec3& factor) {
  setAngularFactor(factor.x, factor.y, factor.z);
}

void PhysicBody::clearForces() { _bullet.body->clearForces(); }

glm::vec3 PhysicBody::getLinearVelocity() const {
  const btVector3& vel = _bullet.body->getLinearVelocity();

  return glm::vec3(vel[0], vel[1], vel[2]);
}

glm::vec3 PhysicBody::getAngularVelocity() const {
  const btVector3& vel = _bullet.body->getAngularVelocity();

  return glm::vec3(vel[0], vel[1], vel[2]);
}

void PhysicBody::setLinearVelocity(float x, float y, float z) {
  _bullet.body->setLinearVelocity(btVector3(x, y, z));
}

void PhysicBody::setLinearVelocity(const glm::vec3& vel) {
  setLinearVelocity(vel.x, vel.y, vel.z);
}

void PhysicBody::setAngularVelocity(float x, float y, float z) {
  _bullet.body->setAngularVelocity(btVector3(x, y, z));
}

void PhysicBody::setAngularVelocity(const glm::vec3& vel) {
  setAngularVelocity(vel.x, vel.y, vel.z);
}

void PhysicBody::setGravity(float x, float y, float z) {
  _bullet.body->setGravity(btVector3(x, y, z));
}

void PhysicBody::setGravity(const glm::vec3& grav) {
  setGravity(grav.x, grav.y, grav.z);
}

glm::vec3 PhysicBody::getGravity() const {
  const btVector3& grav = _bullet.body->getGravity();

  return glm::vec3(grav[0], grav[1], grav[2]);
}

void PhysicBody::setFriction(float friction) {
  _bullet.body->setFriction(std::max(friction, 0.0f));
}

float PhysicBody::getFriction() const { return _bullet.body->getFriction(); }

void PhysicBody::setUserValue(int userValue) { _userValue = userValue; }
int PhysicBody::getUserValue() const { return _userValue; }

void PhysicBody::setUserData(void* userData) { _userData = userData; }

const void* PhysicBody::getUserData() const { return _userData; }

void* PhysicBody::getUserData() { return _userData; }

void PhysicBody::applyCentralImpulse(const glm::vec3& acc) {
  _bullet.body->applyCentralImpulse(btVector3(acc.x, acc.y, acc.z));
}

void PhysicBody::applyImpulse(const glm::vec3& force,
                              const glm::vec3& rel_pos) {
  _bullet.body->applyImpulse(btVector3(force.x, force.y, force.z),
                             btVector3(rel_pos.x, rel_pos.y, rel_pos.z));
}

void PhysicBody::applyCentralForce(const glm::vec3& force) {
  _bullet.body->applyCentralForce(btVector3(force.x, force.y, force.z));
}

void PhysicBody::applyForce(const glm::vec3& force, const glm::vec3& rel_pos) {
  _bullet.body->applyForce(btVector3(force.x, force.y, force.z),
                           btVector3(rel_pos.x, rel_pos.y, rel_pos.z));
}

void PhysicBody::disableSleep() {
  _bullet.body->setActivationState(DISABLE_DEACTIVATION);
  _bullet.body->activate(true);
}

void PhysicBody::forceActivate() {
  _bullet.body->forceActivationState(ACTIVE_TAG);
  _bullet.body->setDeactivationTime(0);
}

void PhysicBody::disableContactResponse() {
  int newFlags = _bullet.body->getCollisionFlags() |
                 btCollisionObject::CF_NO_CONTACT_RESPONSE;
  _bullet.body->setCollisionFlags(newFlags);
}

void PhysicBody::enableContactResponse() {
  int newFlags = _bullet.body->getCollisionFlags() &
                 ~btCollisionObject::CF_NO_CONTACT_RESPONSE;
  _bullet.body->setCollisionFlags(newFlags);
}

// const std::vector<ContactPoint>& PhysicBody::getContacts() const
// {
//   return _contacts;
// }

// void PhysicBody::setContactCallback(const std::function<void()>& callback)
// {
//   _contactCallback = callback;
// }

// // void* PhysicBody::getUserPointer()
// // {
// //     return _bullet.body->getUserPointer();
// // }
