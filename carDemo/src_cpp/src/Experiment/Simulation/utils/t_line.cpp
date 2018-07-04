

#include "t_line.hpp"


t_line::t_line()
{}

t_line::t_line(const t_vec2f& in_p1, const t_vec2f& in_p2)
	:	p1(in_p1), p2(in_p2)
{}

t_line::t_line(float x1, float y1, float x2, float y2)
	:	p1(x1, y1), p2(x2, y2)
{}

