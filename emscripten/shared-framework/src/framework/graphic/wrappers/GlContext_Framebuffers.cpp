
#include "../GlContext.hpp"

#include "glCheckError.hpp"

#include "framework/helpers/internals/OpenGLES.hpp"
#include "framework/system/ErrorHandler.hpp"
#include "framework/system/TraceLogger.hpp"
#include "framework/system/asValue.hpp"

#include <memory>

namespace GlContext {

namespace FrameBuffers {

void generateMany(uint32_t total, uint32_t* buffers) {
  glCheck(glGenFramebuffers(GLsizei(total), buffers));
}

void deleteMany(uint32_t total, const uint32_t* buffers) {
  glCheck(glDeleteFramebuffers(GLsizei(total), buffers));
}

uint32_t generateOne() {
  uint32_t bufferId;
  generateMany(1, &bufferId);
  return bufferId;
}

void deleteOne(uint32_t bufferId) { deleteMany(1, &bufferId); }

uint32_t getColorAttachment(uint32_t index) {
  return GL_COLOR_ATTACHMENT0 + index;
}

void attachTexture2D(uint32_t index, uint32_t textureId) {
  constexpr int level = 0;
  glCheck(glFramebufferTexture2D(GL_FRAMEBUFFER, index, GL_TEXTURE_2D,
                                 textureId, level));
}

void attachDepthTexture2D(uint32_t textureId) {
  constexpr GLint level = 0;
  glCheck(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                 GL_TEXTURE_2D, textureId, level));
}

void attachRenderbuffer(uint32_t bufferId) {
  glCheck(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                    GL_RENDERBUFFER, bufferId));
}

bool check(bool throwException /*= true*/) {
  GLenum status = 0;
  glCheck(status = glCheckFramebufferStatus(GL_FRAMEBUFFER));

  if (status == GL_FRAMEBUFFER_UNDEFINED) {
    const char* msg = "incomplete frame buffer GL_FRAMEBUFFER_UNDEFINED";
    if (throwException)
      D_THROW(std::runtime_error, msg);
    // D_MYLOG(msg);
    return false;
  }
  if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
    const char* msg =
      "incomplete frame buffer GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
    if (throwException)
      D_THROW(std::runtime_error, msg);
    // D_MYLOG(msg);
    return false;
  }
  if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
    const char* msg =
      "incomplete frame buffer GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
    if (throwException)
      D_THROW(std::runtime_error, msg);
    // D_MYLOG(msg);
    return false;
  }
  if (status == GL_FRAMEBUFFER_UNSUPPORTED) {
    const char* msg = "incomplete frame buffer GL_FRAMEBUFFER_UNSUPPORTED";
    if (throwException)
      D_THROW(std::runtime_error, msg);
    // D_MYLOG(msg);
    return false;
  }
  if (status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) {
    const char* msg =
      "incomplete frame buffer GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
    if (throwException)
      D_THROW(std::runtime_error, msg);
    // D_MYLOG(msg);
    return false;
  }
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    const char* msg = "incomplete frame buffer";
    if (throwException)
      D_THROW(std::runtime_error, msg);
    // D_MYLOG(msg);
    return false;
  }

  return true;
}

void drawMany(uint32_t total, const uint32_t* buffers) {
  glCheck(glDrawBuffers(GLsizei(total), buffers));
}

void bind(uint32_t frameBufferId) {
  glCheck(glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId));
}

void downloadPixels(uint32_t posX, uint32_t posY, uint32_t width,
                    uint32_t height, void* pixels) {
  glCheck(glReadPixels(GLint(posX), GLint(posY), GLsizei(width),
                       GLsizei(height), GL_RGBA, GL_UNSIGNED_BYTE, pixels));
}

} // namespace FrameBuffers

} // namespace GlContext
