
#include "PhysicContactEventsHandler.hpp"

#include "DataPool.hpp"

#include "helpers/BulletPhysics.hpp"

#include "demo/utilities/ErrorHandler.hpp"

namespace /*anonymous*/
{

//
//
// local variable(s)

ContactCallback _contactEventCallback = nullptr;

DataPool<PhysicContactData>* _contactDataPool = nullptr;

//
//
// local function(s)

bool almostEqualRelativeAndAbs(float A, float B, float maxDiff,
                               float maxRelDiff = FLT_EPSILON) {
  // Check if the numbers are really close -- needed
  // when comparing numbers near zero.
  const float diff = fabs(A - B);
  if (diff <= maxDiff)
    return true;

  A = fabs(A);
  B = fabs(B);
  const float largest = (B > A ? B : A);

  if (diff <= largest * maxRelDiff)
    return true;
  return false;
}

bool onContactProcessed(btManifoldPoint& contactPoint, void* pBody0,
                        void* pBody1) {
  btRigidBody* pRigidBody0 = reinterpret_cast<btRigidBody*>(pBody0);
  btRigidBody* pRigidBody1 = reinterpret_cast<btRigidBody*>(pBody1);

  // here the value of the "m_userPersistentData" pointer is used as a int
  int* pContactId = reinterpret_cast<int*>(&contactPoint.m_userPersistentData);
  int contactId = *pContactId;

  const bool isBeginContact = (contactId == 0);

  if (!_contactEventCallback)
    return true;
  // D_THROW("no callback found");

  PhysicContactData* pContactData;

  // Create (on begin) or Get (on update) the contact data
  if (isBeginContact) {
    pContactData = _contactDataPool->pop(); // <= auto-growing internal pool
    const int contactIndex =
      _contactDataPool->getIndex(pContactData); // <= FAST

    contactId = contactIndex + 1; // index => id
    *pContactId = contactId;

    pContactData->id = static_cast<unsigned short>(contactId);
    pContactData->pBodyA = pRigidBody0;
    pContactData->pBodyB = pRigidBody1;
  } else {
    const int contactIndex = contactId - 1; // id => index
    pContactData = _contactDataPool->get(contactIndex);
  }

  btVector3 currPos = contactPoint.m_positionWorldOnB;
  btVector3 currNormal = contactPoint.m_normalWorldOnB;

  auto&& prevPos = pContactData->posB;
  auto&& prevNormal = pContactData->normalB;

  /**
   * On a contact update:
   * => this callback is called even when nothing moved
   * => this block check if there is enough difference
   * => if not, the JS-side callback is not called
   */
  if (!isBeginContact) {
    constexpr float maxDiff = 0.001f;
    constexpr float maxRelDiff = 0.01f;

    const bool needUpdate =
      (!almostEqualRelativeAndAbs(currPos[0], prevPos[0], maxDiff,
                                  maxRelDiff) ||
       !almostEqualRelativeAndAbs(currPos[1], prevPos[1], maxDiff,
                                  maxRelDiff) ||
       !almostEqualRelativeAndAbs(currPos[2], prevPos[2], maxDiff,
                                  maxRelDiff) ||
       !almostEqualRelativeAndAbs(currNormal[0], prevNormal[0], maxDiff,
                                  maxRelDiff) ||
       !almostEqualRelativeAndAbs(currNormal[1], prevNormal[1], maxDiff,
                                  maxRelDiff) ||
       !almostEqualRelativeAndAbs(currNormal[2], prevNormal[2], maxDiff,
                                  maxRelDiff));

    if (!needUpdate)
      return true;
  }

  pContactData->posB = glm::vec3(currPos[0], currPos[1], currPos[2]);
  pContactData->normalB =
    glm::vec3(currNormal[0], currNormal[1], currNormal[2]);

  if (isBeginContact)
    _contactEventCallback(ContactEvent::BEGIN, pContactData);
  else
    _contactEventCallback(ContactEvent::UPDATE, pContactData);

  return true;
};

bool onContactDestroyed(void* userPersistentData) {
  if (!_contactEventCallback)
    return true;
  // D_THROW("no callback found");

  const int* pContactId = reinterpret_cast<int*>(&userPersistentData);
  const int contactId = *pContactId;

  const int contactIndex = contactId - 1; // id => index

  PhysicContactData* pContactData = _contactDataPool->get(contactIndex);
  _contactDataPool->release(contactIndex);

  _contactEventCallback(ContactEvent::END, pContactData);

  return true;
}

} // namespace

namespace PhysicContactEventsHandler {

void initialise(ContactCallback callback) {
  if (_contactEventCallback)
    D_THROW(std::runtime_error, "already initialised");

  _contactDataPool = new DataPool<PhysicContactData>();

  gContactProcessedCallback = onContactProcessed;
  gContactDestroyedCallback = onContactDestroyed;

  _contactEventCallback = callback;
}

}; // namespace PhysicContactEventsHandler
