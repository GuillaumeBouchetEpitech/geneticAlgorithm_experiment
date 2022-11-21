
#include "GeometryBuilder.hpp"

#include "ShaderProgram.hpp"

GeometryBuilder::GeometryBuilder()
{
  reset();
}

//

GeometryBuilder& GeometryBuilder::reset()
{
  _shader = nullptr;
  _def.primitiveType = Geometry::PrimitiveType::triangles;
  _def.vbos.clear();

  return *this;
}

GeometryBuilder& GeometryBuilder::setShader(ShaderProgram& shader)
{
  _shader = &shader;

  return *this;
}

GeometryBuilder& GeometryBuilder::setPrimitiveType(Geometry::PrimitiveType primitiveType)
{
  _def.primitiveType = primitiveType;

  return *this;
}

GeometryBuilder& GeometryBuilder::addVbo()
{
  _def.vbos.push_back(Geometry::Definition::Vbo());

  return *this;
}

GeometryBuilder& GeometryBuilder::setVboAsInstanced()
{
  if (_def.vbos.empty())
    D_THROW(std::runtime_error, "cannot add attrs without a vbo defined");

  _def.vbos.back().instanced = true;

  return *this;
}

GeometryBuilder& GeometryBuilder::addVboAttribute(const std::string& name,
                          Geometry::AttrType type,
                          std::size_t index /*= 0*/)
{
  if (_def.vbos.empty())
    D_THROW(std::runtime_error, "cannot add attrs without a vbo defined");

  _def.vbos.back().attrs.emplace_back(name, type, index);

  return *this;
}

GeometryBuilder& GeometryBuilder::setVboStride(unsigned int stride)
{
  if (_def.vbos.empty())
    D_THROW(std::runtime_error, "cannot add attrs without a vbo defined");

  _def.vbos.back().stride = stride;

  return *this;
}

GeometryBuilder& GeometryBuilder::validate()
{
  if (!_shader)
    D_THROW(std::runtime_error, "cannot build without a shader");

  if (_def.vbos.empty())
    D_THROW(std::runtime_error, "cannot build without a vbo defined");

  for (const auto& vbo : _def.vbos)
    if (vbo.attrs.empty())
      D_THROW(std::runtime_error, "cannot build without a vbo.attrs defined");

  return *this;
}

Geometry& GeometryBuilder::build(Geometry& geometry)
{
  validate();
  geometry.initialise(*_shader, _def);
  return geometry;
}

Geometry::Definition GeometryBuilder::getDefinition()
{
  validate();
  return _def;
}
