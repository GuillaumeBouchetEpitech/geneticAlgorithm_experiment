
#pragma once

#include "Geometry.hpp"

class GeometryBuilder
{
private:
    Geometry::t_def _def;
    Shader* _shader = nullptr;

public:
    GeometryBuilder();
    ~GeometryBuilder();

public:
    GeometryBuilder& reset();
    GeometryBuilder& setShader(Shader* shader);
    GeometryBuilder& setPrimitiveType(GLint primitiveType);
    GeometryBuilder& addVbo();
    GeometryBuilder& setVboAsInstanced();
    GeometryBuilder& addVboAttribute(const std::string& name,
                                     Geometry::e_attrType type,
                                     std::size_t index = 0);
    GeometryBuilder& setVboStride(unsigned int stride);
    Geometry& build(Geometry& geometry);

};
