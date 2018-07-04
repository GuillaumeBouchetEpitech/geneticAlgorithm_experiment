

#include "utils.hpp"

#include <cmath> // cos/sin


t_vec2f	rotateVec2(const t_vec2f& point, const t_vec2f& center, float angle)
{
	t_vec2f	newPos;

	float cos_a = cosf(angle);
	float sin_a = sinf(angle);

	newPos.x = center.x + (point.x-center.x) * cos_a + (point.y-center.y)*sin_a;
	newPos.y = center.y + (point.x-center.x) * sin_a - (point.y-center.y)*cos_a;

	return newPos;
}

t_vec2f	lerp(const t_vec2f& p1, const t_vec2f& p2, float ratio)
{
	t_vec2f ret_val;
	ret_val.x = p1.x + (p2.x - p1.x) * ratio;
	ret_val.y = p1.y + (p2.y - p1.y) * ratio;
	return ret_val;
}

