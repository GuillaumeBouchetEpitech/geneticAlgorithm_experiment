
#include "GlContext.hpp"

#include "framework/helpers/OpenGLES.hpp"

#include "framework/asValue.hpp"

#include "framework/ErrorHandler.hpp"

#include <memory>

namespace utilities
{

  using GetDataFunc = void (*)(uint32_t shader, GLenum pname, int *params);
  using GetInfoFunc = void (*)(uint32_t shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);

  void _printInfo(uint32_t id, GetDataFunc getDataFunc, GetInfoFunc getInfoFunc)
  {
    int infoLogLength = 0;
    getDataFunc(id, GL_INFO_LOG_LENGTH, &infoLogLength);

    if (infoLogLength == 0)
      return;

    auto infoLog = std::make_unique<char[]>(std::size_t(infoLogLength));

    GLsizei lengthUsed;
    getInfoFunc(id, infoLogLength, &lengthUsed, infoLog.get());

    if (lengthUsed == 0)
      return;

    D_MYLOG("InfoLog: " << infoLog.get());
  }

  uint32_t loadShader(GLenum type, const std::string& source)
  {
    uint32_t shaderId = glCreateShader(type);

    if (shaderId == 0)
      D_THROW(std::runtime_error, "fail to create a shader"
              << ", source=" << source);

    const char* content = source.c_str();
    const int   length = int(source.size());
    glShaderSource(shaderId, 1, &content, &length);
    glCompileShader(shaderId);

    int compiled;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
      GlContext::printShaderInfo(shaderId);

      glDeleteShader(shaderId);

      D_THROW(std::runtime_error, "fail to compile a shader"
              << ", source=" << source);
    }

    return shaderId;
  }

}


void GlContext::genFramebuffers(uint32_t total, uint32_t* buffers)
{
  glGenFramebuffers(GLsizei(total), buffers);
}

void GlContext::deleteFramebuffers(uint32_t total, const uint32_t* buffers)
{
  glDeleteFramebuffers(GLsizei(total), buffers);
}

uint32_t GlContext::genFramebuffer()
{
  uint32_t bufferId;
  genFramebuffers(1, &bufferId);
  return bufferId;
}

void GlContext::deleteFramebuffer(uint32_t bufferId)
{
  deleteFramebuffers(1, &bufferId);
}

void GlContext::framebufferTexture2D(uint32_t textureId)
{
  constexpr int level = 0;
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, level);
}

void GlContext::framebufferRenderbuffer(uint32_t bufferId)
{
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, bufferId);
}

