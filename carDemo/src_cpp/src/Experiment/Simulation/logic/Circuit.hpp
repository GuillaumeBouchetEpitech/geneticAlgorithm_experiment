

#ifndef	D_CIRCUIT_HPP
#define	D_CIRCUIT_HPP


#include "../utils/t_line.hpp"

#include <string>


class Circuit
{
private: // attributs
	t_lines	m_checkpoints;
	t_lines	m_walls;

	t_vec2f	m_start_position;
	float	m_start_angle;

	t_vec2f	m_stop_position;
	float	m_stop_angle;

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

	inline const t_vec2f&	getStoppingPositon() const { return m_stop_position; }
	inline float			getStoppingAngle() const { return m_stop_angle; }
};


#endif	// D_CIRCUIT_HPP

