
#include "Geometry.hpp"

#include "ShaderProgram.hpp"

#include "GlContext.hpp"

#include "framework/ErrorHandler.hpp"

#include <cstdint>

//

void Geometry::initialise(ShaderProgram& shader, const Definition& def) {
  if (_vao.isAllocated() || _vbo.isAllocated())
    D_THROW(std::runtime_error, "vao/vbo already allocated");

  if (def.vbos.empty())
    D_THROW(std::runtime_error, "empty vbo defintion");

  for (const auto& vbo : def.vbos) {
    const auto& attrs = vbo.attrs;

    if (attrs.empty())
      D_THROW(std::runtime_error, "empty vbo attribute defintion");

    for (const auto& attr : attrs)
      if (!shader.hasAttribute(attr.name.c_str()))
        D_THROW(std::runtime_error,
                "attribute not found, name=\"" << attr.name << "\"");
  }

  _def = def;

  _primitiveType = def.primitiveType;

  _vao.allocate();
  _vao.bind();

  _vbo.allocate(_def.vbos.size());

  for (std::size_t ii = 0; ii < _def.vbos.size(); ++ii) {
    const auto& vbo = _def.vbos[ii];
    const auto& attrs = vbo.attrs;

    _vbo.bind(uint32_t(ii));

    uint32_t stride = vbo.stride;
    if (stride == 0) {
      // auto determine stride value
      for (const auto& attr : attrs) {
        switch (attr.type) {
        case AttrType::Float:
          stride += 1;
          break;
        case AttrType::Vec2f:
          stride += 2;
          break;
        case AttrType::Vec3f:
          stride += 3;
          break;
        case AttrType::Vec4f:
          stride += 4;
          break;
        case AttrType::Mat3f:
          stride += 9;
          break;
        case AttrType::Mat4f:
          stride += 16;
          break;
        }
      }
      stride *= uint32_t(sizeof(float));
    }

    for (const auto& attr : attrs) {
      uint32_t rowSize = 1;
      uint32_t totalRows = 1;
      switch (attr.type) {
      case AttrType::Float:
        rowSize = 1;
        break;
      case AttrType::Vec2f:
        rowSize = 2;
        break;
      case AttrType::Vec3f:
        rowSize = 3;
        break;
      case AttrType::Vec4f:
        rowSize = 4;
        break;
      case AttrType::Mat3f:
        rowSize = 3;
        totalRows = 3;
        break;
      case AttrType::Mat4f:
        rowSize = 4;
        totalRows = 4;
        break;
      }

      const int attrLocation = shader.getAttribute(attr.name.c_str());

      // TODO: check if the index is 0 on k>0 and assert/throw on it

      for (uint32_t kk = 0; kk < totalRows; ++kk) {
        const uint32_t attrId = uint32_t(attrLocation) + kk;
        const uint32_t rowIndex =
          (uint32_t(attr.index) + kk * rowSize) * uint32_t(sizeof(float));

        GlContext::enableVertexAttribArray(attrId);
        GlContext::vertexAttribPointer(attrId, rowSize, stride, rowIndex);
        if (vbo.instanced) {
          GlContext::enableVertexAttribDivisor(attrId);

          if (!_isInstanced)
            _isInstanced = true;
        }
      }
    }
  }

  _vao.unbind();
}

void Geometry::updateBuffer(uint32_t index, const void* data, uint32_t dataSize,
                            bool dynamic /*= false*/) const {
  if (!_vao.isAllocated() || !_vbo.isAllocated())
    D_THROW(std::runtime_error, "vao/vbo not allocated");

  _vbo.bind(index);

  GlContext::bufferData(data, dataSize, dynamic);

  VertexBufferObject::unbind();
}

void Geometry::render() const {
  if (!_vao.isAllocated() || !_vbo.isAllocated())
    D_THROW(std::runtime_error, "vao/vbo not allocated");

  if (_primitiveCount == 0 || (_isInstanced && _instanceCount == 0))
    return;

  _vao.bind();

  GlContext::Primitives primitive = GlContext::Primitives::lines;
  if (_primitiveType == PrimitiveType::triangles)
    primitive = GlContext::Primitives::triangles;
  else if (_primitiveType == PrimitiveType::line_strip)
    primitive = GlContext::Primitives::line_strip;

  if (_isInstanced)
    GlContext::drawArraysInstanced(primitive, _primitiveStart, _primitiveCount,
                                   _instanceCount);
  else
    GlContext::drawArrays(primitive, _primitiveStart, _primitiveCount);

  _vao.unbind();
}

void Geometry::setPrimitiveStart(uint32_t start) { _primitiveStart = start; }

void Geometry::setPrimitiveCount(uint32_t count) { _primitiveCount = count; }

void Geometry::setInstancedCount(uint32_t count) {
  if (!_isInstanced)
    D_THROW(std::runtime_error, "geometry not instanced");

  _instanceCount = count;
}
