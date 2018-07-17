

#pragma once


#include "OpenGLES.hpp"


class	BasicGeometry
{
private:
	GLuint			m_obj_buffer = 0;
	unsigned int	m_numItems = 0;

public:
	BasicGeometry() = default;
	~BasicGeometry();

public:
	void	initialise();
	void	deinitialise();

public:
	void	updateGeometry(float* pData, unsigned int length);
	void	render(GLint primitive, float* matrix, float alpha = 1.0f) const;
};
