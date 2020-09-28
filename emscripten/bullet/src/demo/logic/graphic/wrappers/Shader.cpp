
#include "Shader.hpp"

#include "demo/utilities/TraceLogger.hpp"
#include "demo/utilities/ErrorHandler.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept> // <= std::invalid_argument / runtime_error
#include <memory> // <= std::unique_ptr / make_unique

namespace utilities
{
    bool getShaderSource(const std::string& filename, std::string& source)
    {
        std::ifstream istr(filename);

        if (istr.fail())
            return false;

        std::stringstream sstr;
        sstr << istr.rdbuf();

        source.clear();

        source += sstr.str();

        return true;
    }

    using GetDataFunc = void (*)(GLuint shader, GLenum pname, GLint *params);
    using GetInfoFunc = void (*)(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);

    void _printInfo(GLuint id, GetDataFunc getDataFunc, GetInfoFunc getInfoFunc)
    {
        GLint infoLogLength = 0;
        getDataFunc(id, GL_INFO_LOG_LENGTH, &infoLogLength);

        if (infoLogLength == 0)
            return;

        auto infoLog = std::make_unique<char[]>(infoLogLength);

        GLsizei lengthUsed;
        getInfoFunc(id, infoLogLength, &lengthUsed, infoLog.get());

        if (lengthUsed == 0)
            return;

        D_MYLOG("InfoLog: " << infoLog.get());
    }

    void printShaderInfo(GLuint shader)
    {
        utilities::_printInfo(shader, glGetShaderiv, glGetShaderInfoLog);
    }

    void printProgramInfo(GLuint program)
    {
        utilities::_printInfo(program, glGetProgramiv, glGetProgramInfoLog);
    }

    GLuint loadShader(GLenum type, const std::string& source)
    {
        GLuint shaderId = glCreateShader(type);

        if (shaderId == 0)
            D_THROW(std::runtime_error, "fail to create a shader"
                    << ", source=" << source);

        const char* content = source.c_str();
        const int   length = source.size();
        glShaderSource(shaderId, 1, &content, &length);
        glCompileShader(shaderId);

        GLint compiled;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled);

        if (!compiled)
        {
            utilities::printShaderInfo(shaderId);

            glDeleteShader(shaderId);

            D_THROW(std::runtime_error, "fail to compile a shader"
                    << ", source=" << source);
        }

        return shaderId;
    }
};

//

Shader::Shader(const Definition& def)
{
    std::string    vertexSourceCode;
    std::string    fragmentSourceCode;

    if (!utilities::getShaderSource(def.filenames.vertex, vertexSourceCode))
        D_THROW(std::invalid_argument, "fail to read a file"
                << ", source=" << def.filenames.vertex);

    if (!utilities::getShaderSource(def.filenames.fragment, fragmentSourceCode))
        D_THROW(std::invalid_argument, "fail to read a file"
                << ", source=" << def.filenames.fragment);

    //

    GLuint vertexShader = utilities::loadShader(GL_VERTEX_SHADER, vertexSourceCode);
    GLuint fragmentShader = utilities::loadShader(GL_FRAGMENT_SHADER, fragmentSourceCode);

    _programId = glCreateProgram();

    if (!_programId)
        D_THROW(std::invalid_argument, "fail to create a shader program"
                << ", sources: vertex=" << def.filenames.vertex
                << ", fragment=" << def.filenames.fragment);

    glAttachShader(_programId, vertexShader);
    glAttachShader(_programId, fragmentShader);
    glLinkProgram(_programId);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint linkedStatus;
    glGetProgramiv(_programId, GL_LINK_STATUS, &linkedStatus);

    if (linkedStatus == GL_FALSE)
    {
        utilities::printProgramInfo(_programId);

        glDeleteProgram(_programId);

        D_THROW(std::runtime_error, "fail to link a shader"
                << ", sources: vertex=" << def.filenames.vertex
                << ", fragment=" << def.filenames.fragment);
    }

    for (const auto& attribute : def.attributes)
    {
        if (_attributesMap.count(attribute) > 0)
            D_THROW(std::runtime_error, "duplicate attribute"
                    << ", input=" << attribute
                    << ", sources: vertex=" << def.filenames.vertex
                    << ", fragment=" << def.filenames.fragment);

        GLint location = glGetAttribLocation(_programId, attribute.c_str());

        if (location == -1)
            D_THROW(std::runtime_error, "fail to get an attribute location"
                    << ", input=" << attribute
                    << ", sources: vertex=" << def.filenames.vertex
                    << ", fragment=" << def.filenames.fragment);

        _attributesMap[attribute] = location;
    }

    for (const auto& uniform : def.uniforms)
    {
        if (_uniformsMap.count(uniform) > 0)
            D_THROW(std::runtime_error, "duplicate uniform"
                    << ", input=" << uniform
                    << ", sources: vertex=" << def.filenames.vertex
                    << ", fragment=" << def.filenames.fragment);

        GLint location = glGetUniformLocation(_programId, uniform.c_str());

        if (location == -1)
            D_THROW(std::runtime_error, "fail to get an uniform location"
                    << ", input=" << uniform
                    << ", sources: vertex=" << def.filenames.vertex
                    << ", fragment=" << def.filenames.fragment);

        _uniformsMap[uniform] = location;
    }
}

Shader::~Shader()
{
    if (_programId)
        glDeleteProgram(_programId);
}

//

void Shader::bind() const
{
    if (!_programId)
        D_THROW(std::runtime_error, "shader not initialised");

    glUseProgram(_programId);
}

void Shader::unbind()
{
    glUseProgram(0);
}

//

GLint Shader::getAttribute(const std::string& name) const
{
    auto it = _attributesMap.find(name);

    if (it == _attributesMap.end())
        D_THROW(std::invalid_argument, "unknown attribute, input=" << name);

    return it->second;
}

GLint Shader::getUniform(const std::string& name) const
{
    auto it = _uniformsMap.find(name);

    if (it == _uniformsMap.end())
        D_THROW(std::invalid_argument, "unknown uniform, input=" << name);

    return it->second;
}

bool Shader::hasAttribute(const std::string& name) const
{
    return (_attributesMap.find(name) != _attributesMap.end());
}

bool Shader::hasUniform(const std::string& name) const
{
    return (_uniformsMap.find(name) != _uniformsMap.end());
}
