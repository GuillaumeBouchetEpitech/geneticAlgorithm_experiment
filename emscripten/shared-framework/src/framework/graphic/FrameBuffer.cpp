
#include "FrameBuffer.hpp"

#include "framework/ErrorHandler.hpp"

#include "GlContext.hpp"

FrameBuffer::~FrameBuffer()
{
  dispose();
}

//

void FrameBuffer::initialise(const Definition& def)
{
  if (def.colorTextures.empty())
    D_THROW(std::runtime_error, "empty frame buffer color texture array");
  for (const auto& colorTexture : def.colorTextures)
  {
    if (colorTexture.texture == nullptr)
      D_THROW(std::runtime_error, "null color texture in frame buffer color texture array");

    uint32_t totalSameIndex = 0;
    for (const auto& otherColorTexture : def.colorTextures)
      if (colorTexture.index == otherColorTexture.index)
        totalSameIndex += 1;
    if (totalSameIndex > 1)
      D_THROW(std::runtime_error, "duplicated color texture index in frame buffer color texture array");

    uint32_t totalSameTexture = 0;
    for (const auto& otherColorTexture : def.colorTextures)
      if (colorTexture.texture == otherColorTexture.texture)
        totalSameTexture += 1;
    if (totalSameTexture > 1)
      D_THROW(std::runtime_error, "duplicated color texture pointer in frame buffer color texture array");
  }
  if (def.depthTexture && def.renderBuffer)
    D_THROW(std::runtime_error, "cannot specify frame buffer depth texture and render buffer");
  // if (!def.depthTexture && !def.renderBuffer)
  //   D_THROW(std::runtime_error, "must specify frame buffer depth texture or render buffer");

  //
  //
  //

  if (_frameBufferId == 0)
    _frameBufferId = GlContext::genFramebuffer();

  bind();

  for (const auto& colorTexture : def.colorTextures)
    _attachColorTexture(colorTexture.index, *colorTexture.texture);

  if (def.depthTexture)
    _attachDepthTexture(*def.depthTexture);
  else if (def.renderBuffer)
    _attachDepthRenderBuffer(*def.renderBuffer);

  GlContext::checkFrameBuffer();

  // std::vector<uint32_t> allDrawBuffers;
  // allDrawBuffers.reserve(def.colorTextures.size());
  // for (const auto& colorTexture : def.colorTextures)
  //   allDrawBuffers.push_back(colorTexture.index);

  // GlContext::drawFrameBuffers(uint32_t(allDrawBuffers.size()), allDrawBuffers.data());

  FrameBuffer::unbind();
}

void FrameBuffer::dispose()
{
  if (_frameBufferId == 0)
    return;
  GlContext::deleteFramebuffer(_frameBufferId);
  _frameBufferId = 0;
}

void FrameBuffer::_attachColorTexture(uint32_t index, const Texture& colorTexture)
{
  colorTexture.bind();
  GlContext::framebufferTexture2D(index, colorTexture._textureId);
}

void FrameBuffer::_attachDepthTexture(const Texture& depthTexture)
{
  depthTexture.bind();
  GlContext::framebufferDepthTexture2D(depthTexture._textureId);
}

void FrameBuffer::_attachDepthRenderBuffer(const RenderBuffer& buffer)
{
  buffer.bind();
  GlContext::framebufferRenderbuffer(buffer._bufferId);
}

//

void FrameBuffer::bind() const
{
  if (_frameBufferId == 0)
    D_THROW(std::runtime_error, "not allocated");

  GlContext::bindFramebuffer(_frameBufferId);
}

void FrameBuffer::unbind()
{
  GlContext::bindFramebuffer(0);
}
