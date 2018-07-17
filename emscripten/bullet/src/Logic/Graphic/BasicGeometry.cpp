

#include "BasicGeometry.hpp"

#include "Logic/Data.hpp"
#include "Logic/Graphic/Shader.hpp"

#include "Utility/TraceLogger.hpp"

#include <cstdio>

//

BasicGeometry::~BasicGeometry()
{
	if (m_obj_buffer) // initialised?
		deinitialise();
}

//

void	BasicGeometry::initialise()
{
	if (m_obj_buffer) // already initialised?
		return;

	glGenBuffers(1, &m_obj_buffer);
}

void	BasicGeometry::deinitialise()
{
	if (!m_obj_buffer)
		return;

    glDeleteBuffers(1, &m_obj_buffer);
    m_obj_buffer = 0;
}

//

void	BasicGeometry::updateGeometry(float* pData, unsigned int length)
{
	if (!m_obj_buffer)
		return;

	glBindBuffer(GL_ARRAY_BUFFER, m_obj_buffer);

	glBufferData(GL_ARRAY_BUFFER, length * sizeof(float), pData, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_numItems = length / 6;
}

void	BasicGeometry::render(GLint primitive, float* matrix, float alpha /*= 1.0f*/) const
{
	if (!m_obj_buffer)
		return;

	auto&& pShader = Data::get()->graphic.shaders.pBasic;

	GLint attr_Position = pShader->getAttribute("a_position");
	GLint attr_Color = pShader->getAttribute("a_color");
	GLint unif_ComposedMatrix = pShader->getUniforms("u_composedMatrix");
	GLint unif_Alpha = pShader->getUniforms("u_alpha");

	glEnableVertexAttribArray(attr_Position);
	glEnableVertexAttribArray(attr_Color);

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

