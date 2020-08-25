
#include "PhysicVehicle.hpp"

#include "thirdparty/BulletPhysics.hpp"

#include "demo/utilities/types.hpp"

#include <array>

PhysicVehicle::PhysicVehicle(btDiscreteDynamicsWorld& dynamicsWorld)
{
    //
    //
    // STATS

    btVector3 wheelDirectionCS0(0.0f, 0.0f, -1.0f); // down axis: -Z (toward the ground)
    btVector3 wheelAxleCS(1.0f, 0.0f, 0.0f); // rotation axis: +X
    float wheelRadius = 0.5f;
    float wheelWidth = 0.2f;
    float wheelSide = wheelWidth * 0.3f;

    // The maximum length of the suspension (metres)
    float suspensionRestLength = 0.3f;

    // The maximum distance the suspension can be compressed (centimetres)
    float maxSuspensionTravelCm = 20.0f; // <= 0.2 metres

    // The coefficient of friction between the tyre and the ground.
    // Should be about 0.8 for realistic cars, but can increased for better handling.
    // Set large (10000.0) for kart racers
    float wheelFriction = 100.0f; // <= "kart racer"

    // The stiffness constant for the suspension.
    // => 10.0: "Offroad buggy"
    // => 50.0: "Sports car"
    // => 200.0: "F1 Car"
    float suspensionStiffness = 100.0f; // <= "Sports/F1 Car"

    // The damping coefficient for when the suspension is compressed.
    // Set to k * 2.0 * btSqrt(m_suspensionStiffness) so k is proportional to critical damping.
    // k = 0.0 undamped & bouncy, k = 1.0 critical damping
    // 0.1 to 0.3 are good values
    float wheelsDampingCompression = 0.3f;

    // The damping coefficient for when the suspension is expanding.
    // See the comments for m_wheelsDampingCompression for how to set k.
    // m_wheelsDampingRelaxation should be slightly larger than m_wheelsDampingCompression, eg 0.2 to 0.5
    float wheelsDampingRelaxation = 0.5f;

    // Reduces the rolling torque applied from the wheels that cause the vehicle to roll over.
    // This is a bit of a hack, but it's quite effective. 0.0 = no roll, 1.0 = physical behaviour.
    // If m_frictionSlip is too high, you'll need to reduce this to stop the vehicle rolling over.
    // You should also try lowering the vehicle's centre of mass
    float rollInfluence = 0.5f;

    //
    //
    // CHASSIS

    float front = 2.0f; // <= Y
    float width = 1.0f; // <= X
    float height = 0.5f; // <= Z
    btVector3 chassisHalfExtents(width, front, height);
    _bullet.chassisShape = new btBoxShape(chassisHalfExtents);

    btTransform localTrans;
    localTrans.setIdentity();
    localTrans.setOrigin(btVector3(0.0f, 0.0f, 0.9f)); // <= elevate chassis
    _bullet.compound = new btCompoundShape();
    _bullet.compound->addChildShape(localTrans, _bullet.chassisShape);

    btScalar mass = 5.0f;
    btVector3 localInertia(0.0f, 0.0f, 0.0f);
    _bullet.compound->calculateLocalInertia(mass, localInertia);

    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, nullptr, _bullet.compound, localInertia);
    _bullet.carChassis = new btRigidBody(cInfo);


    // experimental
    _bullet.carChassis->setFriction(1.0f);


    //
    //
    // VEHICLE

    btRaycastVehicle::btVehicleTuning tuning;
    _bullet.vehicleRayCaster = new btDefaultVehicleRaycaster(&dynamicsWorld);

    _bullet.vehicle = new btRaycastVehicle(tuning, _bullet.carChassis, _bullet.vehicleRayCaster);

    // never allow the deactivation (sleep) state of the vehicle
    _bullet.carChassis->setActivationState(DISABLE_DEACTIVATION);


    //
    //
    // WHEELS

    float connectionHeight = 0.5f;

    // choose coordinate system
    int rightIndex = 0;   // <= X
    int upIndex = 2;      // <= Z
    int forwardIndex = 1; // <= Y
    _bullet.vehicle->setCoordinateSystem(rightIndex, upIndex, forwardIndex);

    struct t_wheel
    {
        btVector3   connectionPoint;
        bool        isFrontWheel;
    };
    std::array<t_wheel, toUnderlying(e_Wheels::eCount)> wheels{{
        // front right
        { { chassisHalfExtents[0] - wheelSide,
            chassisHalfExtents[1] - wheelRadius,
            connectionHeight }, true },
        // front left
        { { -chassisHalfExtents[0] + wheelSide,
            chassisHalfExtents[1] - wheelRadius,
            connectionHeight }, true },
        // rear right
        { { chassisHalfExtents[0] - wheelSide,
            -chassisHalfExtents[1] + wheelRadius,
            connectionHeight }, false },
        // rear left
        { { -chassisHalfExtents[0] + wheelSide,
            -chassisHalfExtents[1] + wheelRadius,
            connectionHeight }, false }
    }};

    for (auto& wheel : wheels)
    {
        btWheelInfo& wheelInfo = _bullet.vehicle->addWheel(
                                            wheel.connectionPoint,
                                            wheelDirectionCS0, wheelAxleCS,
                                            suspensionRestLength, wheelRadius,
                                            tuning, wheel.isFrontWheel
                                        );

        wheelInfo.m_suspensionStiffness = suspensionStiffness;
        wheelInfo.m_maxSuspensionTravelCm = maxSuspensionTravelCm;
        wheelInfo.m_wheelsDampingRelaxation = wheelsDampingRelaxation;
        wheelInfo.m_wheelsDampingCompression = wheelsDampingCompression;
        wheelInfo.m_frictionSlip = wheelFriction;
        wheelInfo.m_rollInfluence = rollInfluence;
    }
}

