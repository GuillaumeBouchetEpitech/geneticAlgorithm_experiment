
#pragma once

#include "Geometry.hpp"

class ShaderProgram;

class GeometryBuilder
{
private:
    Geometry::Definition _def;
    ShaderProgram* _shader = nullptr;

public:
    GeometryBuilder();
    ~GeometryBuilder();

public:
    GeometryBuilder& reset();
    GeometryBuilder& setShader(ShaderProgram& shader);
    GeometryBuilder& setPrimitiveType(GLint primitiveType);
    GeometryBuilder& addVbo();
    GeometryBuilder& setVboAsInstanced();
    GeometryBuilder& addVboAttribute(const std::string& name,
                                     Geometry::AttrType type,
                                     std::size_t index = 0);
    GeometryBuilder& setVboStride(unsigned int stride);
    Geometry& build(Geometry& geometry);

};