void GlContext::checkFrameBuffer()
{
  const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if (status == GL_FRAMEBUFFER_UNDEFINED)
    D_THROW(std::runtime_error, "incomplete frame buffer GL_FRAMEBUFFER_UNDEFINED");
  if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
    D_THROW(std::runtime_error, "incomplete frame buffer GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
  if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
    D_THROW(std::runtime_error, "incomplete frame buffer GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
  if (status == GL_FRAMEBUFFER_UNSUPPORTED)
    D_THROW(std::runtime_error, "incomplete frame buffer GL_FRAMEBUFFER_UNSUPPORTED");
  if (status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)
    D_THROW(std::runtime_error, "incomplete frame buffer GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
  if (status != GL_FRAMEBUFFER_COMPLETE)
    D_THROW(std::runtime_error, "incomplete frame buffer");
}

void GlContext::bindFramebuffer(uint32_t frameBufferId)
{
  glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
}

//
//
//
//
//

void GlContext::enableVertexAttribArray(uint32_t attrId)
{
  glEnableVertexAttribArray(attrId);
}

void GlContext::vertexAttribPointer(uint32_t attrId, uint32_t rowSize, uint32_t stride, uint32_t rowIndex)
{
  const void* rowAddr = reinterpret_cast<void*>(rowIndex);
  glVertexAttribPointer(attrId, GLsizei(rowSize), GL_FLOAT, GL_FALSE, GLsizei(stride), rowAddr);
}

void GlContext::enableVertexAttribDivisor(uint32_t attrId)
{
  glVertexAttribDivisor(attrId, 1);
}

void GlContext::bufferData(const void* data, uint32_t dataSize, bool dynamic)
{
  const GLenum usage = (dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

  glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(dataSize), data, GLenum(usage));
}

void GlContext::drawArrays(Primitives primitive, uint32_t primitiveStart, uint32_t primitiveCount)
{
  int rawPrimitive = GL_LINES;
  switch (primitive)
  {
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
  glDrawArrays(GLenum(rawPrimitive), GLint(primitiveStart), GLsizei(primitiveCount));
}

void GlContext::drawArraysInstanced(Primitives primitive, uint32_t primitiveStart, uint32_t primitiveCount, uint32_t instanceCount)
{
  int rawPrimitive = GL_LINES;
  switch (primitive)
  {
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
  glDrawArraysInstanced(GLenum(rawPrimitive), GLint(primitiveStart), GLsizei(primitiveCount), GLsizei(instanceCount));
}

//
//
//
//
//

void GlContext::genRenderbuffers(uint32_t total, uint32_t* buffers)
{
  glGenRenderbuffers(GLsizei(total), buffers);
}

void GlContext::deleteRenderbuffers(uint32_t total, const uint32_t* buffers)
{
  glDeleteRenderbuffers(GLsizei(total), buffers);
}

void GlContext::bindRenderbuffer(uint32_t renderBufferId)
{
  glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId);
}

void GlContext::renderbufferStorage(uint32_t width, uint32_t height)
{
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, GLsizei(width), GLsizei(height));
}

//
//
//
//
//



void GlContext::printShaderInfo(uint32_t shader)
{
  utilities::_printInfo(shader, glGetShaderiv, glGetShaderInfoLog);
}

void GlContext::printProgramInfo(uint32_t program)
{
  utilities::_printInfo(program, glGetProgramiv, glGetProgramInfoLog);
}

uint32_t GlContext::loadVertexShader(const std::string& source)
{
  return utilities::loadShader(GL_VERTEX_SHADER, source);
}

uint32_t GlContext::loadFragmentShader(const std::string& source)
{
  return utilities::loadShader(GL_FRAGMENT_SHADER, source);
}

uint32_t GlContext::createProgram()
{
  return glCreateProgram();
}

void GlContext::deleteProgram(uint32_t programId)
{
  glDeleteProgram(programId);
}

bool GlContext::linkProgram(uint32_t programId, uint32_t vertexShader, uint32_t fragmentShader)
{
  glAttachShader(programId, vertexShader);
  glAttachShader(programId, fragmentShader);
  glLinkProgram(programId);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  int linkedStatus;
  glGetProgramiv(programId, GL_LINK_STATUS, &linkedStatus);

  return (linkedStatus != GL_FALSE);
}

int GlContext::getAttribLocation(uint32_t programId, const char* name)
{
  return glGetAttribLocation(programId, name);
}

int GlContext::getUniformLocation(uint32_t programId, const char* name)
{
  return glGetUniformLocation(programId, name);
}

void GlContext::useProgram(uint32_t programId)
{
  glUseProgram(programId);
}

void GlContext::setUniform(int location, float value)
{
  glUniform1f(location, value);
}

void GlContext::setUniform(int location, float x, float y, float z)
{
  glUniform3f(location, x, y, z);
}

void GlContext::setUniform(int location, float x, float y, float z, float w)
{
  glUniform4f(location, x, y, z, w);
}

void GlContext::setUniform(int location, const glm::mat4& matrix)
{
  glUniformMatrix4fv(location, 1, false, glm::value_ptr(matrix));
}

//
//
//
//
//

void GlContext::genTextures(uint32_t total, uint32_t* buffers)
{
  glGenTextures(GLsizei(total), buffers);
}

void GlContext::deleteTextures(uint32_t total, const uint32_t* buffers)
{
  glDeleteTextures(GLsizei(total), buffers);
}

uint32_t GlContext::genTexture()
{
  uint32_t bufferId;
  genTextures(1, &bufferId);
  return bufferId;
}

void GlContext::deleteTexture(uint32_t textureId)
{
  deleteTextures(1, &textureId);
}

void GlContext::bindTexture(uint32_t textureId)
{
  glBindTexture(GL_TEXTURE_2D, textureId);
}

void GlContext::uploadPixels(uint32_t width, uint32_t height, const void* pixels)
{
  GLint level = 0;
  GLint border = 0;
  glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, GLsizei(width), GLsizei(height), border, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

void GlContext::setTextureAsRepeat(bool repeat)
{
  const int wrapValue = (repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapValue);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapValue);
}


void GlContext::setTextureAsPixelated()
{
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void GlContext::setTextureAsSmoothed(bool generateMipMap /* = false */)
{
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);

  if (generateMipMap)
    glGenerateMipmap(GL_TEXTURE_2D);
}

//
//
//
//
//

void GlContext::genVertexArrays(uint32_t total, uint32_t* buffers)
{
  glGenVertexArrays(GLsizei(total), buffers);
}

void GlContext::deleteVertexArrays(uint32_t total, const uint32_t* buffers)
{
  glDeleteVertexArrays(GLsizei(total), buffers);
}

void GlContext::bindVertexArray(uint32_t vaoId)
{
  glBindVertexArray(vaoId);
}

//
//
//
//
//

void GlContext::genBuffers(uint32_t total, uint32_t* buffers)
{
  glGenBuffers(GLsizei(total), buffers);
}

void GlContext::deleteBuffers(uint32_t total, const uint32_t* buffers)
{
  glDeleteBuffers(GLsizei(total), buffers);
}

void GlContext::bindBuffer(uint32_t vboId)
{
  glBindBuffer(GL_ARRAY_BUFFER, vboId);
}

//
//
//
//
//

std::string GlContext::getVersion()
{
  const unsigned char* msg = glGetString(GL_VERSION);
  return std::string(reinterpret_cast<const char*>(msg));
}

void GlContext::setViewport(int x, int y, uint32_t width, uint32_t height)
{
  glViewport(x, y, GLsizei(width), GLsizei(height));
}

void GlContext::setScissor(int x, int y, uint32_t width, uint32_t height)
{
  glScissor(x, y, GLsizei(width), GLsizei(height));
}

void GlContext::setDepthFunc(DepthFuncs func)
{
  switch (func)
  {
  case DepthFuncs::never: glDepthFunc(GL_NEVER); break;
  case DepthFuncs::less: glDepthFunc(GL_LESS); break;
  case DepthFuncs::equal: glDepthFunc(GL_EQUAL); break;
  case DepthFuncs::lessAndEqual: glDepthFunc(GL_LEQUAL); break;
  case DepthFuncs::greater: glDepthFunc(GL_GREATER); break;
  case DepthFuncs::notEqual: glDepthFunc(GL_NOTEQUAL); break;
  case DepthFuncs::greaterAndEqual: glDepthFunc(GL_GEQUAL); break;
  case DepthFuncs::always: glDepthFunc(GL_ALWAYS); break;

  default:
    D_THROW(std::runtime_error, "unknown depth func");
    break;
  }

}

  enum class BlendFuncs
  {
    srcAlpha,
    oneMinuxSrcAlpha,
  };

// GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA

namespace
{
  GLenum getRawBlendFunc(GlContext::BlendFuncs func)
  {
    switch (func)
    {
    case GlContext::BlendFuncs::srcAlpha: return GL_SRC_ALPHA; break;
    case GlContext::BlendFuncs::oneMinuxSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA; break;

    default:
      D_THROW(std::runtime_error, "unknown blend func");
      break;
    }
  }
}

void GlContext::setBlendFunc(BlendFuncs sfactor, BlendFuncs dfactor)
{
  glBlendFunc(getRawBlendFunc(sfactor), getRawBlendFunc(dfactor));
}

void GlContext::enable(States state)
{
  switch (state)
  {
  case States::cullFace: glEnable(GL_CULL_FACE); break;
  case States::depthTest: glEnable(GL_DEPTH_TEST); break;
  case States::blend: glEnable(GL_BLEND); break;
  case States::scissorTest: glEnable(GL_SCISSOR_TEST); break;

  default:
    D_THROW(std::runtime_error, "unknown state");
    break;
  }
}

void GlContext::disable(States state)
{
  switch (state)
  {
  case States::cullFace: glDisable(GL_CULL_FACE); break;
  case States::depthTest: glDisable(GL_DEPTH_TEST); break;
  case States::blend: glDisable(GL_BLEND); break;
  case States::scissorTest: glDisable(GL_SCISSOR_TEST); break;

  default:
    D_THROW(std::runtime_error, "unknown state");
    break;
  }
}

void GlContext::setBackFaceCullingDirection(BackFaceCullingDirection direction)
{
  switch (direction)
  {
  case BackFaceCullingDirection::clockWise : glFrontFace(GL_CW); break;
  case BackFaceCullingDirection::counterClockWise : glFrontFace(GL_CCW); break;

  default:
    D_THROW(std::runtime_error, "unknown backface culling direction");
    break;
  }
}

void GlContext::clear(int mask)
{
  int rawMask = 0;
  if (mask & asValue(Buffers::color)) rawMask |= GL_COLOR_BUFFER_BIT;
  if (mask & asValue(Buffers::depth)) rawMask |= GL_DEPTH_BUFFER_BIT;

  if (mask == 0)
  {
    D_THROW(std::runtime_error, "clear: empty mask");
  }

  glClear(GLbitfield(rawMask));
}

void GlContext::clearColor(float r, float g, float b, float a)
{
  glClearColor(r, g, b, a);
}

// void GlContext::polygonModeAsLine()
// {
//   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
// }

// void GlContext::polygonModeAsFill()
// {
//   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
// }
