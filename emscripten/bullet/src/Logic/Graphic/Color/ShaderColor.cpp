

#include "ShaderColor.hpp"

#include "../Shader.hpp"

#include "Utility/TraceLogger.hpp"


ShaderColor::~ShaderColor()
{
	delete m_pShader, m_pShader = nullptr;
}

//

bool	ShaderColor::initialise()
{
	delete m_pShader, m_pShader = nullptr;

	//

	Shader::t_def	def;
    def.vertex_filename = "assets/shaders/color.glsl.vert.c";
    def.fragment_filename = "assets/shaders/color.glsl.frag.c";
	def.attributes.push_back("a_Position");
	def.attributes.push_back("a_Color");
	def.uniforms.push_back("u_ComposedMatrix");
	def.uniforms.push_back("u_Alpha");


	m_pShader = Shader::build(def);

	if (!m_pShader)
		D_MYLOG("Failed to build the shader");

	return (m_pShader != nullptr);
}
