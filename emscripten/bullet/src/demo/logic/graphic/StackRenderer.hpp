
#pragma once

#include "thirdparty/GLMath.hpp"

#include <vector>

struct StackRenderer
{
public:
	struct t_vertex
	{
		typedef glm::vec3	t_position;
		typedef glm::vec3	t_color;

		t_position	position;
		t_color		color;

		// t_vertex(const t_position& p, const t_color& c)
		// 	: position(p)
		// 	, color(c)
		// {}
	};
	typedef std::vector<t_vertex> t_vertices;

public:
	t_vertices			_vertices;

public:
	StackRenderer();
	~StackRenderer();

public:
	void	push(const t_vertex& vertex);
	void	pushLine(const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& color);
	void	pushCross(const glm::vec3& pos, const glm::vec3& color, float halfExtent);

	void	flush();
};
