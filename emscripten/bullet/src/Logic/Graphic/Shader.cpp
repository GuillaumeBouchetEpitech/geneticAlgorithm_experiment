

#include "Shader.hpp"

#include "Utility/TraceLogger.hpp"

#include <string>

#include <fstream>
#include <sstream>


namespace /* anonymous */
{
	GLuint	loadShader(GLenum type, const char *source)
	{
		GLuint shader = glCreateShader(type);
		
		if (shader == 0)
		{
			D_MYLOG("Error creating shader");
			return 0;
		}
		
		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);
		
		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		
		if (!compiled)
		{
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			
			if (infoLen > 1)
			{
				char* infoLog = new char[infoLen];
				glGetShaderInfoLog(shader, infoLen, NULL, infoLog);

				D_MYLOG("Error compiling shader: " << infoLog);

				delete[] infoLog;
			}
			
			glDeleteShader (shader);
			return 0;
			
		}
		
		return shader;
	}
};


Shader::~Shader()
{
	if (!m_obj_program)
		return;

	glDeleteProgram(m_obj_program);
	m_obj_program = 0;
}

//

Shader*	Shader::build(const t_def& def)
{
	Shader*	pNewShader = new Shader();

	auto& program = pNewShader->m_obj_program;

	//

	std::string	vertexSourceCode;
#ifdef EMSCRIPTEN
	vertexSourceCode += "precision mediump float;\n\n";
#endif
	{
		std::ifstream istr(def.filenames.vertex);
		std::stringstream sstr;
		sstr << istr.rdbuf();
		vertexSourceCode += sstr.str();
	}


	//

	std::string	fragmentSourceCode;
#ifdef EMSCRIPTEN
	fragmentSourceCode += "precision mediump float;\n\n";
#endif
	{
		std::ifstream istr(def.filenames.fragment);
		std::stringstream sstr;
		sstr << istr.rdbuf();
		fragmentSourceCode += sstr.str();
	}

	//

	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSourceCode.c_str());
	GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSourceCode.c_str());

	program = glCreateProgram();

	if (!program)
	{
		D_MYLOG("Could not create OpenGL program");
		delete pNewShader, pNewShader = nullptr;
	}
	else
	{
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		GLint linked;
		glGetProgramiv(program, GL_LINK_STATUS, &linked);

		if (!linked)
		{
			GLint infoLen = 0;

			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);

			if (infoLen > 1)
			{
				char* infoLog = new char [infoLen];
				glGetProgramInfoLog (program, infoLen, NULL, infoLog);

				D_MYLOG("Error linking program: " << infoLog);

				delete[] infoLog;
			}

			glDeleteProgram(program);
			delete pNewShader, pNewShader = nullptr;
		}
		else
		{
			for (auto&& attribute : def.attributes)
				pNewShader->m_map_attributes[attribute] = glGetAttribLocation(program, attribute.c_str());

			for (auto&& uniform : def.uniforms)
				pNewShader->m_map_uniforms[uniform] = glGetUniformLocation(program, uniform.c_str());
		}
	}

	return pNewShader;
}

//

void	Shader::bind(const Shader* pShader)
{
	if (pShader)
		glUseProgram(pShader->m_obj_program);
	else
		glUseProgram(0);
}

//

GLint	Shader::getAttribute(const std::string& name) const
{
	auto it = m_map_attributes.find(name);
	if (it != m_map_attributes.end())
		return it->second;
	return -1;
}

GLint	Shader::getUniforms(const std::string& name) const
{
	auto it = m_map_uniforms.find(name);
	if (it != m_map_uniforms.end())
		return it->second;
	return -1;
}