PhysicVehicle::~PhysicVehicle()
{
    delete _bullet.vehicle;
    delete _bullet.vehicleRayCaster;

    delete _bullet.carChassis;

    delete _bullet.compound;
    delete _bullet.chassisShape;
}

//

void PhysicVehicle::applyEngineForce(float engineForce)
{
    // rear wheels
    _bullet.vehicle->applyEngineForce(engineForce, toUnderlying(e_Wheels::eBackLeft));
    _bullet.vehicle->applyEngineForce(engineForce, toUnderlying(e_Wheels::eBackRight));
}

void PhysicVehicle::setSteeringValue(float vehicleSteering)
{
    // front wheels
    _bullet.vehicle->setSteeringValue(vehicleSteering, toUnderlying(e_Wheels::eFrontLeft));
    _bullet.vehicle->setSteeringValue(vehicleSteering, toUnderlying(e_Wheels::eFrontRight));
}

void PhysicVehicle::reset()
{
    _bullet.carChassis->clearForces();
    _bullet.carChassis->setLinearVelocity(btVector3(0, 0, 0));
    _bullet.carChassis->setAngularVelocity(btVector3(0, 0, 0));
    _bullet.carChassis->forceActivationState(ACTIVE_TAG);
    _bullet.carChassis->setDeactivationTime(0);

    for (unsigned int ii = 0; ii < toUnderlying(e_Wheels::eCount); ++ii)
        _bullet.vehicle->setBrake(1000, ii);

    _bullet.vehicle->resetSuspension();
    _bullet.vehicle->updateVehicle(0);

    for (unsigned int ii = 0; ii < toUnderlying(e_Wheels::eCount); ++ii)
        _bullet.vehicle->setBrake(0, ii);

    // enableContactResponse();
}

// void PhysicVehicle::disableContactResponse()
// {
//     int newFlags = _bullet.carChassis->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE;
//     _bullet.carChassis->setCollisionFlags(newFlags);
// }

// void PhysicVehicle::enableContactResponse()
// {
//     int newFlags = _bullet.carChassis->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE;
//     _bullet.carChassis->setCollisionFlags(newFlags);
// }

void PhysicVehicle::setPosition(const glm::vec3& position)
{
    btVector3 pos(position.x, position.y, position.z);

    btTransform initialTransform = _bullet.carChassis->getWorldTransform();

    initialTransform.setOrigin(pos);

    _bullet.carChassis->setWorldTransform(initialTransform);
    // _bullet.motionState->setWorldTransform(initialTransform);
}

void PhysicVehicle::setRotation(const glm::vec4& rotation)
{
    btQuaternion rot(rotation.x, rotation.y, rotation.z, rotation.w);

    btTransform initialTransform = _bullet.carChassis->getWorldTransform();

    initialTransform.setRotation(rot);

    _bullet.carChassis->setWorldTransform(initialTransform);
    // _bullet.motionState->setWorldTransform(initialTransform);
}

const glm::mat4& PhysicVehicle::getOpenGLMatrix(glm::mat4& mat4x4) const
{
    _bullet.carChassis->getWorldTransform().getOpenGLMatrix(glm::value_ptr(mat4x4));
    return mat4x4;
}

//

int PhysicVehicle::getNumWheels() const
{
    return _bullet.vehicle->getNumWheels();
}

const glm::mat4& PhysicVehicle::getWheelOpenGLMatrix(int index, glm::mat4& mat4x4) const
{
    _bullet.vehicle->getWheelTransformWS(index).getOpenGLMatrix(glm::value_ptr(mat4x4));
    return mat4x4;
}
