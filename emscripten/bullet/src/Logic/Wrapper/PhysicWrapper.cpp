

#include "PhysicWrapper.hpp"


#include "../Data.hpp"

#include "../Physic/World.hpp"
#include "../Physic/Trimesh.hpp"
#include "../Physic/Vehicle.hpp"

#include "Utility/TraceLogger.hpp"


PhysicWrapper::~PhysicWrapper()
{}


//
//
// world

void	PhysicWrapper::step(int world_index, float elapsed_time)
{
	// D_MYLOG("world_index=" << world_index);
	switch (world_index)
	{
		case 0: Data::get()->m_PhysicWorld.step(elapsed_time); break;
		case 1: Data::get()->m_PhysicWorld2.step(elapsed_time); break;
		case 2: Data::get()->m_PhysicWorld3.step(elapsed_time); break;
	}
}

//
//
// trimesh

void	PhysicWrapper::createGround(const std::vector<float>& b, const std::vector<int>& i, int id)
{
	Data::get()->m_PhysicWorld.createGround(b, i, id);
	Data::get()->m_PhysicWorld2.createGround(b, i, id);
	Data::get()->m_PhysicWorld3.createGround(b, i, id);
}

void	PhysicWrapper::createGhostWall(const std::vector<float>& b, const std::vector<int>& i)
{
	Data::get()->m_PhysicWorld.createWall(b, i);
	Data::get()->m_PhysicWorld2.createWall(b, i);
	Data::get()->m_PhysicWorld3.createWall(b, i);
}

//
//
// vehicle

void	PhysicWrapper::createVehicles(unsigned int total)
{
	int step = total / 3;

	// for (unsigned int i = 0; i < total; ++i)
	// 	Data::get()->m_PhysicWorld.createVehicle();

	for (unsigned int ii = step*0; ii < step*1; ++ii)
		Data::get()->m_PhysicWorld.createVehicle();
	for (unsigned int ii = step*1; ii < step*2; ++ii)
		Data::get()->m_PhysicWorld2.createVehicle();
	for (unsigned int ii = step*2; ii < step*3; ++ii)
		Data::get()->m_PhysicWorld3.createVehicle();
}


void	PhysicWrapper::vehicle_applyEngineForce(int vehicle_index, float engineForce)
{
	// Data::get()->m_PhysicWorld.getVehicle(vehicle_index)->applyEngineForce(engineForce);

	unsigned int size  = Data::get()->m_PhysicWorld.getVehicleSize();
	unsigned int size2  = Data::get()->m_PhysicWorld2.getVehicleSize();
	unsigned int size3  = Data::get()->m_PhysicWorld3.getVehicleSize();
	if (vehicle_index < size)
		Data::get()->m_PhysicWorld.getVehicle(vehicle_index)->applyEngineForce(engineForce);
	else if (vehicle_index < size + size2)
		Data::get()->m_PhysicWorld2.getVehicle(vehicle_index - size)->applyEngineForce(engineForce);
	else if (vehicle_index < size + size2 + size3)
		Data::get()->m_PhysicWorld3.getVehicle(vehicle_index - (size + size2))->applyEngineForce(engineForce);
}

void	PhysicWrapper::vehicle_setSteeringValue(int vehicle_index, float vehicleSteering)
{
	// Data::get()->m_PhysicWorld.getVehicle(vehicle_index)->setSteeringValue(vehicleSteering);

	unsigned int size  = Data::get()->m_PhysicWorld.getVehicleSize();
	unsigned int size2  = Data::get()->m_PhysicWorld2.getVehicleSize();
	unsigned int size3  = Data::get()->m_PhysicWorld3.getVehicleSize();
	if (vehicle_index < size)
		Data::get()->m_PhysicWorld.getVehicle(vehicle_index)->setSteeringValue(vehicleSteering);
	else if (vehicle_index < size + size2)
		Data::get()->m_PhysicWorld2.getVehicle(vehicle_index - size)->setSteeringValue(vehicleSteering);
	else if (vehicle_index < size + size2 + size3)
		Data::get()->m_PhysicWorld3.getVehicle(vehicle_index - (size + size2))->setSteeringValue(vehicleSteering);
}

int		PhysicWrapper::vehicle_getWheelsNumber(int vehicle_index)
{
	unsigned int size  = Data::get()->m_PhysicWorld.getVehicleSize();
	unsigned int size2  = Data::get()->m_PhysicWorld2.getVehicleSize();
	unsigned int size3  = Data::get()->m_PhysicWorld3.getVehicleSize();
	if (vehicle_index < size)
		return Data::get()->m_PhysicWorld.getVehicle(vehicle_index)->getNumWheels();
	else if (vehicle_index < size + size2)
		return Data::get()->m_PhysicWorld2.getVehicle(vehicle_index - size)->getNumWheels();
	else if (vehicle_index < size + size2 + size3)
		return Data::get()->m_PhysicWorld3.getVehicle(vehicle_index - (size + size2))->getNumWheels();
}

