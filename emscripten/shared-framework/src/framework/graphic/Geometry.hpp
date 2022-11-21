
#pragma once

#include "vertexBuffers/VertexBufferObject.hpp"
#include "vertexBuffers/VertexArrayObject.hpp"

#include "framework/ErrorHandler.hpp"

#include "framework/helpers/GLMath.hpp"

#include <string>
#include <vector>
#include <cstdint>

class ShaderProgram;

class Geometry
{
public:
  enum class AttrType: uint32_t
  {
    Float,
    Vec2f,
    Vec3f,
    Vec4f,
    Mat3f,
    Mat4f
  };

  enum class PrimitiveType: uint32_t
  {
    lines,
    line_strip,
    triangles,
  };

  struct Definition
  {
    struct Vbo
    {
      struct Attr
      {
        std::string name = "";
        AttrType    type = AttrType::Float;
        std::size_t index = 0;

        Attr() = default;

        Attr(const std::string& inName, AttrType inType, std::size_t inIndex)
          : name(inName)
          , type(inType)
          , index(inIndex)
        {}
      };

      unsigned int    stride = 0;
      bool            instanced = false;

      std::vector<Attr> attrs;

      Vbo() = default;
    };

    std::vector<Vbo> vbos;

    PrimitiveType primitiveType = PrimitiveType::lines;
  };

private:
  Definition          _def;
  VertexArrayObject   _vao;
  VertexBufferObject  _vbo;
  PrimitiveType       _primitiveType = PrimitiveType::lines;
  unsigned int        _primitiveStart = 0;
  unsigned int        _primitiveCount = 0;
  unsigned int        _instanceCount = 0;
  bool                _isInstanced = false;

public:
  Geometry() = default;

public:
  void initialise(ShaderProgram& shader, const Definition& def);

public:
  void updateBuffer(uint32_t index, const void* data, uint32_t dataSize, bool dynamic = false) const;
  void render() const;

public:
  template <typename VertexType>
  void updateBuffer(uint32_t index, const std::vector<VertexType>& data, bool dynamic = false) const
  {
    updateBuffer(index, static_cast<const void*>(data.data()), uint32_t(data.size()) * uint32_t(sizeof(VertexType)), dynamic);
  }

  void setPrimitiveStart(uint32_t start);
  void setPrimitiveCount(uint32_t count);
  void setInstancedCount(uint32_t count);
};
