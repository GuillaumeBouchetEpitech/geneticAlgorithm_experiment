
#include "GlContext.hpp"

#include "framework/helpers/internals/OpenGLES.hpp"

#include "framework/asValue.hpp"

#include "framework/ErrorHandler.hpp"
#include "framework/TraceLogger.hpp"

#include <memory>

namespace debug
{

  void glCheckError(const char* file, unsigned int line, const char* expression)
  {
    // Get the last error
    GLenum errorCode = glGetError();

    if (errorCode == GL_NO_ERROR)
      return;

    std::string error       = "Unknown error";
    std::string description = "No description";

    // Decode the error code
    switch (errorCode)
    {
      case GL_INVALID_ENUM:
      {
        error       = "GL_INVALID_ENUM";
        description = "An unacceptable value has been specified for an enumerated argument.";
        break;
      }

      case GL_INVALID_VALUE:
      {
        error       = "GL_INVALID_VALUE";
        description = "A numeric argument is out of range.";
        break;
      }

      case GL_INVALID_OPERATION:
      {
        error       = "GL_INVALID_OPERATION";
        description = "The specified operation is not allowed in the current state.";
        break;
      }

      case GL_STACK_OVERFLOW:
      {
        error       = "GL_STACK_OVERFLOW";
        description = "This command would cause a stack overflow.";
        break;
      }

      case GL_STACK_UNDERFLOW:
      {
        error       = "GL_STACK_UNDERFLOW";
        description = "This command would cause a stack underflow.";
        break;
      }

      case GL_OUT_OF_MEMORY:
      {
        error       = "GL_OUT_OF_MEMORY";
        description = "There is not enough memory left to execute the command.";
        break;
      }

      // case GLEXT_GL_INVALID_FRAMEBUFFER_OPERATION:
      // {
      //   error       = "GL_INVALID_FRAMEBUFFER_OPERATION";
      //   description = "The object bound to FRAMEBUFFER_BINDING is not \"framebuffer complete\".";
      //   break;
      // }
    }

    D_MYLOG("An internal OpenGL call failed in " << file << "(" << line << ").");
    D_MYLOG("Expression:")
    D_MYLOG(expression);
    D_MYLOG("Error description:");
    D_MYLOG(error);
    D_MYLOG("     " << description);
  }

}

#if 0

#define glCheck(expr) \
  { \
    expr; \
    debug::glCheckError(__FILE__, __LINE__, #expr); \
  } \

#else

#define glCheck(expr) (expr)

#endif


namespace utilities
{

  using GetDataFunc = void (*)(uint32_t shader, GLenum pname, int *params);
  using GetInfoFunc = void (*)(uint32_t shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);

  void _printInfo(uint32_t id, GetDataFunc getDataFunc, GetInfoFunc getInfoFunc)
  {
    int infoLogLength = 0;
    glCheck(getDataFunc(id, GL_INFO_LOG_LENGTH, &infoLogLength));

    if (infoLogLength == 0)
      return;

    auto infoLog = std::make_unique<char[]>(std::size_t(infoLogLength));

    GLsizei lengthUsed;
    glCheck(getInfoFunc(id, infoLogLength, &lengthUsed, infoLog.get()));

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
    glCheck(glShaderSource(shaderId, 1, &content, &length));
    glCheck(glCompileShader(shaderId));

    int compiled;
    glCheck(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled));

    if (!compiled)
    {
      GlContext::printShaderInfo(shaderId);

      glCheck(glDeleteShader(shaderId));

      D_THROW(std::runtime_error, "fail to compile a shader"
              << ", source=" << source);
    }

    return shaderId;
  }

}


void GlContext::genFramebuffers(uint32_t total, uint32_t* buffers)
{
  glCheck(glGenFramebuffers(GLsizei(total), buffers));
}

void GlContext::deleteFramebuffers(uint32_t total, const uint32_t* buffers)
{
  glCheck(glDeleteFramebuffers(GLsizei(total), buffers));
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

void GlContext::framebufferTexture2D(uint32_t index, uint32_t textureId)
{
  constexpr int level = 0;
  glCheck(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, textureId, level));
}

