

#include "Shader.hpp"

#include "Utility/TraceLogger.hpp"

#include <string>

#include <fstream>
#include <sstream>


namespace /* anonymous */
{
	// void sCheckGLError()
	// {
	// 	GLenum errCode = glGetError();
	// 	if (errCode != GL_NO_ERROR)
	// 	{
	// 		std::cout << "OpenGL error = " << errCode << std::endl;
	// 		// assert(false);
	// 	}
	// }

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




// Shader::Shader()
// {}

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

	std::string	vertex_source;
#ifdef EMSCRIPTEN
	vertex_source += "precision mediump float;\n\n";
#endif
	{
		std::ifstream istr(def.vertex_filename);
		std::stringstream sstr;
		sstr << istr.rdbuf();
		vertex_source += sstr.str();
	}


	//

	std::string	fragment_source;
#ifdef EMSCRIPTEN
	fragment_source += "precision mediump float;\n\n";
#endif
	{
		std::ifstream istr(def.fragment_filename);
		std::stringstream sstr;
		sstr << istr.rdbuf();
		fragment_source += sstr.str();
	}

	//

	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertex_source.c_str());
	GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragment_source.c_str());

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

			// m_attr_Position = glGetAttribLocation(program, "a_Position");
			// m_attr_Color = glGetAttribLocation(program, "a_Color");

			// m_unif_ComposedMatrix = glGetUniformLocation(program, "u_ComposedMatrix");
			// m_unif_Alpha = glGetUniformLocation(program, "u_Alpha");
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

	// sCheckGLError();
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
