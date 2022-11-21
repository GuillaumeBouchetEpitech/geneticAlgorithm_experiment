
#include "FrameBuffer.hpp"

#include "framework/ErrorHandler.hpp"

#include "GlContext.hpp"

FrameBuffer::~FrameBuffer()
{
  GlContext::deleteFramebuffer(_frameBufferId);
}

//

void FrameBuffer::initialise()
{
  if (_frameBufferId == 0)
    _frameBufferId = GlContext::genFramebuffer();
}

void FrameBuffer::attachColorTexture(Texture& texture)
{
  GlContext::framebufferTexture2D(texture._textureId);
}

void FrameBuffer::attachDepthRenderBuffer(RenderBuffer& buffer)
{
  GlContext::framebufferRenderbuffer(buffer._bufferId);
}

//

void FrameBuffer::executeCheck() const
{
  GlContext::checkFrameBuffer();
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
