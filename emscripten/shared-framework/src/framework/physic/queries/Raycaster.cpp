
#include "Raycaster.hpp"

#include "framework/helpers/internals/BulletPhysics.hpp"
#include "framework/physic/PhysicWorld.hpp"
#include "framework/system/ErrorHandler.hpp"
#include "framework/system/TraceLogger.hpp"

Raycaster::Raycaster(PhysicWorld& physicWorld) : _physicWorld(physicWorld) {}

void Raycaster::_normalRaycast(RaycastParams& params,
                               RaycastParams::ResultRaw& result) {
  // normal raycast

  btVector3 rayFrom(params.from.x, params.from.y, params.from.z);
  btVector3 rayTo(params.to.x, params.to.y, params.to.z);

  struct CustomRayResultCallback : public btCollisionWorld::RayResultCallback {
    CustomRayResultCallback(const btVector3& rayFromWorld,
                            const btVector3& rayToWorld, void* toIgnore,
                            Raycaster::RaycastParams::Type type,
                            Raycaster::RaycastParams::ResultRaw& result)
      : m_rayFromWorld(rayFromWorld), m_rayToWorld(rayToWorld),
        m_result(result), m_toIgnore(toIgnore), m_type(type) {}

    btVector3 m_rayFromWorld; // used to calculate hitPointWorld from
                              // hitFraction
    btVector3 m_rayToWorld;

    Raycaster::RaycastParams::ResultRaw& m_result;

    void* m_toIgnore;
    Raycaster::RaycastParams::Type m_type;

    btVector3 m_hitNormalWorld;
    btVector3 m_hitPointWorld;

    virtual btScalar
    addSingleResult(btCollisionWorld::LocalRayResult& rayResult,
                    bool normalInWorldSpace) {
      // //caller already does the filter on the m_closestHitFraction
      // btAssert(rayResult.m_hitFraction <= m_closestHitFraction);

      m_closestHitFraction = rayResult.m_hitFraction;
      m_collisionObject = rayResult.m_collisionObject;
      if (normalInWorldSpace) {
        m_hitNormalWorld = rayResult.m_hitNormalLocal;
      } else {
        /// need to transform normal into worldspace
        m_hitNormalWorld = m_collisionObject->getWorldTransform().getBasis() *
                           rayResult.m_hitNormalLocal;
      }
      m_hitPointWorld.setInterpolate3(m_rayFromWorld, m_rayToWorld,
                                      rayResult.m_hitFraction);

      if (m_type == Raycaster::RaycastParams::Type::closest)
        return rayResult.m_hitFraction;

      const btRigidBody* body = btRigidBody::upcast(m_collisionObject);
      if (body && body->hasContactResponse()) {
        glm::vec3 impactPoint =
          glm::vec3(m_hitPointWorld[0], m_hitPointWorld[1], m_hitPointWorld[2]);
        glm::vec3 impactNormal = glm::vec3(
          m_hitNormalWorld[0], m_hitNormalWorld[1], m_hitNormalWorld[2]);

        void* userData = body->getUserPointer();
        if (userData != m_toIgnore) {
          PhysicBody* physicBody = nullptr;
          if (userData) {
            physicBody = static_cast<PhysicBody*>(userData);
          }

          if (m_result.allImpactsTotal < m_result.allImpactsMaxSize) {
            m_result.allImpactsData[m_result.allImpactsTotal] = {
              impactPoint, impactNormal, physicBody};
            m_result.allImpactsTotal += 1;
          }
        }
      }

      return 1.0f;
    }
  };

  CustomRayResultCallback rayCallback(rayFrom, rayTo, params.toIgnore,
                                      params.type, result);
  rayCallback.m_collisionFilterGroup = params.collisionGroup;
  rayCallback.m_collisionFilterMask = params.collisionMask;

  _physicWorld._bullet.dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);

  result.hasHit = rayCallback.hasHit();

  if (!result.hasHit)
    // return false;
    return;

  if (params.type == RaycastParams::Type::closest) {
    const btRigidBody* body =
      btRigidBody::upcast(rayCallback.m_collisionObject);
    if (body && body->hasContactResponse()) {
      glm::vec3 impactPoint = glm::vec3(rayCallback.m_hitPointWorld[0],
                                        rayCallback.m_hitPointWorld[1],
                                        rayCallback.m_hitPointWorld[2]);
      glm::vec3 impactNormal = glm::vec3(rayCallback.m_hitNormalWorld[0],
                                         rayCallback.m_hitNormalWorld[1],
                                         rayCallback.m_hitNormalWorld[2]);

      void* userData = body->getUserPointer();
      if (userData != params.toIgnore) {
        PhysicBody* physicBody = nullptr;
        if (userData) {
          physicBody = static_cast<PhysicBody*>(userData);
        }

        if (result.allImpactsTotal < result.allImpactsMaxSize) {
          result.allImpactsData[result.allImpactsTotal] = {
            impactPoint, impactNormal, physicBody};
          result.allImpactsTotal += 1;
        }
      }
    }
  }
}

