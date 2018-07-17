

#pragma once


#include "OpenGLES.hpp"


class	InstancedGeometry
{
private:
	GLuint		   m_obj_bufferGeometry = 0;
    GLuint		   m_obj_bufferMatrix = 0;
	unsigned int   m_numItems = 0;
    unsigned int   m_numInstance = 0;

public:
    InstancedGeometry() = default;
    ~InstancedGeometry();

public:
    void	initialise();
    void	updateGeometry(float* pData, unsigned int length);
    void	updateMatrix(float* pData, unsigned int length);

public:
    void	render(GLint primitive, float* matrix) const;

};
