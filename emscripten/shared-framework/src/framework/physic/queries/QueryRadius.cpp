
#include "QueryRadius.hpp"

#include "framework/physic/PhysicWorld.hpp"

#include "framework/ErrorHandler.hpp"
#include "framework/TraceLogger.hpp"

#include "framework/helpers/internals/BulletPhysics.hpp"

QueryRadius::QueryRadius(PhysicWorld& physicWorld)
  : _physicWorld(physicWorld) {}

bool QueryRadius::_queryRadius(QueryRadiusParams& params,
                               QueryRadiusParams::ResultRaw& result) {
  result.allBodiesTotal = 0;

  btSphereShape sphereShape(params.radius);

  btPairCachingGhostObject volume = btPairCachingGhostObject();
  volume.setCollisionShape(&sphereShape);
  volume.setCollisionFlags(volume.getCollisionFlags() |
                           btCollisionObject::CF_NO_CONTACT_RESPONSE);

  btTransform tr;
  tr.setIdentity();
  tr.setOrigin(
    btVector3(params.position.x, params.position.y, params.position.z));
  volume.setWorldTransform(tr);

  /// ContactResultCallback is used to report contact points
  struct MyContactResultCallback
    : public btCollisionWorld::ContactResultCallback {
    int m_bodyUniqueIdA;
    int m_bodyUniqueIdB;
    int m_linkIndexA;
    int m_linkIndexB;
    btScalar m_deltaTime;

    btPairCachingGhostObject& _volume;
    QueryRadiusParams::ResultRaw& _result;

    MyContactResultCallback(btPairCachingGhostObject& volume,
                            QueryRadiusParams::ResultRaw& result)
      : _volume(volume), _result(result) {}

    virtual ~MyContactResultCallback() {}

    virtual bool needsCollision(btBroadphaseProxy* proxy0) const {
      bool collides =
        (proxy0->m_collisionFilterGroup & m_collisionFilterMask) != 0;
      collides =
        collides && (m_collisionFilterGroup & proxy0->m_collisionFilterMask);
      return collides;
    }

    virtual btScalar
    addSingleResult(btManifoldPoint& cp,
                    const btCollisionObjectWrapper* colObj0Wrap, int partId0,
                    int index0, const btCollisionObjectWrapper* colObj1Wrap,
                    int partId1, int index1) {
      static_cast<void>(cp);      // unused
      static_cast<void>(partId0); // unused
      static_cast<void>(index0);  // unused
      static_cast<void>(partId1); // unused
      static_cast<void>(index1);  // unused

      if (colObj0Wrap->m_collisionObject != &_volume) {
        if (_result.allBodiesTotal < _result.allBodiesMaxSize) {
          // check duplicates

          PhysicBody* pBody = static_cast<PhysicBody*>(
            colObj0Wrap->m_collisionObject->getUserPointer());

          bool found = false;
          for (std::size_t ii = 0; ii < _result.allBodiesTotal; ++ii)
            if (_result.allBodiesData[ii] == pBody) {
              found = true;
              break;
            }

          if (found == false) {
            _result.allBodiesData[_result.allBodiesTotal] = pBody;
            _result.allBodiesTotal += 1;
          }
        }
      }

      if (colObj1Wrap->m_collisionObject != &_volume) {
        if (_result.allBodiesTotal < _result.allBodiesMaxSize) {
          // check duplicates

          PhysicBody* pBody = static_cast<PhysicBody*>(
            colObj1Wrap->m_collisionObject->getUserPointer());

          bool found = false;
          for (std::size_t ii = 0; ii < _result.allBodiesTotal; ++ii)
            if (_result.allBodiesData[ii] == pBody) {
              found = true;
              break;
            }

          if (found == false) {
            _result.allBodiesData[_result.allBodiesTotal] = pBody;
            _result.allBodiesTotal += 1;
          }
        }
      }

      return 1;
    }
  };

  MyContactResultCallback cr(volume, result);

  cr.m_collisionFilterGroup = params.collisionGroup;
  cr.m_collisionFilterMask = params.collisionMask;

  _physicWorld._bullet.dynamicsWorld->contactTest(&volume, cr);

  return result.allBodiesTotal > 0;
}
