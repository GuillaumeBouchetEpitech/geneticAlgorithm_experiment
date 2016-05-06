
#ifndef D_SIMULATION_HPP
#define D_SIMULATION_HPP


#include "TraceLogger.hpp"

#include "AI.hpp"


#include <iostream>
#include <vector>
#include <array>
#include <cmath>





//
// UTILS

struct t_vec2f
{
	float x, y;

	t_vec2f()
		:	x(0.0f), y(0.0f)
	{}

	t_vec2f(float in_x, float in_y)
		:	x(in_x), y(in_y)
	{}
};

struct t_line
{
	t_vec2f p1, p2;

	t_line()
	{}

	t_line(const t_vec2f& in_p1, const t_vec2f& in_p2)
		:	p1(in_p1), p2(in_p2)
	{}

	t_line(float x1, float y1, float x2, float y2)
		:	p1(x1, y1), p2(x2, y2)
	{}
};

typedef std::vector<t_line>	t_lines;

///

bool	CollisionDroiteSeg(const t_vec2f& A, const t_vec2f& B, const t_vec2f& O, const t_vec2f& P);
float	CollisionSegSeg_partial(const t_vec2f& A, const t_vec2f& B, const t_vec2f& O, const t_vec2f& P);
bool	CollisionSegSeg(const t_vec2f& A, const t_vec2f& B, const t_vec2f& O, const t_vec2f& P);

// cercle

bool	CollisionPointCercle(const t_vec2f& P, const t_vec2f& C, float radius);
bool	CollisionDroiteCercle(const t_vec2f& A, const t_vec2f&  B, const t_vec2f& C, float radius);
bool	CollisionSegmentCercle(const t_vec2f& A, const t_vec2f&  B, const t_vec2f& C, float radius);

t_vec2f	rotateVec2(const t_vec2f& point, const t_vec2f& center, float angle);

t_vec2f	lerp(const t_vec2f& p1, const t_vec2f& p2, float ratio);

// UTILS
//



//
// CIRCUIT

class Circuit
{
private: // attributs
	t_lines	m_checkpoints;
	t_lines	m_walls;

	t_vec2f	m_start_position;
	float	m_start_angle;

	bool m_valid;

public: // ctor/dtor
	Circuit();
	~Circuit();

public: // methods
	bool loadMap(const std::string& filename);

public: // setter/getter
	inline bool	isValid() const { return m_valid; }
	inline const t_lines&	getCheckpoints() const { return m_checkpoints; }
	inline const t_lines&	getWalls() const { return m_walls; }
	inline const t_vec2f&	getStartingPositon() const { return m_start_position; }
	inline float			getStartingAngle() const { return m_start_angle; }
};

// CIRCUIT
//



//
// CAR

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
	unsigned int	m_total_updates;

	// TODO : use enumeration
	t_sensors	m_sensors;

	t_lines		m_current_checkpoints;

	std::vector<t_line>	m_trail;

public: // ctor/dtor
	Car();

public: // methods
	void	update(float step, const Circuit& circuit, const NeuralNetwork& nn);
	void	revive();

private: // methods
	void	updateSensors();
	void	collideSensors(const Circuit& circuit);
	void	collideCheckpoints();
	void	collideWalls(const t_lines& walls);

public: // setter/getter

	inline void	setCheckpoints(const t_lines& checkpoints)	{ m_current_checkpoints = checkpoints; }
	inline void	setPosition(const t_vec2f& pos) { m_position = pos; }
	inline void	setAngle(float angle) { m_angle = angle; }
	inline const t_vec2f&	getPosition() const { return m_position; }
	inline float			getAngle() const { return m_angle; }

	// const std::array<t_line, 5>&	getSensorsLines() const { return m_sensors_lines; }
	// const std::array<float, 5>&		getSensorsValues() const { return m_sensors_values; }

	inline const t_sensors&	getSensors() const { return m_sensors; }


	inline float	getFitness() const { return m_fitness; }
	// inline float	getFitness() const { return (m_fitness + (1000.0f / m_total_updates)); }
	// inline float	getFitness() const { return (m_fitness + (m_total_updates / 1000.0f)); }
	inline bool		isAlive() const { return m_alive; }

	inline unsigned int	getTotalUpdates() const { return m_total_updates; }

	inline const std::vector<t_line>&	getTrail() const { return m_trail; };

};

// CAR
//



//
// SIMULATION

class Simulation
{
private: // attributs
	Circuit						m_Circuit;
	std::vector<Car>			m_Cars;
	GeneticAlgorithm			m_genAlgo;

	std::vector< std::vector<t_line> >	m_trails;

public: // ctor/dtor
	Simulation(const std::string& filename);

public: // methods
	void	update(float step);

public: // setter/getter
	inline const Circuit&			getCircuit() const { return m_Circuit; }
	inline const std::vector<Car>&	getCars() const { return m_Cars; }

	inline unsigned int getCurrentGeneration() const { return m_genAlgo.getCurrentGeneration(); }
	inline unsigned int	getBestFitness() const { return m_genAlgo.getBestFitness(); }

	inline const std::vector<GeneticAlgorithm::t_genome>&	getGenomes() const { return m_genAlgo.getGenomes(); }

	inline const std::vector< std::vector<t_line> >&	getTrails() const { return m_trails; };
	
};

// SIMULATION
//



#endif // D_SIMULATION_HPP
