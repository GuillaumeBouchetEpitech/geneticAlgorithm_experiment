
#include "PhysicWorld.hpp"

#include "body/PhysicBody.hpp"
#include "vehicle/PhysicVehicle.hpp"

#include "framework/helpers/internals/BulletPhysics.hpp"
#include "framework/system/TraceLogger.hpp"

#include <algorithm>
#include <array>
#include <unordered_set>

class MyDebugDrawer : public btIDebugDraw {
private:
  int _debugMode = 0;

  PhysicWorld::debuggerPushLineCallback _debuggerPushLineCallback;

public:
  MyDebugDrawer(const PhysicWorld::debuggerPushLineCallback& callback)
    : _debuggerPushLineCallback(callback) {}

public:
  virtual void drawLine(const btVector3& from, const btVector3& to,
                        const btVector3& color) override {
    if (!_debuggerPushLineCallback)
      return;

    _debuggerPushLineCallback(glm::vec3(from[0], from[1], from[2]),
                              glm::vec3(to[0], to[1], to[2]),
                              glm::vec3(color[0], color[1], color[2]));
  }

  virtual void drawContactPoint(const btVector3& PointOnB,
                                const btVector3& normalOnB, btScalar distance,
                                int lifeTime, const btVector3& color) override {
    static_cast<void>(distance); // unused
    static_cast<void>(lifeTime); // unused

    glm::vec3 point = glm::vec3(PointOnB[0], PointOnB[1], PointOnB[2]);
    glm::vec3 normal = glm::vec3(normalOnB[0], normalOnB[1], normalOnB[2]);

    _debuggerPushLineCallback(point, point + normal * 0.5f,
                              glm::vec3(color[0], color[1], color[2]));
  }

  virtual void reportErrorWarning(const char* warningString) override {
    D_MYLOG("warningString: " << warningString);
  }

  virtual void draw3dText(const btVector3& location,
                          const char* textString) override {
    static_cast<void>(location); // unused

    D_MYLOG("textString: " << textString);
  }

  virtual void setDebugMode(int debugMode) override { _debugMode = debugMode; }

  virtual int getDebugMode() const override { return _debugMode; }
};

// PhysicWorld* PhysicWorld::self = nullptr;

PhysicWorld::PhysicWorld()
  : _physicBodyManager(*this), _physicVehicleManager(*this), _raycaster(*this),
    _queryRadius(*this) {
  // PhysicWorld::self = this;

  _bullet.broadphase = new btDbvtBroadphase();
  _bullet.collisionConfiguration = new btDefaultCollisionConfiguration();
  _bullet.dispatcher =
    new btCollisionDispatcher(_bullet.collisionConfiguration);
  _bullet.solver = new btSequentialImpulseConstraintSolver;
  // _bullet.dynamicsWorld = new MyDynamicsWorld(
  _bullet.dynamicsWorld =
    new btDiscreteDynamicsWorld(_bullet.dispatcher, _bullet.broadphase,
                                _bullet.solver, _bullet.collisionConfiguration);
  _bullet.dynamicsWorld->setGravity(btVector3(0, 0, -10));

  // PhysicContactEventsHandler::initialise([](ContactEvent event,
  // PhysicContactData* contactData)
  // {
  //   // D_MYLOG("event=" << int(event));

  //   PhysicBody* pPhysicBodyA =
  //   static_cast<PhysicBody*>(contactData->pBodyA->getUserPointer());
  //   PhysicBody* pPhysicBodyB =
  //   static_cast<PhysicBody*>(contactData->pBodyB->getUserPointer());

  //   pPhysicBodyA->_onContactChange(event, contactData, false);
  //   pPhysicBodyB->_onContactChange(event, contactData, true);
  // });
}

PhysicWorld::~PhysicWorld() {
  btIDebugDraw* currentDebugDrawer = _bullet.dynamicsWorld->getDebugDrawer();
  if (currentDebugDrawer)
    delete currentDebugDrawer;

  _physicBodyManager.clear();
  _physicVehicleManager.clear();

  delete _bullet.dynamicsWorld;
  delete _bullet.solver;
  delete _bullet.collisionConfiguration;
  delete _bullet.broadphase;
  delete _bullet.dispatcher;
}

