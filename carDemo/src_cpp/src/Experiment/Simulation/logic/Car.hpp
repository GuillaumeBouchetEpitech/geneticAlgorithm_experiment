

#ifndef	D_CAR_HPP
#define	D_CAR_HPP


#include "../utils/t_line.hpp"

#include "../ai/NeuralNetwork.hpp"


#include <array>


class Circuit;

class Car
{
public: // external structures
	struct t_sensor
	{
		t_line	m_line;
		float	m_value;
	};
	typedef std::array<t_sensor, 5>	t_sensors;

private: // attributs
	t_vec2f	m_position;
	float	m_angle;
	float	m_fitness;
	bool	m_alive;
	unsigned int	m_min_updates;
	unsigned int	m_total_updates;

	// TODO : use enumeration
	t_sensors	m_sensors;

	unsigned int	m_current_checkpoint;

	std::vector<t_line>	m_trail;

public: // ctor/dtor
	Car();

public: // methods
	void	update(float step, const Circuit& circuit, const NeuralNetwork& nn);
	void	reset(const Circuit& circuit);

private: // methods
	void	updateSensors();
	void	collideSensors(const t_lines& walls);
	void	collideCheckpoints(const t_lines& checkpoints);
	void	collideWalls(const t_lines& walls);

public: // setter/getter

	inline const t_vec2f&	getPosition() const { return m_position; }
	inline float			getAngle() const { return m_angle; }

	inline const t_sensors&	getSensors() const { return m_sensors; }


	inline float	getFitness() const { return m_fitness; }
	// inline float	getFitness() const { return (m_fitness + (1000.0f / m_total_updates)); }
	// inline float	getFitness() const { return (m_fitness + (m_total_updates / 1000.0f)); }
	inline bool		isAlive() const { return m_alive; }

	// inline bool		getLife() const { return (m_min_updates / 50.0f); } // TODO: this is hardcoded :(
	float			getLife() const;

	inline unsigned int	getTotalUpdates() const { return m_total_updates; }

	inline const std::vector<t_line>&	getTrail() const { return m_trail; };

};


#endif	// D_CAR_HPP

