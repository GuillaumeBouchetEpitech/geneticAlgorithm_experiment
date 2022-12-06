
#pragma once

#include "helpers/BulletPhysics.hpp"

class MyDynamicsWorld : public btDiscreteDynamicsWorld {
public:
  MyDynamicsWorld(btDispatcher* dispatcher, btBroadphaseInterface* pairCache,
                  btConstraintSolver* constraintSolver,
                  btCollisionConfiguration* collisionConfiguration);

  virtual ~MyDynamicsWorld();

protected:
  void createPredictiveContactsInternal_ex(btRigidBody** bodies, int numBodies,
                                           btScalar timeStep);
  virtual void createPredictiveContacts(btScalar timeStep) override;
};
