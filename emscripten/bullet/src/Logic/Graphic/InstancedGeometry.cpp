

#include "InstancedGeometry.hpp"

#include "Logic/Data.hpp"
#include "Logic/Graphic/Shader.hpp"

InstancedGeometry::~InstancedGeometry()
{
    if (m_obj_bufferGeometry)
    {
        glDeleteBuffers(1, &m_obj_bufferGeometry);
        m_obj_bufferGeometry = 0;
    }

    if (m_obj_bufferMatrix)
    {
        glDeleteBuffers(1, &m_obj_bufferMatrix);
        m_obj_bufferMatrix = 0;
    }
}

void	InstancedGeometry::initialise()
{
    if (!m_obj_bufferGeometry)
        glGenBuffers(1, &m_obj_bufferGeometry);
    if (!m_obj_bufferMatrix)
        glGenBuffers(1, &m_obj_bufferMatrix);
}

void	InstancedGeometry::updateGeometry(float* pData, unsigned int length)
{
    if (!m_obj_bufferGeometry)
        return;

    glBindBuffer(GL_ARRAY_BUFFER, m_obj_bufferGeometry);

    glBufferData(GL_ARRAY_BUFFER, length * sizeof(float), pData, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_numItems = length / 6;
}

void	InstancedGeometry::updateMatrix(float* pData, unsigned int length)
{
    if (!m_obj_bufferMatrix)
        return;

    glBindBuffer(GL_ARRAY_BUFFER, m_obj_bufferMatrix);

    glBufferData(GL_ARRAY_BUFFER, length * sizeof(float), pData, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_numInstance = length / 16;
}

void	InstancedGeometry::render(GLint primitive, float* matrix) const
{
	if (!m_obj_bufferGeometry ||
        !matrix)
		return;

    auto&& pShader = Data::get()->graphic.shaders.pInstancing;

    auto attr_Position = pShader->getAttribute("a_position");
	auto attr_Color = pShader->getAttribute("a_color");
    auto attr_Transform = pShader->getAttribute("a_transform");
	auto unif_ComposedMatrix = pShader->getUniforms("u_composedMatrix");


    glUniformMatrix4fv(unif_ComposedMatrix, 1, GL_FALSE, matrix);

	glEnableVertexAttribArray(attr_Position);
	glEnableVertexAttribArray(attr_Color);

		std::size_t		bpp = sizeof(float);
		std::size_t 	stride = 6 * bpp;
		void*			index_pos = (void*)(0 * bpp);
		void* 			index_color = (void*)(3 * bpp);

		glBindBuffer(GL_ARRAY_BUFFER, m_obj_bufferGeometry);

			glVertexAttribPointer(attr_Position, 3, GL_FLOAT, GL_FALSE, stride, index_pos);
			glVertexAttribPointer(attr_Color, 3, GL_FLOAT, GL_FALSE, stride, index_color);

        std::size_t 	stride2 = 16 * bpp;
        void* 			index2_transform1 = (void*)(0 * bpp);
        void* 			index2_transform2 = (void*)(4 * bpp);
        void* 			index2_transform3 = (void*)(8 * bpp);
        void* 			index2_transform4 = (void*)(12 * bpp);

    glEnableVertexAttribArray(attr_Transform + 0);
    glEnableVertexAttribArray(attr_Transform + 1);
    glEnableVertexAttribArray(attr_Transform + 2);
    glEnableVertexAttribArray(attr_Transform + 3);

        glBindBuffer(GL_ARRAY_BUFFER, m_obj_bufferMatrix);

            glVertexAttribPointer(attr_Transform + 0, 4, GL_FLOAT, GL_FALSE, stride2, index2_transform1);
            glVertexAttribPointer(attr_Transform + 1, 4, GL_FLOAT, GL_FALSE, stride2, index2_transform2);
            glVertexAttribPointer(attr_Transform + 2, 4, GL_FLOAT, GL_FALSE, stride2, index2_transform3);
            glVertexAttribPointer(attr_Transform + 3, 4, GL_FLOAT, GL_FALSE, stride2, index2_transform4);
            glVertexAttribDivisor(attr_Transform + 0, 1);
            glVertexAttribDivisor(attr_Transform + 1, 1);
            glVertexAttribDivisor(attr_Transform + 2, 1);
            glVertexAttribDivisor(attr_Transform + 3, 1);

            glDrawArraysInstanced(primitive, 0, m_numItems, m_numInstance);

            glVertexAttribDivisor(attr_Transform + 0, 0);
            glVertexAttribDivisor(attr_Transform + 1, 0);
            glVertexAttribDivisor(attr_Transform + 2, 0);
            glVertexAttribDivisor(attr_Transform + 3, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(attr_Transform + 3);
    glDisableVertexAttribArray(attr_Transform + 2);
    glDisableVertexAttribArray(attr_Transform + 1);
    glDisableVertexAttribArray(attr_Transform + 0);
    glDisableVertexAttribArray(attr_Color);
	glDisableVertexAttribArray(attr_Position);
}
