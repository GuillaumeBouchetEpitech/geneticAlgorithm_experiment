
#include "../GlContext.hpp"

#include "glCheckError.hpp"

#include "framework/helpers/internals/OpenGLES.hpp"
#include "framework/system/ErrorHandler.hpp"
#include "framework/system/TraceLogger.hpp"
#include "framework/system/asValue.hpp"

#include <memory>

namespace GlContext {

namespace Texture {

void generateMany(uint32_t total, uint32_t* buffers) {
  glCheck(glGenTextures(GLsizei(total), buffers));
}

void deleteMany(uint32_t total, const uint32_t* buffers) {
  glCheck(glDeleteTextures(GLsizei(total), buffers));
}

uint32_t generateOne() {
  uint32_t textureId;
  generateMany(1, &textureId);
  return textureId;
}

void deleteOne(uint32_t textureId) { deleteMany(1, &textureId); }

void active(uint32_t index) { glCheck(glActiveTexture(GL_TEXTURE0 + index)); }

void bind(uint32_t textureId) {
  glCheck(glBindTexture(GL_TEXTURE_2D, textureId));
}

void uploadPixels(uint32_t width, uint32_t height, const void* pixels) {
  GLint level = 0;
  GLint border = 0;
  glCheck(glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, GLsizei(width),
                       GLsizei(height), border, GL_RGBA, GL_UNSIGNED_BYTE,
                       pixels));
}

void uploadSingleFloatPixels(uint32_t width, uint32_t height) {
  GLint level = 0;
  GLint border = 0;
  glCheck(glTexImage2D(GL_TEXTURE_2D, level, GL_R32F, GLsizei(width),
                       GLsizei(height), border, GL_RED, GL_FLOAT, nullptr));
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
int getRawDepthType(DepthType depthType) {
  switch (depthType) {
  case DepthType::unsingedShort:
    return GL_UNSIGNED_SHORT;
    break;
  case DepthType::unsingedInt:
    return GL_UNSIGNED_INT;
    break;
  case DepthType::float32:
    return GL_FLOAT;
    break;
  default:
    return GL_UNSIGNED_SHORT;
    break;
  }
}
} // namespace

void setAsDepthTexture(uint32_t width, uint32_t height, DepthFormat depthFormat,
                       DepthType depthType) {
  // glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, width, height);

  // // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GLsizei(width), GLsizei(height),
  // border, GL_RGBA, GL_UNSIGNED_BYTE, pixels); GLint level = 0; GLint border =
  // 0; glTexImage2D(GL_TEXTURE_2D, level, GL_DEPTH_COMPONENT, GLsizei(width),
  // GLsizei(height), border, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  // // glTexImage2D(GL_TEXTURE_2D, level, GL_DEPTH_COMPONENT16, GLsizei(width),
  // GLsizei(height), border, GL_DEPTH_COMPONENT16, GL_FLOAT, 0);

  const int level = 0;
  const int border = 0;
  const int format = GL_DEPTH_COMPONENT;

  const int internalFormat = getRawDepthFormat(depthFormat);
  const int type = getRawDepthType(depthType);

  setTextureAsRepeat(false);
  setTextureAsPixelated();

  glCheck(glTexImage2D(GL_TEXTURE_2D, level, internalFormat, GLsizei(width),
                       GLsizei(height), border, format, GLenum(type), nullptr));

  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));
  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE));
  // glCheck(glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE,
  // GL_INTENSITY));
}

void setTextureAsRepeat(bool repeat) {
  const int wrapValue = (repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapValue));
  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapValue));
}

void setTextureAsPixelated() {
  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
}

void setTextureAsSmoothed(bool generateMipMap /* = false */) {
  // glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
  // GL_NEAREST_MIPMAP_LINEAR)); glCheck(glTexParameteri(GL_TEXTURE_2D,
  // GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST));
  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

  if (generateMipMap)
    glCheck(glGenerateMipmap(GL_TEXTURE_2D));
}

} // namespace Texture

} // namespace GlContext
