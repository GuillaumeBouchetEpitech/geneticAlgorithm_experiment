
#pragma once

#include "demo/helpers/OpenGLES.hpp"
#include "demo/helpers/GLMath.hpp"

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
        struct Filenames
        {
            std::string vertex;
            std::string fragment;
        }
        filenames;

        std::vector<std::string> attributes;
        std::vector<std::string> uniforms;
    };

private:
    GLuint _programId = 0;
    std::unordered_map<std::string, GLint> _attributesMap;
    std::unordered_map<std::string, GLint> _uniformsMap;

public:
    ShaderProgram(const Definition& def);
    ~ShaderProgram();

public:
    void bind() const;

public:
    static void unbind();

public:
    GLint getAttribute(const char* name) const;
    GLint getUniform(const char* name) const;
    bool hasAttribute(const char* name) const;
    bool hasUniform(const char* name) const;

public:
    void setUniform(const char* name, float value) const;
    void setUniform(const char* name, float x, float y, float z, float w) const;
    void setUniform(const char* name, const glm::vec4& vec4) const;
    void setUniform(const char* name, const glm::mat4& mat4) const;

public:
    void setUniform(GLint location, float value) const;
    void setUniform(GLint location, float x, float y, float z, float w) const;
    void setUniform(GLint location, const glm::vec4& vec4) const;
    void setUniform(GLint location, const glm::mat4& mat4) const;
};
