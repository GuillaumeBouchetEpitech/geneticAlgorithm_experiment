

#pragma once


#include "Logic/Physic/World.hpp"
#include "Logic/Physic/Vehicle.hpp"

#include "Simulation/AI/NeuralNetwork/NeuralNetwork.hpp"


#include <array>


class Car
{
private:
	static Physic::World*	m_pPhysicWorld;
public:
	static void	initialise(Physic::World* pPhysicWorld);

public: // external structures
	struct t_sensor
	{
		float	near[3];
		float	far[3];
		float	value;
	};
	typedef std::array<t_sensor, 15>	t_sensors;

private: // attributs

	Physic::Vehicle*	m_pVehicle = nullptr;

	float	m_fitness;
	bool	m_alive;
	int		m_currentHealth;
	unsigned int	m_totalUpdateNumber;

	// TODO : use enumeration
	t_sensors	m_eyeSensors;
	t_sensor	m_groundSensor;

	int			m_latestCheckpointId;

public: // ctor/dtor
	Car();

public: // methods
	void	update(const NeuralNetwork& nn);
	void	reset() const;

private: // methods
	void	updateSensors();
	void	collideSensors();
	void	collideCheckpoints();

public: // setter/getter

	inline const t_sensors&	getSensors() const { return m_eyeSensors; }
	inline const t_sensor&	getGroundSensor() const { return m_groundSensor; }
	inline float	getFitness() const { return m_fitness; }
	inline bool		isAlive() const { return m_alive; }

};
