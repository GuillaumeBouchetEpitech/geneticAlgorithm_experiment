

#include "Circuit.hpp"

#include "../utils/utils.hpp"

#include <fstream>
#include <sstream>
#include <cmath>


Circuit::Circuit()
	:	m_valid(false)
{}

Circuit::~Circuit()
{}

bool Circuit::loadMap(const std::string& filename)
{
	// D_MYLOG("filename=" << filename);

	// open

	std::ifstream ifs;
	ifs.open(filename, std::ifstream::in);

	if (!ifs.is_open())
	{
		// D_MYLOG("fail to open");
		return false;
	}

	// loop

	std::string line;
	std::string label;

	while (ifs.good())
	{
		std::getline(ifs, line);

		if (line.empty())
		{
			// D_MYLOG("[empty line]");
			continue;
		}

		// D_MYLOG("line=" << line);

		std::stringstream sstr(line);
		sstr >> label;

		//
		// checkpoints

		if (label == "checkpoint")
		{
			// D_MYLOG("processing checkpoint");

			std::vector<float> vals;
			float val;
			while (sstr >> val)
			{
				if (std::isnan(val))
				{
					// D_MYLOG("invalid value -> Not a Number");
					return false;
				}

				vals.push_back(val);
			}

			if (vals.size() != 4)
			{
				// D_MYLOG("invalid number of values");
				return false;
			}

			// D_MYLOG("processed checkpoint");
			// D_MYLOG("vals: " << vals[0] << "," << vals[1] << "  " << vals[2] << "," << vals[3]);

			t_vec2f p1(vals[0],vals[1]);
			t_vec2f p2(vals[2],vals[3]);
			m_checkpoints.push_back( t_line(p1, p2) );
		}

		// checkpoints
		//

		else
		{
			// D_MYLOG("unidentified line");
		}
	}

	// close

	ifs.close();

	// final check

	// TODO : more complex check required here
	// -> need to use m_valid
	if (m_checkpoints.size() < 3)
		return false;

	// process walls

	for (unsigned int i = 1; i < m_checkpoints.size(); ++i)
	{
		const t_line& prev = m_checkpoints[i - 1];
		const t_line& curr = m_checkpoints[i];

		m_walls.push_back( t_line(prev.p1, curr.p1) );
		m_walls.push_back( t_line(prev.p2, curr.p2) );
	}	
	// m_walls.push_back( t_line(m_checkpoints.front().p1, m_checkpoints.back().p1) );
	// m_walls.push_back( t_line(m_checkpoints.front().p2, m_checkpoints.back().p2) );

	// m_walls.push_back( t_line(m_checkpoints.front().p1, m_checkpoints.front().p2) );
	// m_walls.push_back( t_line(m_checkpoints.back().p1, m_checkpoints.back().p2) );

	// expand the walls (fix collisions bugs)

	for (unsigned int i = 0; i < m_walls.size(); ++i)
	{
		m_walls[i].p1 = lerp(m_walls[i].p1, m_walls[i].p2, -0.01f);
		m_walls[i].p2 = lerp(m_walls[i].p1, m_walls[i].p2, 1.01f);
	}

	// duplicate the checkpoints

	t_lines	tmp_checkpoints;
	tmp_checkpoints.push_back(m_checkpoints[0]);
	for (unsigned int i = 1; i < m_checkpoints.size(); ++i)
	{
		const t_line& l1 = m_checkpoints[i - 1];
		const t_line& l2 = m_checkpoints[i];

		t_line	tmp_line;
		for (float f = 0.25f; f < 1.0f; f += 0.25f)
		{
			tmp_line.p1 = lerp(l1.p1, l2.p1, f);
			tmp_line.p2 = lerp(l1.p2, l2.p2, f);
			tmp_checkpoints.push_back(tmp_line);
		}

		tmp_checkpoints.push_back(l2);
	}
	// {
	// 	const t_line& l1 = m_checkpoints.front();
	// 	const t_line& l2 = m_checkpoints.back();

	// 	t_line	tmp_line;
	// 	tmp_line.p1 = lerp(l1.p1, l2.p1, 0.5f);
	// 	tmp_line.p2 = lerp(l1.p2, l2.p2, 0.5f);
	// 	tmp_checkpoints.push_back(tmp_line);

	// 	tmp_checkpoints.push_back(l2);
	// }
	m_checkpoints = tmp_checkpoints;

	// print checkpoints

	// D_MYLOG("print checkpoints");
	// for (unsigned int i = 0; i < m_checkpoints.size(); ++i)
	// {
	// 	const t_line& l = m_checkpoints[i];
	// 	D_MYLOG("[" << i << "]: " << l.p1.x << "," << l.p1.y << "  " << l.p2.x << "," << l.p2.y);
	// }



	// print walls

	// D_MYLOG("print walls");
	// for (unsigned int i = 0; i < m_walls.size(); ++i)
	// {
	// 	const t_line& l = m_walls[i];
	// 	D_MYLOG("[" << i << "]: " << l.p1.x << "," << l.p1.y << "  " << l.p2.x << "," << l.p2.y);
	// }

	// process starting position

	{
		t_line& l1 = m_checkpoints.front();
		m_start_position.x = l1.p1.x + (l1.p2.x - l1.p1.x) * 0.5f;
		m_start_position.y = l1.p1.y + (l1.p2.y - l1.p1.y) * 0.5f;

		t_vec2f pos;
		t_line& l2 = m_checkpoints[1];
		pos.x = l2.p1.x + (l2.p2.x - l2.p1.x) * 0.5f;
		pos.y = l2.p1.y + (l2.p2.y - l2.p1.y) * 0.5f;

		// D_MYLOG("m_start_position.x=" << m_start_position.x);
		// D_MYLOG("m_start_position.y=" << m_start_position.y);
		// D_MYLOG("pos.x=" << pos.x);
		// D_MYLOG("pos.y=" << pos.y);

		m_start_angle = atan2f(pos.y - m_start_position.y, pos.x - m_start_position.x);

		// m_start_position.x += 10.0f * cosf(m_start_angle);
		// m_start_position.y += 10.0f * sinf(m_start_angle);

		// D_MYLOG("m_start_angle=" << m_start_angle);
	}

	{
		t_line& l1 = m_checkpoints.back();
		m_stop_position.x = l1.p1.x + (l1.p2.x - l1.p1.x) * 0.5f;
		m_stop_position.y = l1.p1.y + (l1.p2.y - l1.p1.y) * 0.5f;

		t_vec2f pos;
		t_line& l2 = m_checkpoints[m_checkpoints.size() - 2];
		pos.x = l2.p1.x + (l2.p2.x - l2.p1.x) * 0.5f;
		pos.y = l2.p1.y + (l2.p2.y - l2.p1.y) * 0.5f;

		// D_MYLOG("m_stop_position.x=" << m_stop_position.x);
		// D_MYLOG("m_stop_position.y=" << m_stop_position.y);
		// D_MYLOG("pos.x=" << pos.x);
		// D_MYLOG("pos.y=" << pos.y);

		m_stop_angle = atan2f(pos.y - m_stop_position.y, pos.x - m_stop_position.x);

		// m_stop_position.x += 10.0f * cosf(m_stop_angle);
		// m_stop_position.y += 10.0f * sinf(m_stop_angle);

		// D_MYLOG("m_stop_angle=" << m_stop_angle);
	}

	return true;
}

