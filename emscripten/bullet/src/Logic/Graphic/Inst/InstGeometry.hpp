

#pragma once


#include "OpenGLES.hpp"


class	InstShader;

class	InstGeometry
{
private:
    InstShader*    m_pInstShader = nullptr;
	GLuint		   m_obj_buffer = 0;
    GLuint		   m_obj_buffer2 = 0;
	unsigned int   m_numItems = 0;
    unsigned int   m_numInstance = 0;

public:
    InstGeometry() = default;
    ~InstGeometry();

public:
    void	initialise(InstShader*);
    void	update(float* pData, unsigned int length);
    void	update2(float* pData, unsigned int length);

public:
    void	render(GLint primitive, float* matrix);

};
