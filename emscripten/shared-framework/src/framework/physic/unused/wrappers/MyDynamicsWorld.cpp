
#include "MyDynamicsWorld.hpp"

#include <iostream>

namespace /*anonymous*/
{

class btjsClosestNotMeConvexResultCallback
  : public btCollisionWorld::ClosestConvexResultCallback {
public:
  btCollisionObject* m_me;
  btScalar m_allowedPenetration;
  btOverlappingPairCache* m_pairCache;
  btDispatcher* m_dispatcher;

public:
  btjsClosestNotMeConvexResultCallback(btCollisionObject* me,
                                       const btVector3& fromA,
                                       const btVector3& toA,
                                       btOverlappingPairCache* pairCache,
                                       btDispatcher* dispatcher)
    : btCollisionWorld::ClosestConvexResultCallback(fromA, toA), m_me(me),
      m_allowedPenetration(0.0f), m_pairCache(pairCache),
      m_dispatcher(dispatcher) {}

  virtual btScalar
  addSingleResult(btCollisionWorld::LocalConvexResult& convexResult,
                  bool normalInWorldSpace) override {
    if (convexResult.m_hitCollisionObject == m_me)
      return 1.0f;

    // ignore result if there is no contact response
    if (!convexResult.m_hitCollisionObject->hasContactResponse())
      return 1.0f;

    btVector3 linVelA, linVelB;
    linVelA = m_convexToWorld - m_convexFromWorld;
    linVelB = btVector3(0, 0, 0); // toB.getOrigin()-fromB.getOrigin();

    btVector3 relativeVelocity = (linVelA - linVelB);
    // don't report time of impact for motion away from the contact normal (or
    // causes minor penetration)
    if (convexResult.m_hitNormalLocal.dot(relativeVelocity) >=
        -m_allowedPenetration)
      return 1.f;

    return ClosestConvexResultCallback::addSingleResult(convexResult,
                                                        normalInWorldSpace);
  }

  virtual bool needsCollision(btBroadphaseProxy* proxy0) const override {
    // don't collide with itself
    if (proxy0->m_clientObject == m_me)
      return false;

    /// don't do CCD when the collision filters are not matching
    if (!ClosestConvexResultCallback::needsCollision(proxy0))
      return false;

    btCollisionObject* otherObj = (btCollisionObject*)proxy0->m_clientObject;

    // call needsResponse, see
    // http://code.google.com/p/bullet/issues/detail?id=179
    if (m_dispatcher->needsResponse(m_me, otherObj))
      return true;

    return false;
  }
};

}; // namespace

MyDynamicsWorld::MyDynamicsWorld(
  btDispatcher* dispatcher, btBroadphaseInterface* pairCache,
  btConstraintSolver* constraintSolver,
  btCollisionConfiguration* collisionConfiguration)
  : btDiscreteDynamicsWorld(dispatcher, pairCache, constraintSolver,
                            collisionConfiguration) {}

MyDynamicsWorld::~MyDynamicsWorld() {}

//

void MyDynamicsWorld::createPredictiveContactsInternal_ex(btRigidBody** bodies,
                                                          int numBodies,
                                                          btScalar timeStep) {
  btTransform predictedTrans;
  for (int i = 0; i < numBodies; ++i) {
    btRigidBody* body = bodies[i];
    body->setHitFraction(1.f);

    if (body->isActive() && (!body->isStaticOrKinematicObject())) {
      body->predictIntegratedTransform(timeStep, predictedTrans);

      btScalar squareMotion =
        (predictedTrans.getOrigin() - body->getWorldTransform().getOrigin())
          .length2();

      if (getDispatchInfo().m_useContinuous &&
          body->getCcdSquareMotionThreshold() &&
          body->getCcdSquareMotionThreshold() < squareMotion) {
        BT_PROFILE("predictive convexSweepTest");
        if (body->getCollisionShape()->isConvex()) {
          btjsClosestNotMeConvexResultCallback sweepResults(
            body, body->getWorldTransform().getOrigin(),
            predictedTrans.getOrigin(),
            getBroadphase()->getOverlappingPairCache(), getDispatcher());

          // btConvexShape* convexShape =
          // static_cast<btConvexShape*>(body->getCollisionShape());
          btSphereShape tmpSphere(body->getCcdSweptSphereRadius());
          sweepResults.m_allowedPenetration =
            getDispatchInfo().m_allowedCcdPenetration;

          sweepResults.m_collisionFilterGroup =
            body->getBroadphaseProxy()->m_collisionFilterGroup;
          sweepResults.m_collisionFilterMask =
            body->getBroadphaseProxy()->m_collisionFilterMask;
          btTransform modifiedPredictedTrans = predictedTrans;
          modifiedPredictedTrans.setBasis(body->getWorldTransform().getBasis());

          convexSweepTest(&tmpSphere, body->getWorldTransform(),
                          modifiedPredictedTrans, sweepResults);
          if (sweepResults.hasHit() &&
              (sweepResults.m_closestHitFraction < 1.f)) {
            btVector3 distVec = (predictedTrans.getOrigin() -
                                 body->getWorldTransform().getOrigin()) *
                                sweepResults.m_closestHitFraction;
            btScalar distance = distVec.dot(-sweepResults.m_hitNormalWorld);

            btPersistentManifold* manifold = m_dispatcher1->getNewManifold(
              body, sweepResults.m_hitCollisionObject);
            btMutexLock(&m_predictiveManifoldsMutex);
            m_predictiveManifolds.push_back(manifold);
            btMutexUnlock(&m_predictiveManifoldsMutex);

            btVector3 worldPointB =
              body->getWorldTransform().getOrigin() + distVec;
            btVector3 localPointB =
              sweepResults.m_hitCollisionObject->getWorldTransform().inverse() *
              worldPointB;

            btManifoldPoint newPoint(btVector3(0, 0, 0), localPointB,
                                     sweepResults.m_hitNormalWorld, distance);

            bool isPredictive = true;
            int index = manifold->addManifoldPoint(newPoint, isPredictive);
            btManifoldPoint& pt = manifold->getContactPoint(index);
            pt.m_combinedRestitution = 0;
            pt.m_combinedFriction = btManifoldResult::calculateCombinedFriction(
              body, sweepResults.m_hitCollisionObject);
            pt.m_positionWorldOnA = body->getWorldTransform().getOrigin();
            pt.m_positionWorldOnB = worldPointB;

            if (gContactStartedCallback)
              gContactStartedCallback(manifold);

            if (gContactEndedCallback)
              gContactEndedCallback(manifold);
          }
        }
      }
    }
  }
}

void MyDynamicsWorld::createPredictiveContacts(btScalar timeStep) {
  BT_PROFILE("createPredictiveContacts");
  releasePredictiveContacts();
  if (m_nonStaticRigidBodies.size() > 0) {
    createPredictiveContactsInternal_ex(
      &m_nonStaticRigidBodies[0], m_nonStaticRigidBodies.size(), timeStep);
  }
}
