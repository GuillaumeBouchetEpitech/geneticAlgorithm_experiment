
#ifndef	D_T_LINE_HPP
#define	D_T_LINE_HPP


#include "t_vec2f.hpp"

#include <vector>


struct t_line
{
	t_vec2f p1, p2;

	t_line();
	t_line(const t_vec2f& in_p1, const t_vec2f& in_p2);
	t_line(float x1, float y1, float x2, float y2);
};

typedef std::vector<t_line>	t_lines;


#endif // D_T_LINE_HPP
