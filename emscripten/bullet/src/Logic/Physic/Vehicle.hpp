

#pragma once


class	btCollisionShape;
class	btCompoundShape;
class	btDefaultMotionState;
class	btRigidBody;

class	btDefaultVehicleRaycaster;
class	btRaycastVehicle;

class	btDiscreteDynamicsWorld;


namespace	Physic
{


class World;

class Vehicle
{
private:
	friend World;

private:
	btCollisionShape*	m_pChassisShape = nullptr;
	btCompoundShape*	m_pCompound = nullptr;
	btDefaultMotionState*	m_pMotionState = nullptr;
	btRigidBody*		m_pCarChassis = nullptr;

	btDefaultVehicleRaycaster*	m_pVehicleRayCaster = nullptr;
	btRaycastVehicle*	m_pVehicle = nullptr;

private:
	Vehicle(btDiscreteDynamicsWorld* pDynamicsWorld);
	~Vehicle();

public:
	void	applyEngineForce(float engineForce);
	void	setSteeringValue(float vehicleSteering);
	void	fullBrake();

public:
	void	setPosition(const float* pVec3_pos);
	void	setRotation(const float* pQuat_rot);

public:
	void	getOpenGLMatrix(float* mat4);

	int		getNumWheels() const;
	void	getWheelMatrix(int index, float* mat4) const;

};


};