void	PhysicWrapper::vehicle_getWheelsMatrix(int vehicle_index, int wheel_index, float* mat4x4)
{
	unsigned int size  = Data::get()->m_PhysicWorld.getVehicleSize();
	unsigned int size2  = Data::get()->m_PhysicWorld2.getVehicleSize();
	unsigned int size3  = Data::get()->m_PhysicWorld3.getVehicleSize();
	if (vehicle_index < size)
		Data::get()->m_PhysicWorld.getVehicle(vehicle_index)->getWheelMatrix(wheel_index, mat4x4);
	else if (vehicle_index < size + size2)
		Data::get()->m_PhysicWorld2.getVehicle(vehicle_index - size)->getWheelMatrix(wheel_index, mat4x4);
	else if (vehicle_index < size + size2 + size3)
		Data::get()->m_PhysicWorld3.getVehicle(vehicle_index - (size + size2))->getWheelMatrix(wheel_index, mat4x4);
}

void	PhysicWrapper::vehicle_getOpenGLMatrix(int vehicle_index, float* mat4x4)
{
	// Data::get()->m_PhysicWorld.getVehicle(vehicle_index)->getOpenGLMatrix(mat4x4);

	unsigned int size  = Data::get()->m_PhysicWorld.getVehicleSize();
	unsigned int size2  = Data::get()->m_PhysicWorld2.getVehicleSize();
	unsigned int size3  = Data::get()->m_PhysicWorld3.getVehicleSize();
	if (vehicle_index < size)
		Data::get()->m_PhysicWorld.getVehicle(vehicle_index)->getOpenGLMatrix(mat4x4);
	else if (vehicle_index < size + size2)
		Data::get()->m_PhysicWorld2.getVehicle(vehicle_index - size)->getOpenGLMatrix(mat4x4);
	else if (vehicle_index < size + size2 + size3)
		Data::get()->m_PhysicWorld3.getVehicle(vehicle_index - (size + size2))->getOpenGLMatrix(mat4x4);
}

void	PhysicWrapper::vehicle_reset(int vehicle_index, float* vec3, float* quat4)
{
	btVector3		pos(vec3[0], vec3[1], vec3[2]);
	btQuaternion	rot(quat4[0], quat4[1], quat4[2], quat4[3]);

	// Data::get()->m_PhysicWorld.getVehicle(vehicle_index)->fullBrake();
	// Data::get()->m_PhysicWorld.getVehicle(vehicle_index)->setPosition(pos);
	// Data::get()->m_PhysicWorld.getVehicle(vehicle_index)->setRotation(rot);

	unsigned int size  = Data::get()->m_PhysicWorld.getVehicleSize();
	unsigned int size2  = Data::get()->m_PhysicWorld2.getVehicleSize();
	unsigned int size3  = Data::get()->m_PhysicWorld3.getVehicleSize();
	if (vehicle_index < size)
	{
		Data::get()->m_PhysicWorld.getVehicle(vehicle_index)->fullBrake();
		Data::get()->m_PhysicWorld.getVehicle(vehicle_index)->setPosition(pos);
		Data::get()->m_PhysicWorld.getVehicle(vehicle_index)->setRotation(rot);
	}
	else if (vehicle_index < size + size2)
	{
		Data::get()->m_PhysicWorld2.getVehicle(vehicle_index - size)->fullBrake();
		Data::get()->m_PhysicWorld2.getVehicle(vehicle_index - size)->setPosition(pos);
		Data::get()->m_PhysicWorld2.getVehicle(vehicle_index - size)->setRotation(rot);
	}
	else if (vehicle_index < size + size2 + size3)
	{
		Data::get()->m_PhysicWorld3.getVehicle(vehicle_index - (size + size2))->fullBrake();
		Data::get()->m_PhysicWorld3.getVehicle(vehicle_index - (size + size2))->setPosition(pos);
		Data::get()->m_PhysicWorld3.getVehicle(vehicle_index - (size + size2))->setRotation(rot);
	}
}

//
//
// raycast

bool	PhysicWrapper::raycast(int vehicle_index, const float* pVec3_from, const float* pVec3_to, float* pVec3_hit, int* pID_hit)
{
	// init

	if (pVec3_hit)
		memset(pVec3_hit, 0, 3 * sizeof(float));
	if (pID_hit)
		*pID_hit = -1;

	// raycast

	// bool	success = Data::get()->m_PhysicWorld.raycast(pVec3_from, pVec3_to);
	Physic::Trimesh*	pTrimesh;
	// bool	success = Data::get()->m_PhysicWorld.raycast(pVec3_from, pVec3_to, &pTrimesh, pVec3_hit);

	bool	success = false;
	unsigned int size  = Data::get()->m_PhysicWorld.getVehicleSize();
	unsigned int size2  = Data::get()->m_PhysicWorld2.getVehicleSize();
	unsigned int size3  = Data::get()->m_PhysicWorld3.getVehicleSize();
	if (vehicle_index < size)
		success = Data::get()->m_PhysicWorld.raycast(pVec3_from, pVec3_to, &pTrimesh, pVec3_hit);
	else if (vehicle_index < size + size2)
		success = Data::get()->m_PhysicWorld2.raycast(pVec3_from, pVec3_to, &pTrimesh, pVec3_hit);
	else if (vehicle_index < size + size2 + size3)
		success = Data::get()->m_PhysicWorld3.raycast(pVec3_from, pVec3_to, &pTrimesh, pVec3_hit);

	// result

	if (success)
	{
		// if (pID_hit)
		// 	*pID_hit = Data::get()->m_PhysicWorld.getRaycastedTrimesh()->getID();
		// if (pVec3_hit)
		// 	Data::get()->m_PhysicWorld.getRaycastedPosition(pVec3_hit);
		if (pID_hit)
			*pID_hit = pTrimesh->getID();
	}

	return success;
}
