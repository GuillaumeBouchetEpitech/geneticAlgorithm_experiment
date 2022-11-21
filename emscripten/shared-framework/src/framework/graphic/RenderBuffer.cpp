
#include "RenderBuffer.hpp"

#include "GlContext.hpp"

#include "framework/ErrorHandler.hpp"

RenderBuffer::~RenderBuffer()
{
  if (_bufferId != 0)
    GlContext::deleteRenderbuffers(1, &_bufferId);
}

//

void RenderBuffer::allocateDepth(const glm::ivec2& size)
{
  _size = size;

  if (_size.x < 1 || _size.y < 1)
    D_THROW(std::runtime_error, "image allocated with incorrect size");

  if (_bufferId == 0)
    GlContext::genRenderbuffers(1, &_bufferId);

  GlContext::bindRenderbuffer(_bufferId);
  GlContext::renderbufferStorage(uint32_t(_size.x), uint32_t(_size.y));
  GlContext::bindRenderbuffer(0);
}

const glm::ivec2& RenderBuffer::getSize() const
{
  return _size;
}

//

void RenderBuffer::bind() const
{
  if (_bufferId == 0)
    D_THROW(std::runtime_error, "not allocated");

  GlContext::bindRenderbuffer(_bufferId);
}

void RenderBuffer::unbind()
{
  GlContext::bindRenderbuffer(0);
}
