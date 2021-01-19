
#include "RenderBuffer.hpp"

#include "demo/utilities/ErrorHandler.hpp"

RenderBuffer::~RenderBuffer()
{
    if (_bufferId != 0)
        glDeleteRenderbuffers(1, &_bufferId);
}

//

void RenderBuffer::allocateDepth(const glm::ivec2& size)
{
    _size = size;

    if (_size.x < 1 || _size.y < 1)
        D_THROW(std::runtime_error, "image allocated with incorrect size");

    if (_bufferId == 0)
        glGenRenderbuffers(1, &_bufferId);

    glBindRenderbuffer(GL_RENDERBUFFER, _bufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, _size.x, _size.y);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
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

    glBindRenderbuffer(GL_RENDERBUFFER, _bufferId);
}

void RenderBuffer::unbind()
{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
