

#include "GeometryColor.hpp"

#include "ShaderColor.hpp"
#include "../Shader.hpp"

#include "Utility/TraceLogger.hpp"

#include <cstdio>

//


ShaderColor*	GeometryColor::m_pShaderColor = nullptr;

void	GeometryColor::initialise(ShaderColor* pShaderColor)
{
	m_pShaderColor = pShaderColor;
}

//

GeometryColor::~GeometryColor()
{
	if (m_obj_buffer) // initialised?
		deinitialise();
}

//

void	GeometryColor::initialise()
{
	if (m_obj_buffer) // already initialised?
		return;

	glGenBuffers(1, &m_obj_buffer);
}

void	GeometryColor::deinitialise()
{
	if (!m_obj_buffer)
		return;

    glDeleteBuffers(1, &m_obj_buffer);
    m_obj_buffer = 0;
}

//

void	GeometryColor::update(float* pData, unsigned int length)
{
	if (!m_obj_buffer)
		return;

	glBindBuffer(GL_ARRAY_BUFFER, m_obj_buffer);

	glBufferData(GL_ARRAY_BUFFER, length, pData, GL_STATIC_DRAW);
	// glBufferData(GL_ARRAY_BUFFER, length, pData, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_numItems = length / 6 / sizeof(float);
}

void	GeometryColor::render(GLint primitive, float* matrix, float alpha /*= 1.0f*/)
{
	if (!m_obj_buffer ||
		!m_pShaderColor)
		return;

	GLint attr_Position = m_pShaderColor->getShader()->getAttribute("a_Position");
	GLint attr_Color = m_pShaderColor->getShader()->getAttribute("a_Color");
	GLint unif_ComposedMatrix = m_pShaderColor->getShader()->getUniforms("u_ComposedMatrix");
	GLint unif_Alpha = m_pShaderColor->getShader()->getUniforms("u_Alpha");

	glEnableVertexAttribArray(attr_Position);
	glEnableVertexAttribArray(attr_Color);

		// glUniformMatrix4fv(unif_ComposedMatrix, 1, GL_FALSE, glm::value_ptr(composedMatrix));

		glUniformMatrix4fv( unif_ComposedMatrix, 1, GL_FALSE, matrix );
		glUniform1f( unif_Alpha, alpha );

		std::size_t		bpp = sizeof(float);
		std::size_t 	stride = 6 * bpp;
		void*			index_pos = (void*)(0 * bpp);
		void* 			index_color = (void*)(3 * bpp);

		glBindBuffer(GL_ARRAY_BUFFER, m_obj_buffer);

			glVertexAttribPointer(attr_Position, 3, GL_FLOAT, GL_FALSE, stride, index_pos);
			glVertexAttribPointer(attr_Color, 3, GL_FLOAT, GL_FALSE, stride, index_color);

			glDrawArrays(primitive, 0, m_numItems);

		glBindBuffer(GL_ARRAY_BUFFER, 0);



	glDisableVertexAttribArray(attr_Position);
	glDisableVertexAttribArray(attr_Color);
}

