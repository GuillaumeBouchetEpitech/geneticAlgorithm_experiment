
#include "../GlContext.hpp"

#include "glCheckError.hpp"

#include "framework/helpers/internals/OpenGLES.hpp"

#include "framework/asValue.hpp"

#include "framework/ErrorHandler.hpp"
#include "framework/TraceLogger.hpp"

#include <memory>

namespace GlContext {

namespace VertexBufferObject {

void generateMany(uint32_t total, uint32_t* buffers) {
  glCheck(glGenBuffers(GLsizei(total), buffers));
}

void deleteMany(uint32_t total, const uint32_t* buffers) {
  glCheck(glDeleteBuffers(GLsizei(total), buffers));
}

void bind(uint32_t vboId) { glCheck(glBindBuffer(GL_ARRAY_BUFFER, vboId)); }

void enableAttribArray(uint32_t attrId) {
  glCheck(glEnableVertexAttribArray(attrId));
}

void setAttribPointer(uint32_t attrId, uint32_t rowSize, uint32_t stride,
                      uint32_t rowIndex) {
  const void* rowAddr = reinterpret_cast<void*>(rowIndex);
  glCheck(glVertexAttribPointer(attrId, GLsizei(rowSize), GL_FLOAT, GL_FALSE,
                                GLsizei(stride), rowAddr));
}

void enableAttribDivisor(uint32_t attrId) {
  glCheck(glVertexAttribDivisor(attrId, 1));
}

void uploadBuffer(const void* data, uint32_t dataSize, bool dynamic) {
  const GLenum usage = (dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

  glCheck(
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(dataSize), data, GLenum(usage)));
}

void drawArrays(Primitives primitive, uint32_t primitiveStart,
                uint32_t primitiveCount) {
  int rawPrimitive = GL_LINES;
  switch (primitive) {
  case Primitives::lines:
    rawPrimitive = GL_LINES;
    break;
  case Primitives::line_strip:
    rawPrimitive = GL_LINE_STRIP;
    break;
  case Primitives::triangles:
    rawPrimitive = GL_TRIANGLES;
    break;
  default:
    D_THROW(std::runtime_error, "unknown primitive");
    break;
  }
  glCheck(glDrawArrays(GLenum(rawPrimitive), GLint(primitiveStart),
                       GLsizei(primitiveCount)));
}

void drawInstancedArrays(Primitives primitive, uint32_t primitiveStart,
                         uint32_t primitiveCount, uint32_t instanceCount) {
  int rawPrimitive = GL_LINES;
  switch (primitive) {
  case Primitives::lines:
    rawPrimitive = GL_LINES;
    break;
  case Primitives::line_strip:
    rawPrimitive = GL_LINE_STRIP;
    break;
  case Primitives::triangles:
    rawPrimitive = GL_TRIANGLES;
    break;
  default:
    D_THROW(std::runtime_error, "unknown primitive");
    break;
  }
  glCheck(glDrawArraysInstanced(GLenum(rawPrimitive), GLint(primitiveStart),
                                GLsizei(primitiveCount),
                                GLsizei(instanceCount)));
}

} // namespace VertexBufferObject

} // namespace GlContext