void PhysicWorld::setDebuggerPushLine(
  const debuggerPushLineCallback& callback) {
  // _debuggerPushLineCallback = callback;

  btIDebugDraw* currentDebugDrawer = _bullet.dynamicsWorld->getDebugDrawer();
  if (currentDebugDrawer)
    delete currentDebugDrawer;

  MyDebugDrawer* newDebugDrawer = new MyDebugDrawer(callback);
  newDebugDrawer->setDebugMode(
    btIDebugDraw::DebugDrawModes::DBG_DrawWireframe
    // | btIDebugDraw::DebugDrawModes::DBG_DrawAabb
    | btIDebugDraw::DebugDrawModes::DBG_DrawContactPoints |
    btIDebugDraw::DebugDrawModes::DBG_DrawConstraints
    // | btIDebugDraw::DebugDrawModes::DBG_DrawConstraintLimits
  );

  _bullet.dynamicsWorld->setDebugDrawer(newDebugDrawer);
}

void PhysicWorld::step(float elapsedTime, uint32_t maxSubSteps,
                       float fixedTimeStep) {
  _bullet.dynamicsWorld->stepSimulation(elapsedTime, int(maxSubSteps),
                                        fixedTimeStep);
}

void PhysicWorld::render() { _bullet.dynamicsWorld->debugDrawWorld(); }

//
//
//
//
//

PhysicBodyManager& PhysicWorld::getPhysicBodyManager() {
  return _physicBodyManager;
}

const PhysicBodyManager& PhysicWorld::getPhysicBodyManager() const {
  return _physicBodyManager;
}

//
//
//
//
//

PhysicVehicleManager& PhysicWorld::getPhysicVehicleManager() {
  return _physicVehicleManager;
}

const PhysicVehicleManager& PhysicWorld::getPhysicVehicleManager() const {
  return _physicVehicleManager;
}

//
//
//
//
//

Raycaster& PhysicWorld::getRaycaster() { return _raycaster; }

QueryRadius& PhysicWorld::getQueryRadius() { return _queryRadius; }

//

// PhysicHingeConstraint* PhysicWorld::addHingeConstraint(const
// PhysicHingeConstraintDef& def)
// {
//   PhysicHingeConstraint* constraint = new PhysicHingeConstraint(def);

//   _bullet.dynamicsWorld->addConstraint(constraint->_bullet.constraint);

//   _hinge_constraints.push_back(constraint);

//   return constraint;
// }

// void PhysicWorld::destroyHingeConstraint(PhysicHingeConstraint* constraint)
// {
//   auto it = std::find(_hinge_constraints.begin(), _hinge_constraints.end(),
//   constraint); if (it == _hinge_constraints.end())
//     return;

//   _bullet.dynamicsWorld->removeConstraint(constraint->_bullet.constraint);
//   delete constraint;

//   _hinge_constraints.erase(it);
// }

// //

// PhysicFixedConstraint* PhysicWorld::addFixedConstraint(const
// PhysicFixedConstraintDef& def)
// {
//   PhysicFixedConstraint* constraint = new PhysicFixedConstraint(def);

//   const bool disableCollisionsBetweenLinkedBodies = true;

//   _bullet.dynamicsWorld->addConstraint(
//     constraint->_bullet.constraint,
//     disableCollisionsBetweenLinkedBodies
//   );

//   _fixed_constraints.push_back(constraint);

//   return constraint;
// }

// void PhysicWorld::destroyFixedConstraint(PhysicFixedConstraint* constraint)
// {
//   auto it = std::find(_fixed_constraints.begin(), _fixed_constraints.end(),
//   constraint); if (it == _fixed_constraints.end())
//     return;

//   _bullet.dynamicsWorld->removeConstraint(constraint->_bullet.constraint);
//   delete constraint;

//   _fixed_constraints.erase(it);
// }

//
