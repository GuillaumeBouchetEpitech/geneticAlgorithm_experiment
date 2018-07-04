

#pragma once


#include <vector>


class IPhysicWrapper
{
public:
	virtual ~IPhysicWrapper()
	{}

public: // to override

	//
	//
	// world

	virtual void	step(int world_index, float elapsed_time) = 0;

	//
	//
	// trimesh

	virtual void	createGround(const std::vector<float>& v, const std::vector<int>& i, int id) = 0;
	virtual void	createGhostWall(const std::vector<float>& v, const std::vector<int>& i) = 0;

	//
	//
	// vehicle

	virtual void	createVehicles(unsigned int total) = 0;
	virtual void	vehicle_applyEngineForce(int vehicle_index, float engineForce) = 0;
	virtual void	vehicle_setSteeringValue(int vehicle_index, float vehicleSteering) = 0;
	virtual void	vehicle_getOpenGLMatrix(int vehicle_index, float* mat4x4) = 0;
	virtual int		vehicle_getWheelsNumber(int vehicle_index) = 0;
	virtual void	vehicle_getWheelsMatrix(int vehicle_index, int wheel_index, float* mat4x4) = 0;
	virtual void	vehicle_reset(int vehicle_index, float* vec3, float* quat4) = 0;

	//
	//
	// raycast

	virtual bool	raycast(int vehicle_index, const float* vec3_from, const float* vec3_to, float* vec3_hit, int* id_hit) = 0;
};
