

#pragma once


#include "Simulation/IPhysicWrapper.hpp"


class PhysicWrapper
	: public IPhysicWrapper
{
public:
	virtual ~PhysicWrapper();

public: // to override

	//
	//
	// world

	virtual void	step(int world_index, float elapsed_time) override;

	//
	//
	// trimesh

	virtual void	createGround(const std::vector<float>& b, const std::vector<int>& i, int id) override;
	virtual void	createGhostWall(const std::vector<float>& b, const std::vector<int>& i) override;

	//
	//
	// vehicle

	virtual void	createVehicles(unsigned int total) override;
	virtual void	vehicle_applyEngineForce(int vehicle_index, float engineForce) override;
	virtual void	vehicle_setSteeringValue(int vehicle_index, float vehicleSteering) override;
	virtual void	vehicle_getOpenGLMatrix(int vehicle_index, float* mat4x4) override;
	virtual int		vehicle_getWheelsNumber(int vehicle_index) override;
	virtual void	vehicle_getWheelsMatrix(int vehicle_index, int wheel_index, float* mat4x4) override;
	virtual void	vehicle_reset(int vehicle_index, float* vec3, float* quat4) override;

	//
	//
	// raycast

	virtual bool	raycast(int vehicle_index, const float* vec3_from, const float* vec3_to, float* vec3_hit, int* id_hit) override;

};
