
#pragma once

#include "demo/logic/physic/PhysicWorld.hpp"
#include "demo/logic/physic/PhysicVehicle.hpp"

#include "demo/simulation/machineLearning/NeuralNetwork.hpp"

#include "thirdparty/GLMath.hpp"

#include <array>

class PhysicWorld;
class PhysicVehicle;

class Car
{
public: // external structures
	struct t_sensor
	{
		glm::vec3	near, far;
		float		value = 0.0f;
	};
	typedef std::array<t_sensor, 15>	t_sensors;

	struct t_neuralNetworkOutput
	{
		float steer	= 0.0f;
		float speed	= 0.0f;
	};

private: // attributs

	PhysicWorld&	_physicWorld;
	PhysicVehicle*	_vehicle = nullptr;

	float	_fitness;
	bool	_isAlive;
	int		_health;
	unsigned int	_totalUpdateNumber;

	// TODO : use enumeration
	t_sensors	_eyeSensors;
	t_sensor	_groundSensor;

	int			_groundIndex;

	t_neuralNetworkOutput	_output;

public: // ctor/dtor
	Car(PhysicWorld& physicWorld,
		const glm::vec3& position,
		const glm::vec4& quaternion);

public: // methods
	void	update(const NeuralNetwork& nn);
	void	reset(const glm::vec3& position, const glm::vec4& quaternion);

private: // methods
	void	updateSensors();
	void	collideEyeSensors();
	void	collideGroundSensor();

public: // setter/getter
	const t_sensors&				getEyeSensors() const;
	const t_sensor&					getGroundSensor() const;
	float							getFitness() const;
	bool							isAlive() const;
	int								getGroundIndex() const;
	const t_neuralNetworkOutput&	getNeuralNetworkOutput() const;
	const PhysicVehicle&			getVehicle() const;
	float							getLife() const;
};
