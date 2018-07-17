

#include "Vehicle.hpp"

#include <Bullet/btBulletDynamicsCommon.h>


namespace	Physic
{


Vehicle::Vehicle(btDiscreteDynamicsWorld* pDynamicsWorld)
{
#define CUBE_HALF_EXTENTS 1

	//
	//
	// STATS

	btVector3 wheelDirectionCS0(0,0,-1);
	btVector3 wheelAxleCS(1,0,0);
	float	wheelRadius = 0.5f;
	float	wheelWidth = 0.2f;

	/// The maximum length of the suspension (metres)
	float	suspensionRestLength = 0.8f;

	/// The maximum distance the suspension can be compressed (centimetres)
	float	maxSuspensionTravelCm = 70.0f;

	/// The coefficient of friction between the tyre and the ground.
	/// Should be about 0.8 for realistic cars, but can increased for better handling.
	/// Set large (10000.0) for kart racers
	float	wheelFriction = 10000.0f;

	/// The stiffness constant for the suspension. 10.0 - Offroad buggy, 50.0 - Sports car, 200.0 - F1 Car
	// float	suspensionStiffness = 200.f;
	float	suspensionStiffness = 30.f;

	/// The damping coefficient for when the suspension is compressed. Set to k * 2.0 * btSqrt(m_suspensionStiffness) so k is proportional to critical damping.
	/// k = 0.0 undamped & bouncy, k = 1.0 critical damping
	/// 0.1 to 0.3 are good values
	// float	wheelsDampingCompression = 1.0f;
	float	wheelsDampingCompression = 0.2f;

	/// The damping coefficient for when the suspension is expanding. See the comments for m_wheelsDampingCompression for how to set k.
	/// m_wheelsDampingRelaxation should be slightly larger than m_wheelsDampingCompression, eg 0.2 to 0.5
	// float	wheelsDampingRelaxation = 1.0f;
	float	wheelsDampingRelaxation = 0.4f;

	/// Reduces the rolling torque applied from the wheels that cause the vehicle to roll over.
	/// This is a bit of a hack, but it's quite effective. 0.0 = no roll, 1.0 = physical behaviour.
	/// If m_frictionSlip is too high, you'll need to reduce this to stop the vehicle rolling over.
	/// You should also try lowering the vehicle's centre of mass
	float	rollInfluence = 0.01f;

	//
	//
	// CHASSIS

	m_pChassisShape = new btBoxShape(btVector3(1.f,2.f, 0.5f));
	m_pCompound = new btCompoundShape();

	btTransform localTrans;
	localTrans.setIdentity();
	localTrans.setOrigin(btVector3(0,0,1));
	m_pCompound->addChildShape(localTrans, m_pChassisShape);


	btScalar	mass = 80;
	btVector3 localInertia(0,0,0);
	m_pCompound->calculateLocalInertia(mass,localInertia);

	btTransform tr;
	tr.setIdentity();
	tr.setOrigin(btVector3(0,0,10));

	m_pMotionState = new btDefaultMotionState(tr);
	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, m_pMotionState, m_pCompound, localInertia);
	m_pCarChassis = new btRigidBody(cInfo);


	//
	//
	// VEHICLE

	btRaycastVehicle::btVehicleTuning	tuning;
	m_pVehicleRayCaster = new btDefaultVehicleRaycaster(pDynamicsWorld);

	m_pVehicle = new btRaycastVehicle(tuning, m_pCarChassis, m_pVehicleRayCaster);

	// never deactivate the vehicle
	m_pCarChassis->setActivationState(DISABLE_DEACTIVATION);


	//
	//
	// WHEELS

	float connectionHeight = 1.2f;

	// choose coordinate system
	int rightIndex = 0;
	int upIndex = 2;
	int forwardIndex = 1;
	m_pVehicle->setCoordinateSystem(rightIndex,upIndex,forwardIndex);

	bool isFrontWheel = true;

