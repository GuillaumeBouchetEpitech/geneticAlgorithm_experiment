
#pragma once

#include "Geometry.hpp"

class ShaderProgram;

class GeometryBuilder {
private:
  Geometry::Definition _def;
  ShaderProgram* _shader = nullptr;

public:
  GeometryBuilder();
  ~GeometryBuilder() = default;

public:
  GeometryBuilder& reset();

  GeometryBuilder& setShader(ShaderProgram& shader);
  GeometryBuilder& setPrimitiveType(Geometry::PrimitiveType primitiveType);
  GeometryBuilder& addVbo();
  GeometryBuilder& setVboAsInstanced();
  GeometryBuilder& addVboAttribute(const std::string& name,
                                   Geometry::AttrType type, int index,
                                   bool isIgnored = false);
  GeometryBuilder& addVboAttribute(const std::string& name,
                                   Geometry::AttrType type);
  GeometryBuilder& addIgnoredVboAttribute(const std::string& name,
                                          Geometry::AttrType type);
  GeometryBuilder& addIgnoredVboAttribute(const std::string& name,
                                          Geometry::AttrType type, int index);
  GeometryBuilder& setVboStride(unsigned int stride);

  GeometryBuilder& validate();

  Geometry& build(Geometry& geometry);
  Geometry::Definition getDefinition();
};
