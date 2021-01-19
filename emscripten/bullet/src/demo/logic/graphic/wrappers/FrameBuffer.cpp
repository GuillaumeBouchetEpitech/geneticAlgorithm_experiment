
#include "FrameBuffer.hpp"

#include "demo/utilities/ErrorHandler.hpp"

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &_frameBufferId);
}

//

void FrameBuffer::initialise()
{
    if (_frameBufferId == 0)
        glGenFramebuffers(1, &_frameBufferId);
}

void FrameBuffer::attachColorTexture(Texture& texture)
{
    GLint level = 0;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture._textureId, level);
}

void FrameBuffer::attachDepthRenderBuffer(RenderBuffer& buffer)
{
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer._bufferId);
}

//

void FrameBuffer::executeCheck() const
{
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

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

//

void FrameBuffer::bind() const
{
    if (_frameBufferId == 0)
        D_THROW(std::runtime_error, "not allocated");

    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferId);
}

void FrameBuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