void GlContext::framebufferDepthTexture2D(uint32_t textureId)
{
  constexpr GLint level = 0;
  glCheck(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureId, level));
}



void GlContext::framebufferRenderbuffer(uint32_t bufferId)
{
  glCheck(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, bufferId));
}

void GlContext::checkFrameBuffer()
{
  GLenum status = 0;
  glCheck(status = glCheckFramebufferStatus(GL_FRAMEBUFFER));

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

void GlContext::drawFrameBuffers(uint32_t size, uint32_t* buffers)
{
  glCheck(glDrawBuffers(GLsizei(size), static_cast<GLenum*>(buffers)));
}

void GlContext::bindFramebuffer(uint32_t frameBufferId)
{
  glCheck(glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId));
}

//
//
//
//
//

void GlContext::enableVertexAttribArray(uint32_t attrId)
{
  glCheck(glEnableVertexAttribArray(attrId));
}

void GlContext::vertexAttribPointer(uint32_t attrId, uint32_t rowSize, uint32_t stride, uint32_t rowIndex)
{
  const void* rowAddr = reinterpret_cast<void*>(rowIndex);
  glCheck(glVertexAttribPointer(attrId, GLsizei(rowSize), GL_FLOAT, GL_FALSE, GLsizei(stride), rowAddr));
}

void GlContext::enableVertexAttribDivisor(uint32_t attrId)
{
  glCheck(glVertexAttribDivisor(attrId, 1));
}

void GlContext::bufferData(const void* data, uint32_t dataSize, bool dynamic)
{
  const GLenum usage = (dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

  glCheck(glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(dataSize), data, GLenum(usage)));
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
  glCheck(glDrawArrays(GLenum(rawPrimitive), GLint(primitiveStart), GLsizei(primitiveCount)));
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
  glCheck(glDrawArraysInstanced(GLenum(rawPrimitive), GLint(primitiveStart), GLsizei(primitiveCount), GLsizei(instanceCount)));
}

//
//
//
//
//

void GlContext::genRenderbuffers(uint32_t total, uint32_t* buffers)
{
  glCheck(glGenRenderbuffers(GLsizei(total), buffers));
}

void GlContext::deleteRenderbuffers(uint32_t total, const uint32_t* buffers)
{
  glCheck(glDeleteRenderbuffers(GLsizei(total), buffers));
}

void GlContext::bindRenderbuffer(uint32_t renderBufferId)
{
  glCheck(glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId));
}

void GlContext::renderbufferStorage(uint32_t width, uint32_t height)
{
  glCheck(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, GLsizei(width), GLsizei(height)));
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
  uint32_t program;
  glCheck(program = glCreateProgram());
  return program;
}

void GlContext::deleteProgram(uint32_t programId)
{
  glCheck(glDeleteProgram(programId));
}

bool GlContext::linkProgram(uint32_t programId, uint32_t vertexShader, uint32_t fragmentShader)
{
  glCheck(glAttachShader(programId, vertexShader));
  glCheck(glAttachShader(programId, fragmentShader));
  glCheck(glLinkProgram(programId));
  glCheck(glDeleteShader(vertexShader));
  glCheck(glDeleteShader(fragmentShader));

  int linkedStatus;
  glCheck(glGetProgramiv(programId, GL_LINK_STATUS, &linkedStatus));

  return (linkedStatus != GL_FALSE);
}

int GlContext::getAttribLocation(uint32_t programId, const char* name)
{
  int location;
  glCheck(location = glGetAttribLocation(programId, name));
  return location;
}

int GlContext::getUniformLocation(uint32_t programId, const char* name)
{
  int location;
  glCheck(location = glGetUniformLocation(programId, name));
  return location;
}

void GlContext::useProgram(uint32_t programId)
{
  glCheck(glUseProgram(programId));
}

void GlContext::setUniform(int location, int value)
{
  glCheck(glUniform1i(location, value));
}

