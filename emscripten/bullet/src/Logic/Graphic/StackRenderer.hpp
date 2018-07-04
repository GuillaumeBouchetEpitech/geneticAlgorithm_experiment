

#pragma once


#include "OpenGLES.hpp"

#include <vector>


class Shader;

struct StackRenderer
{
public:

	struct t_pos
	{
		float x, y, z;

		t_pos(float in_x, float in_y, float in_z)
			: x(in_x)
			, y(in_y)
			, z(in_z)
		{}
	};

	struct t_color
	{
		float r, g, b, a;

		t_color(float in_r, float in_g, float in_b, float in_a)
			: r(in_r)
			, g(in_g)
			, b(in_b)
			, a(in_a)
		{}
	};

	struct t_vertex
	{
		t_pos	pos;
		t_color	color;

		t_vertex(const t_pos& p, const t_color& c)
			: pos(p)
			, color(c)
		{}
	};

public:
	GLuint m_vboId = 0;
	std::vector<t_vertex>	m_vertices;

	Shader*	m_pShader = nullptr;

	float*	m_pMatrix = nullptr;

public:
	StackRenderer() = default;

public:
	void create();
	void destroy();

public:
	inline void	setMatrix(float* pMatrix) { m_pMatrix = pMatrix; }

	void push_vertex(const t_pos& v, const t_color& c);
    void flush();
};
