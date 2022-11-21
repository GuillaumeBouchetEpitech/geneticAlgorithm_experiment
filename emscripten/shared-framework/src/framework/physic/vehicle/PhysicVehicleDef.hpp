
#pragma once

#include "framework/physic/body/PhysicBodyManager.hpp"

#include "framework/helpers/GLMath.hpp"

#include <vector>

class PhysicBody;
class AbstractPhysicBody;

struct PhysicVehicleDef
{
  PhysicBodyManager::BodyWeakRef body;

  short wheelsCollisionGroup = -1;
  short wheelsCollisionMask = -1;

  glm::ivec3 coordinateSystem =
  {
    0, // rightIndex -> X
    2, // upIndex -> Z
    1, // forwardIndex -> Y
  };

  struct WheelStats
  {
    // relative to tthe center of the vehicle
    glm::vec3 connectionPoint = { 0, 0, 0 };

    // not sure if required, but just in case...
    bool isFrontWheel = false;

    // down axis: -Z (toward the ground)
    glm::vec3 wheelDirectionCS0 = { 0.0f, 0.0f, -1.0f };

    // rotation axis: +X
    glm::vec3 wheelAxleCS = { 1.0f, 0.0f, 0.0f };

    // The maximum length of the suspension (metres)
    float suspensionRestLength = 0.3f;

    float wheelRadius = 0.5f;

    // The stiffness constant for the suspension.
    // => 10.0: "Offroad buggy"
    // => 50.0: "Sports car"
    // => 200.0: "F1 Car"
    float suspensionStiffness = 100.0f; // <= "Sports/F1 Car"

    // The maximum distance the suspension can be compressed (centimetres)
    float maxSuspensionTravelCm = 20.0f; // <= 0.2 metres

    // The damping coefficient for when the suspension is expanding.
    // See the comments for m_wheelsDampingCompression for how to set k.
    // m_wheelsDampingRelaxation should be slightly larger than m_wheelsDampingCompression, eg 0.2 to 0.5
    float wheelsDampingRelaxation = 0.5f;

    // The damping coefficient for when the suspension is compressed.
    // Set to k * 2.0 * btSqrt(m_suspensionStiffness) so k is proportional to critical damping.
    // k = 0.0 undamped & bouncy, k = 1.0 critical damping
    // 0.1 to 0.3 are good values
    float wheelsDampingCompression = 0.3f;

    // The coefficient of friction between the tyre and the ground.
    // Should be about 0.8 for realistic cars, but can increased for better handling.
    // Set large (10000.0) for kart racers
    float wheelFriction = 100.0f; // <= "kart racer"

    // Reduces the rolling torque applied from the wheels that cause the vehicle to roll over.
    // This is a bit of a hack, but it's quite effective. 0.0 = no roll, 1.0 = physical behaviour.
    // If m_frictionSlip is too high, you'll need to reduce this to stop the vehicle rolling over.
    // You should also try lowering the vehicle's centre of mass
    float rollInfluence = 0.5f;
  };

  std::vector<WheelStats> allWheelStats;
};