void GlContext::setUniform(int location, int x, int y)
{
  glCheck(glUniform2i(location, x, y));
}

void GlContext::setUniform(int location, int x, int y, int z)
{
  glCheck(glUniform3i(location, x, y, z));
}

void GlContext::setUniform(int location, int x, int y, int z, int w)
{
  glCheck(glUniform4i(location, x, y, z, w));
}

void GlContext::setUniform(int location, float value)
{
  glCheck(glUniform1f(location, value));
}

void GlContext::setUniform(int location, float x, float y, float z)
{
  glCheck(glUniform3f(location, x, y, z));
}

void GlContext::setUniform(int location, float x, float y, float z, float w)
{
  glCheck(glUniform4f(location, x, y, z, w));
}

void GlContext::setUniform(int location, const glm::mat4& matrix)
{
  glCheck(glUniformMatrix4fv(location, 1, false, glm::value_ptr(matrix)));
}

//
//
//
//
//

void GlContext::genTextures(uint32_t total, uint32_t* buffers)
{
  glCheck(glGenTextures(GLsizei(total), buffers));
}

void GlContext::deleteTextures(uint32_t total, const uint32_t* buffers)
{
  glCheck(glDeleteTextures(GLsizei(total), buffers));
}

uint32_t GlContext::genTexture()
{
  uint32_t textureId;
  GlContext::genTextures(1, &textureId);
  return textureId;
}

void GlContext::deleteTexture(uint32_t textureId)
{
  GlContext::deleteTextures(1, &textureId);
}

void GlContext::activeTexture(uint32_t index)
{
  glCheck(glActiveTexture(GL_TEXTURE0 + index));
}


void GlContext::bindTexture(uint32_t textureId)
{
  glCheck(glBindTexture(GL_TEXTURE_2D, textureId));
}

void GlContext::uploadPixels(uint32_t width, uint32_t height, const void* pixels)
{
  GLint level = 0;
  GLint border = 0;
  glCheck(glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, GLsizei(width), GLsizei(height), border, GL_RGBA, GL_UNSIGNED_BYTE, pixels));
}

void GlContext::downloadPixels(uint32_t posX, uint32_t posY, uint32_t width, uint32_t height, void* pixels)
{
  glCheck(glReadPixels(GLint(posX), GLint(posY), GLsizei(width), GLsizei(height), GL_RGBA, GL_UNSIGNED_BYTE, pixels));
}

void GlContext::setAsDepthTexture(uint32_t width, uint32_t height)
{
  // glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, width, height);

  // // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GLsizei(width), GLsizei(height), border, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  // GLint level = 0;
  // GLint border = 0;
  // glTexImage2D(GL_TEXTURE_2D, level, GL_DEPTH_COMPONENT, GLsizei(width), GLsizei(height), border, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  // // glTexImage2D(GL_TEXTURE_2D, level, GL_DEPTH_COMPONENT16, GLsizei(width), GLsizei(height), border, GL_DEPTH_COMPONENT16, GL_FLOAT, 0);

  const int level = 0;
  const int internalFormat = GL_DEPTH_COMPONENT16;
  const int border = 0;
  const int format = GL_DEPTH_COMPONENT;

  // const int type = GL_UNSIGNED_BYTE; // does not work
  const int type = GL_UNSIGNED_SHORT; // works

  glCheck(glTexImage2D(
    GL_TEXTURE_2D,
    level, internalFormat,
    GLsizei(width), GLsizei(height), border,
    format, type, nullptr));

  GlContext::setTextureAsRepeat(false);
  GlContext::setTextureAsPixelated();

  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));
  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE));
  // glCheck(glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY));
}


void GlContext::setTextureAsRepeat(bool repeat)
{
  const int wrapValue = (repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapValue));
  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapValue));
}


void GlContext::setTextureAsPixelated()
{
  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
}

void GlContext::setTextureAsSmoothed(bool generateMipMap /* = false */)
{
  // glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
  // glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST));
  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

  if (generateMipMap)
    glCheck(glGenerateMipmap(GL_TEXTURE_2D));
}

