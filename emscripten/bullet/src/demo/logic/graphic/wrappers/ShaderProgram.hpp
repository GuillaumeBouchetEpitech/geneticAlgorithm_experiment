
#pragma once

#include "thirdparty/OpenGLES.hpp"

#include <string>
#include <vector>
#include <unordered_map>

class UniformBufferObject;

class ShaderProgram
{
private:
    friend UniformBufferObject;

public:
    struct Definition
    {
        struct t_filenames
        {
            std::string vertex;
            std::string fragment;
        }
        filenames;

        std::vector<std::string> attributes;
        std::vector<std::string> uniforms;
    };

private:
    GLuint  _programId = 0;
    std::unordered_map<std::string, GLint>  _attributesMap;
    std::unordered_map<std::string, GLint>  _uniformsMap;

public:
    ShaderProgram(const Definition& def);
    ~ShaderProgram();

public:
    void    bind() const;

public:
    static void unbind();

public:
    GLint   getAttribute(const std::string& name) const;
    GLint   getUniform(const std::string& name) const;
    bool    hasAttribute(const std::string& name) const;
    bool    hasUniform(const std::string& name) const;

};
