

#include "InstGeometry.hpp"

#include "InstShader.hpp"


InstGeometry::~InstGeometry()
{
    if (m_obj_buffer)
    {
        glDeleteBuffers(1, &m_obj_buffer);
        m_obj_buffer = 0;
    }

    if (m_obj_buffer2)
    {
        glDeleteBuffers(1, &m_obj_buffer2);
        m_obj_buffer2 = 0;
    }
}

void	InstGeometry::initialise(InstShader* pShader)
{
    if (pShader)
        m_pInstShader = pShader;

    if (!m_obj_buffer)
        glGenBuffers(1, &m_obj_buffer);
    if (!m_obj_buffer2)
        glGenBuffers(1, &m_obj_buffer2);
}

void	InstGeometry::update(float* pData, unsigned int length)
{
    if (!m_obj_buffer)
        return;

    glBindBuffer(GL_ARRAY_BUFFER, m_obj_buffer);

    glBufferData(GL_ARRAY_BUFFER, length, pData, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_numItems = length / 6 / sizeof(float);
}

void	InstGeometry::update2(float* pData, unsigned int length)
{
    if (!m_obj_buffer2)
        return;

    glBindBuffer(GL_ARRAY_BUFFER, m_obj_buffer2);

    glBufferData(GL_ARRAY_BUFFER, length, pData, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_numInstance = length / 16 / sizeof(float);
}

void	InstGeometry::render(GLint primitive, float* matrix)
{
	if (!m_obj_buffer ||
		!m_pInstShader ||
        !matrix)
		return;

    auto attr_Position = m_pInstShader->getShader().getAttribute("a_Position");
	auto attr_Color = m_pInstShader->getShader().getAttribute("a_Color");
    auto attr_Transform = m_pInstShader->getShader().getAttribute("a_Transform");
	auto unif_ComposedMatrix = m_pInstShader->getShader().getUniforms("u_ComposedMatrix");


    glUniformMatrix4fv( unif_ComposedMatrix, 1, GL_FALSE, matrix );

	glEnableVertexAttribArray(attr_Position);
	glEnableVertexAttribArray(attr_Color);

		std::size_t		bpp = sizeof(float);
		std::size_t 	stride = 6 * bpp;
		void*			index_pos = (void*)(0 * bpp);
		void* 			index_color = (void*)(3 * bpp);

		glBindBuffer(GL_ARRAY_BUFFER, m_obj_buffer);

			glVertexAttribPointer(attr_Position, 3, GL_FLOAT, GL_FALSE, stride, index_pos);
			glVertexAttribPointer(attr_Color, 3, GL_FLOAT, GL_FALSE, stride, index_color);

        std::size_t 	stride2 = 16 * bpp;
        void* 			index2_transform1 = (void*)(0 * bpp);
        void* 			index2_transform2 = (void*)(4 * bpp);
        void* 			index2_transform3 = (void*)(8 * bpp);
        void* 			index2_transform4 = (void*)(12 * bpp);

    glEnableVertexAttribArray(attr_Transform+0);
    glEnableVertexAttribArray(attr_Transform+1);
    glEnableVertexAttribArray(attr_Transform+2);
    glEnableVertexAttribArray(attr_Transform+3);

        glBindBuffer(GL_ARRAY_BUFFER, m_obj_buffer2);

            glVertexAttribPointer(attr_Transform+0, 4, GL_FLOAT, GL_FALSE, stride2, index2_transform1);
            glVertexAttribPointer(attr_Transform+1, 4, GL_FLOAT, GL_FALSE, stride2, index2_transform2);
            glVertexAttribPointer(attr_Transform+2, 4, GL_FLOAT, GL_FALSE, stride2, index2_transform3);
            glVertexAttribPointer(attr_Transform+3, 4, GL_FLOAT, GL_FALSE, stride2, index2_transform4);
            glVertexAttribDivisor(attr_Transform+0, 1); // This makes it instanced!
            glVertexAttribDivisor(attr_Transform+1, 1); // This makes it instanced!
            glVertexAttribDivisor(attr_Transform+2, 1); // This makes it instanced!
            glVertexAttribDivisor(attr_Transform+3, 1); // This makes it instanced!

            glDrawArraysInstanced(primitive, 0, m_numItems, m_numInstance);

            glVertexAttribDivisor(attr_Transform+0, 0);
            glVertexAttribDivisor(attr_Transform+1, 0);
            glVertexAttribDivisor(attr_Transform+2, 0);
            glVertexAttribDivisor(attr_Transform+3, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(attr_Transform+3);
    glDisableVertexAttribArray(attr_Transform+2);
    glDisableVertexAttribArray(attr_Transform+1);
    glDisableVertexAttribArray(attr_Transform+0);
    glDisableVertexAttribArray(attr_Color);
	glDisableVertexAttribArray(attr_Position);
}
