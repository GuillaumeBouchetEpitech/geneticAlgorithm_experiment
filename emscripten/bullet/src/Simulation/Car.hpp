

#pragma once


#include "IPhysicWrapper.hpp"

#include "AI/NeuralNetwork.hpp"


#include <array>


class Car
{
private:
	static IPhysicWrapper*	m_pPhysicWrapper;
public:
	static void	initialise(IPhysicWrapper* pPhysicWrapper);

public: // external structures
	struct t_sensor
	{
		float	near[3];
		float	far[3];
		float	value;
	};
	typedef std::array<t_sensor, 15>	t_sensors;

private: // attributs

	unsigned int	m_index;

	float	m_fitness;
	bool	m_alive;
	int		m_min_updates;
	unsigned int	m_total_updates;

	// TODO : use enumeration
	t_sensors	m_sensors;
	t_sensor	m_sensor_ground;

	int			m_current_checkpoint;

	std::vector<float>	m_trail;

public: // ctor/dtor
	Car(unsigned int index);

public: // methods
	void	update(const NeuralNetwork& nn);
	void	reset() const;

private: // methods
	void	updateSensors();
	void	collideSensors();
	void	collideCheckpoints();

public: // setter/getter

	inline const t_sensors&	getSensors() const { return m_sensors; }
	inline const t_sensor&	getGroundSensor() const { return m_sensor_ground; }


	inline float	getFitness() const { return m_fitness; }
	inline bool		isAlive() const { return m_alive; }

	inline unsigned int	getIndex() const { return m_index; }

	inline unsigned int	getTotalUpdates() const { return m_total_updates; }

	inline const std::vector<float>&	getTrail() const { return m_trail; };

};
