

#include "StackRenderer.hpp"


#include "Shader.hpp"

#include "Utility/TraceLogger.hpp"



void	StackRenderer::create()
{
	Shader::t_def	def;
	def.vertex_filename = "assets/shaders/stack.glsl.vert.c";
	def.fragment_filename = "assets/shaders/stack.glsl.frag.c";
	def.attributes.push_back("a_Position");
	def.attributes.push_back("a_Color");
	def.uniforms.push_back("u_ComposedMatrix");

	m_pShader = Shader::build(def);

	if (!m_pShader)
		D_MYLOG("Failed to build the shader");

	//

	glGenBuffers(1, &m_vboId);
	m_vertices.reserve( 2 * 512 );

	glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.capacity() * sizeof(t_vertex), &m_vertices[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void	StackRenderer::destroy()
{
	if (m_vboId)
	{
		glDeleteBuffers(1, &m_vboId);
		m_vboId = 0;
	}

	delete m_pShader, m_pShader = nullptr;
}

//

void	StackRenderer::push_vertex(const t_pos& v, const t_color& c)
{
	if (m_vertices.size() == m_vertices.capacity())
		flush();

	m_vertices.push_back(t_vertex(v, c));
}
    
void	StackRenderer::flush()
{
	if (m_vertices.empty() ||
		!m_pMatrix ||
		!m_pShader)
		return;

	//
	//

	Shader::bind( m_pShader );

	auto attr_Position = m_pShader->getAttribute("a_Position");
	auto attr_Color = m_pShader->getAttribute("a_Color");
	auto unif_ComposedMatrix = m_pShader->getUniforms("u_ComposedMatrix");

	glEnableVertexAttribArray(attr_Position);
	glEnableVertexAttribArray(attr_Color);

		glUniformMatrix4fv( unif_ComposedMatrix, 1, GL_FALSE, m_pMatrix );

		std::size_t		bpp = sizeof(float);
		std::size_t 	stride = 7 * bpp;
		void*			index_pos = (void*)(0 * bpp);
		void* 			index_color = (void*)(3 * bpp);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboId);

			glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size() * stride, &m_vertices[0]);

			glVertexAttribPointer(attr_Position, 3, GL_FLOAT, GL_FALSE, stride, index_pos);
			glVertexAttribPointer(attr_Color, 4, GL_FLOAT, GL_FALSE, stride, index_color);

			glDrawArrays(GL_LINES, 0, m_vertices.size());

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(attr_Position);
	glDisableVertexAttribArray(attr_Color);

	Shader::bind( nullptr );

	//
	//

	m_vertices.clear();
}


