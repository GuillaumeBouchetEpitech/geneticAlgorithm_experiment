
#include "../GlContext.hpp"

#include "glCheckError.hpp"

#include "framework/helpers/internals/OpenGLES.hpp"
#include "framework/system/ErrorHandler.hpp"
#include "framework/system/TraceLogger.hpp"
#include "framework/system/asValue.hpp"

#include <memory>

namespace GlContext {

//
//
//
//
//

namespace RenderBuffer {

void generateMany(uint32_t total, uint32_t* buffers) {
  glCheck(glGenRenderbuffers(GLsizei(total), buffers));
}

void deleteMany(uint32_t total, const uint32_t* buffers) {
  glCheck(glDeleteRenderbuffers(GLsizei(total), buffers));
}

void bind(uint32_t renderBufferId) {
  glCheck(glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId));
}

namespace {
int getRawDepthFormat(DepthFormat depthFormat) {
  switch (depthFormat) {
  case DepthFormat::depth24:
    return GL_DEPTH_COMPONENT24;
  case DepthFormat::depth32:
    return GL_DEPTH_COMPONENT32;
  case DepthFormat::depth32f:
    return GL_DEPTH_COMPONENT32F;
  default:
    return GL_DEPTH_COMPONENT16;
  }
}
} // namespace

void setSize(uint32_t width, uint32_t height, DepthFormat depthFormat) {

  const int internalFormat = getRawDepthFormat(depthFormat);

  glCheck(glRenderbufferStorage(GL_RENDERBUFFER, GLenum(internalFormat),
                                GLsizei(width), GLsizei(height)));
}

} // namespace RenderBuffer

//
//
//
//
//

namespace VertexArrayObject {

void generateMany(uint32_t total, uint32_t* buffers) {
  glCheck(glGenVertexArrays(GLsizei(total), buffers));
}

void deleteMany(uint32_t total, const uint32_t* buffers) {
  glCheck(glDeleteVertexArrays(GLsizei(total), buffers));
}

void bind(uint32_t vaoId) { glCheck(glBindVertexArray(vaoId)); }

} // namespace VertexArrayObject

//
//
//
//
//

std::string getVersion() {
  const unsigned char* msg = glGetString(GL_VERSION);
  return std::string(reinterpret_cast<const char*>(msg));
}

void setViewport(int x, int y, uint32_t width, uint32_t height) {
  glCheck(glViewport(x, y, GLsizei(width), GLsizei(height)));
}

void setScissor(int x, int y, uint32_t width, uint32_t height) {
  glCheck(glScissor(x, y, GLsizei(width), GLsizei(height)));
}

void setDepthFunc(DepthFuncs func) {
  switch (func) {
  case DepthFuncs::never:
    glCheck(glDepthFunc(GL_NEVER));
    break;
  case DepthFuncs::less:
    glCheck(glDepthFunc(GL_LESS));
    break;
  case DepthFuncs::equal:
    glCheck(glDepthFunc(GL_EQUAL));
    break;
  case DepthFuncs::lessAndEqual:
    glCheck(glDepthFunc(GL_LEQUAL));
    break;
  case DepthFuncs::greater:
    glCheck(glDepthFunc(GL_GREATER));
    break;
  case DepthFuncs::notEqual:
    glCheck(glDepthFunc(GL_NOTEQUAL));
    break;
  case DepthFuncs::greaterAndEqual:
    glCheck(glDepthFunc(GL_GEQUAL));
    break;
  case DepthFuncs::always:
    glCheck(glDepthFunc(GL_ALWAYS));
    break;

  default:
    D_THROW(std::runtime_error, "unknown depth func");
    break;
  }
}

// enum class BlendFuncs
// {
//   srcAlpha,
//   oneMinuxSrcAlpha,
// };

// GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA

namespace {
GLenum getRawBlendFunc(BlendFuncs func) {
  switch (func) {
  case BlendFuncs::srcAlpha:
    return GL_SRC_ALPHA;
    break;
  case BlendFuncs::oneMinuxSrcAlpha:
    return GL_ONE_MINUS_SRC_ALPHA;
    break;

  default:
    D_THROW(std::runtime_error, "unknown blend func");
    break;
  }
}
} // namespace

void setBlendFunc(BlendFuncs sfactor, BlendFuncs dfactor) {
  glCheck(glBlendFunc(getRawBlendFunc(sfactor), getRawBlendFunc(dfactor)));
}

void enable(States state) {
  switch (state) {
  case States::cullFace:
    glCheck(glEnable(GL_CULL_FACE));
    break;
  case States::depthTest:
    glCheck(glEnable(GL_DEPTH_TEST));
    break;
  case States::blend:
    glCheck(glEnable(GL_BLEND));
    break;
  case States::scissorTest:
    glCheck(glEnable(GL_SCISSOR_TEST));
    break;

  default:
    D_THROW(std::runtime_error, "unknown state");
    break;
  }
}

void disable(States state) {
  switch (state) {
  case States::cullFace:
    glCheck(glDisable(GL_CULL_FACE));
    break;
  case States::depthTest:
    glCheck(glDisable(GL_DEPTH_TEST));
    break;
  case States::blend:
    glCheck(glDisable(GL_BLEND));
    break;
  case States::scissorTest:
    glCheck(glDisable(GL_SCISSOR_TEST));
    break;

  default:
    D_THROW(std::runtime_error, "unknown state");
    break;
  }
}

void setBackFaceCullingDirection(BackFaceCullingDirection direction) {
  switch (direction) {
  case BackFaceCullingDirection::clockWise:
    glCheck(glFrontFace(GL_CW));
    break;
  case BackFaceCullingDirection::counterClockWise:
    glCheck(glFrontFace(GL_CCW));
    break;

  default:
    D_THROW(std::runtime_error, "unknown backface culling direction");
    break;
  }
}

void clear(int mask) {
  int rawMask = 0;
  if (mask & asValue(Buffers::color))
    rawMask |= GL_COLOR_BUFFER_BIT;
  if (mask & asValue(Buffers::depth))
    rawMask |= GL_DEPTH_BUFFER_BIT;

  if (mask == 0) {
    D_THROW(std::runtime_error, "clear: empty mask");
  }

  glCheck(glClear(GLbitfield(rawMask)));
}

void clearColor(float r, float g, float b, float a) {
  glCheck(glClearColor(r, g, b, a));
}

void clearDepth(float value) { glCheck(glClearDepthf(value)); }

// void polygonModeAsLine()
// {
//   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
// }

// void polygonModeAsFill()
// {
//   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
// }

int getMaxTextureSize() {
  int maxTextureSize = 0;
  glCheck(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize));
  return maxTextureSize;
}

} // namespace GlContext
