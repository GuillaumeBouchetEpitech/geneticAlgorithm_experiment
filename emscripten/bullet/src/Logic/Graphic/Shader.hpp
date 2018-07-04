

#pragma once


#include "OpenGLES.hpp"

#include <string>
#include <vector>
#include <map>


class Shader
{
public:
	struct t_def
	{
		// std::string	vertex_source;
		// std::string	fragment_source;
		std::string	vertex_filename;
		std::string	fragment_filename;
		std::vector<std::string> attributes;
		std::vector<std::string> uniforms;
	};

private:
	GLuint	m_obj_program = 0;
	std::map<std::string, GLint>	m_map_attributes;
	std::map<std::string, GLint>	m_map_uniforms;

private:
	Shader() = default;
public:
	~Shader();

public:
	static Shader*	build(const t_def& def);

public:
	static void	bind(const Shader* pShader);

public:
	GLint	getAttribute(const std::string& name) const;
	GLint	getUniforms(const std::string& name) const;

};