void Raycaster::_convexSweep(RaycastParams& params,
                             RaycastParams::ResultRaw& result) {

  btVector3 rayFrom(params.from.x, params.from.y, params.from.z);
  btVector3 rayTo(params.to.x, params.to.y, params.to.z);

  // replace raycast with convex sweep test

  btSphereShape sphereShape(params.radius);

  btTransform from;
  from.setIdentity();
  from.setOrigin(rayFrom);

  btTransform to;
  to.setIdentity();
  to.setOrigin(rayTo);

  struct CustomConvexResultCallback
    : public btCollisionWorld::ConvexResultCallback {
    CustomConvexResultCallback(const btVector3& convexFromWorld,
                               const btVector3& convexToWorld, void* toIgnore,
                               Raycaster::RaycastParams::Type type,
                               Raycaster::RaycastParams::ResultRaw& result)
      : m_convexFromWorld(convexFromWorld), m_convexToWorld(convexToWorld),
        m_result(result), m_toIgnore(toIgnore), m_type(type),
        m_hitCollisionObject(0) {}

    btVector3
      m_convexFromWorld; // used to calculate hitPointWorld from hitFraction
    btVector3 m_convexToWorld;
    Raycaster::RaycastParams::ResultRaw& m_result;

    void* m_toIgnore;
    Raycaster::RaycastParams::Type m_type;

    btVector3 m_hitNormalWorld;
    btVector3 m_hitPointWorld;
    const btCollisionObject* m_hitCollisionObject;

    virtual btScalar
    addSingleResult(btCollisionWorld::LocalConvexResult& convexResult,
                    bool normalInWorldSpace) {
      // caller already does the filter on the m_closestHitFraction
      btAssert(convexResult.m_hitFraction <= m_closestHitFraction);

      m_closestHitFraction = convexResult.m_hitFraction;
      m_hitCollisionObject = convexResult.m_hitCollisionObject;
      if (normalInWorldSpace) {
        m_hitNormalWorld = convexResult.m_hitNormalLocal;
      } else {
        /// need to transform normal into worldspace
        m_hitNormalWorld =
          m_hitCollisionObject->getWorldTransform().getBasis() *
          convexResult.m_hitNormalLocal;
      }
      m_hitPointWorld = convexResult.m_hitPointLocal;

      if (m_type == Raycaster::RaycastParams::Type::closest)
        return convexResult.m_hitFraction;

      const btRigidBody* body = btRigidBody::upcast(m_hitCollisionObject);
      if (body && body->hasContactResponse()) {
        glm::vec3 impactPoint =
          glm::vec3(m_hitPointWorld[0], m_hitPointWorld[1], m_hitPointWorld[2]);
        glm::vec3 impactNormal = glm::vec3(
          m_hitNormalWorld[0], m_hitNormalWorld[1], m_hitNormalWorld[2]);

        void* userData = body->getUserPointer();
        if (userData != m_toIgnore) {
          PhysicBody* physicBody = nullptr;
          if (userData) {
            physicBody = static_cast<PhysicBody*>(userData);
          }

          if (m_result.allImpactsTotal < m_result.allImpactsMaxSize) {
            m_result.allImpactsData[m_result.allImpactsTotal] = {
              impactPoint, impactNormal, physicBody};
            m_result.allImpactsTotal += 1;
          }
        }
      }

      return 1.0f;
    }
  };

  CustomConvexResultCallback sweepCallback(rayFrom, rayTo, params.toIgnore,
                                           params.type, result);
  sweepCallback.m_collisionFilterGroup = params.collisionGroup;
  sweepCallback.m_collisionFilterMask = params.collisionMask;

  btScalar allowedCcdPenetration = 0.0f;

  _physicWorld._bullet.dynamicsWorld->convexSweepTest(
    &sphereShape, from, to, sweepCallback, allowedCcdPenetration);

  result.hasHit = sweepCallback.hasHit();

  if (!result.hasHit)
    // return false;
    return;

  if (params.type == RaycastParams::Type::closest) {
    const btRigidBody* body =
      btRigidBody::upcast(sweepCallback.m_hitCollisionObject);
    if (body && body->hasContactResponse()) {
      glm::vec3 impactPoint = glm::vec3(sweepCallback.m_hitPointWorld[0],
                                        sweepCallback.m_hitPointWorld[1],
                                        sweepCallback.m_hitPointWorld[2]);
      glm::vec3 impactNormal = glm::vec3(sweepCallback.m_hitNormalWorld[0],
                                         sweepCallback.m_hitNormalWorld[1],
                                         sweepCallback.m_hitNormalWorld[2]);

      void* userData = body->getUserPointer();
      if (userData != params.toIgnore) {
        PhysicBody* physicBody = nullptr;
        if (userData) {
          physicBody = static_cast<PhysicBody*>(userData);
        }

        if (result.allImpactsTotal < result.allImpactsMaxSize) {
          result.allImpactsData[result.allImpactsTotal] = {
            impactPoint, impactNormal, physicBody};
          result.allImpactsTotal += 1;
        }
      }
    }
  }
}

bool Raycaster::_raycast(RaycastParams& params,
                         RaycastParams::ResultRaw& result) {
  result.allImpactsTotal = 0;

  if (params.radius <= 0.0f) {
    _normalRaycast(params, result);
  } else {
    _convexSweep(params, result);
  }

  return result.allImpactsTotal > 0;
}