	btVector3 connectionPointCS0(CUBE_HALF_EXTENTS - (0.3f * wheelWidth), 2 * CUBE_HALF_EXTENTS - wheelRadius, connectionHeight);
	m_pVehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, isFrontWheel);

	connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS + (0.3 * wheelWidth), 2 * CUBE_HALF_EXTENTS - wheelRadius, connectionHeight);
	m_pVehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, isFrontWheel);

	isFrontWheel = false;

	connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS + (0.3 * wheelWidth), -2 * CUBE_HALF_EXTENTS + wheelRadius, connectionHeight);
	m_pVehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, isFrontWheel);

	connectionPointCS0 = btVector3(CUBE_HALF_EXTENTS - (0.3 * wheelWidth), -2 * CUBE_HALF_EXTENTS + wheelRadius, connectionHeight);
	m_pVehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, isFrontWheel);
	
	for (int ii = 0; ii < m_pVehicle->getNumWheels(); ++ii)
	{
		btWheelInfo&	wheel = m_pVehicle->getWheelInfo(ii);

		wheel.m_suspensionStiffness = suspensionStiffness;
		wheel.m_wheelsDampingRelaxation = wheelsDampingRelaxation;
		wheel.m_wheelsDampingCompression = wheelsDampingCompression;
		wheel.m_frictionSlip = wheelFriction;
		wheel.m_rollInfluence = rollInfluence;
	}

#undef	CUBE_HALF_EXTENTS
}

Vehicle::~Vehicle()
{
	delete m_pVehicle;
	delete m_pVehicleRayCaster;

	delete m_pCarChassis;
	delete m_pMotionState;
	delete m_pCompound;
	delete m_pChassisShape;
}


void	Vehicle::applyEngineForce(float engineForce)
{
	// rear wheels
	const int wheelIndex1 = 2;
	const int wheelIndex2 = 3;

	m_pVehicle->applyEngineForce(engineForce, wheelIndex1);
	m_pVehicle->applyEngineForce(engineForce, wheelIndex2);
}

void	Vehicle::setSteeringValue(float vehicleSteering)
{
	// front wheels
	const int wheelIndex1 = 0;
	const int wheelIndex2 = 1;

	m_pVehicle->setSteeringValue(vehicleSteering, wheelIndex1);
	m_pVehicle->setSteeringValue(vehicleSteering, wheelIndex2);
}

void	Vehicle::fullBrake()
{
	m_pCarChassis->clearForces();
	btVector3 zeroVector(0, 0, 0);
	m_pCarChassis->setLinearVelocity(zeroVector);
	m_pCarChassis->setAngularVelocity(zeroVector);

	for (int ii = 0; ii < 4; ++ii)
		m_pVehicle->setBrake(100, ii);

	m_pVehicle->updateVehicle(0);

	for (int ii = 0; ii < 4; ++ii)
		m_pVehicle->setBrake(0, ii);
}

void	Vehicle::setPosition(const float* pPosition)
{
	btVector3	pos(pPosition[0], pPosition[1], pPosition[2]);

	btTransform initialTransform = m_pCarChassis->getWorldTransform();

	initialTransform.setOrigin(pos);

	m_pCarChassis->setWorldTransform(initialTransform);
	m_pMotionState->setWorldTransform(initialTransform);
}

void	Vehicle::setRotation(const float* pRotation)
{
	btQuaternion	rot(pRotation[0], pRotation[1], pRotation[2], pRotation[3]);

	btTransform initialTransform = m_pCarChassis->getWorldTransform();

	initialTransform.setRotation(rot);

	m_pCarChassis->setWorldTransform(initialTransform);
	m_pMotionState->setWorldTransform(initialTransform);
}

void	Vehicle::getOpenGLMatrix(btScalar* pMat4x4)
{
	m_pCarChassis->getWorldTransform().getOpenGLMatrix(pMat4x4);
}

//

int		Vehicle::getNumWheels() const
{
	return m_pVehicle->getNumWheels();
}

void	Vehicle::getWheelOpenGLMatrix(int index, float* pMat4x4) const
{
	m_pVehicle->getWheelTransformWS(index).getOpenGLMatrix(pMat4x4);
}




};

