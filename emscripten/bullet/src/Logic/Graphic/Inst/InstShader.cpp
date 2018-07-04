

#include "InstShader.hpp"

#include "Utility/TraceLogger.hpp"


bool InstShader::initialise()
{
    Shader::t_def	def;
    def.vertex_filename = "assets/shaders/instancing.glsl.vert.c";
    def.fragment_filename = "assets/shaders/instancing.glsl.frag.c";
    def.attributes.push_back("a_Position");
    def.attributes.push_back("a_Color");
    def.attributes.push_back("a_Transform");
    def.uniforms.push_back("u_ComposedMatrix");

    m_pShader = Shader::build(def);

    if (!m_pShader)
        D_MYLOG("Failed to build the shader");

    return (m_pShader != nullptr);
}