//
//
//
//
//

void GlContext::genVertexArrays(uint32_t total, uint32_t* buffers)
{
  glCheck(glGenVertexArrays(GLsizei(total), buffers));
}

void GlContext::deleteVertexArrays(uint32_t total, const uint32_t* buffers)
{
  glCheck(glDeleteVertexArrays(GLsizei(total), buffers));
}

void GlContext::bindVertexArray(uint32_t vaoId)
{
  glCheck(glBindVertexArray(vaoId));
}

//
//
//
//
//

void GlContext::genBuffers(uint32_t total, uint32_t* buffers)
{
  glCheck(glGenBuffers(GLsizei(total), buffers));
}

void GlContext::deleteBuffers(uint32_t total, const uint32_t* buffers)
{
  glCheck(glDeleteBuffers(GLsizei(total), buffers));
}

void GlContext::bindBuffer(uint32_t vboId)
{
  glCheck(glBindBuffer(GL_ARRAY_BUFFER, vboId));
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
  glCheck(glViewport(x, y, GLsizei(width), GLsizei(height)));
}

void GlContext::setScissor(int x, int y, uint32_t width, uint32_t height)
{
  glCheck(glScissor(x, y, GLsizei(width), GLsizei(height)));
}

void GlContext::setDepthFunc(DepthFuncs func)
{
  switch (func)
  {
  case DepthFuncs::never: glCheck(glDepthFunc(GL_NEVER)); break;
  case DepthFuncs::less: glCheck(glDepthFunc(GL_LESS)); break;
  case DepthFuncs::equal: glCheck(glDepthFunc(GL_EQUAL)); break;
  case DepthFuncs::lessAndEqual: glCheck(glDepthFunc(GL_LEQUAL)); break;
  case DepthFuncs::greater: glCheck(glDepthFunc(GL_GREATER)); break;
  case DepthFuncs::notEqual: glCheck(glDepthFunc(GL_NOTEQUAL)); break;
  case DepthFuncs::greaterAndEqual: glCheck(glDepthFunc(GL_GEQUAL)); break;
  case DepthFuncs::always: glCheck(glDepthFunc(GL_ALWAYS)); break;

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
  glCheck(glBlendFunc(getRawBlendFunc(sfactor), getRawBlendFunc(dfactor)));
}

void GlContext::enable(States state)
{
  switch (state)
  {
  case States::cullFace: glCheck(glEnable(GL_CULL_FACE)); break;
  case States::depthTest: glCheck(glEnable(GL_DEPTH_TEST)); break;
  case States::blend: glCheck(glEnable(GL_BLEND)); break;
  case States::scissorTest: glCheck(glEnable(GL_SCISSOR_TEST)); break;

  default:
    D_THROW(std::runtime_error, "unknown state");
    break;
  }
}

void GlContext::disable(States state)
{
  switch (state)
  {
  case States::cullFace: glCheck(glDisable(GL_CULL_FACE)); break;
  case States::depthTest: glCheck(glDisable(GL_DEPTH_TEST)); break;
  case States::blend: glCheck(glDisable(GL_BLEND)); break;
  case States::scissorTest: glCheck(glDisable(GL_SCISSOR_TEST)); break;

  default:
    D_THROW(std::runtime_error, "unknown state");
    break;
  }
}

void GlContext::setBackFaceCullingDirection(BackFaceCullingDirection direction)
{
  switch (direction)
  {
  case BackFaceCullingDirection::clockWise : glCheck(glFrontFace(GL_CW)); break;
  case BackFaceCullingDirection::counterClockWise : glCheck(glFrontFace(GL_CCW)); break;

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

  glCheck(glClear(GLbitfield(rawMask)));
}

void GlContext::clearColor(float r, float g, float b, float a)
{
  glCheck(glClearColor(r, g, b, a));
}

void GlContext::clearDepth(float value)
{
  glCheck(glClearDepthf(value));
}


// void GlContext::polygonModeAsLine()
// {
//   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
// }

// void GlContext::polygonModeAsFill()
// {
//   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
// }
