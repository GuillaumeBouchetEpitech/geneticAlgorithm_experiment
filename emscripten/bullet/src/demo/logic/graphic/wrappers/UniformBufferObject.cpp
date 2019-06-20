
#include "UniformBufferObject.hpp"

#include "Shader.hpp"

#include "demo/utilities/ErrorHandler.hpp"

#include <stdexcept> // <= std::invalid_argument / runtime_error / out_of_range

GLint UniformBufferObject::_sOffsetAlignment = -1;

UniformBufferObject::~UniformBufferObject()
{
    if (!_uboIds.empty())
        deallocate();
}

void UniformBufferObject::allocate(unsigned int size /* = 1 */)
{
    if (size < 1)
        D_THROW(std::invalid_argument, "invalid size, input=" << size);

    if (!_uboIds.empty())
        deallocate();

    _uboIds.resize(size);
    glGenBuffers(_uboIds.size(), _uboIds.data());

    _uboBoundLayouts.assign(_uboIds.size(), -1);
}

void UniformBufferObject::deallocate()
{
    if (_uboIds.empty())
        D_THROW(std::runtime_error, "not allocated");

    glDeleteBuffers(_uboIds.size(), _uboIds.data());
    _uboIds.clear();
    _uboBoundLayouts.clear();
}

void UniformBufferObject::bind(unsigned int index /* = 0 */) const
{
    if (_uboIds.empty())
        D_THROW(std::runtime_error, "not allocated");

    if (index >= _uboIds.size())
        D_THROW(std::out_of_range, "index out of range, input=" << index << ", max=" << _uboIds.size());

    glBindBuffer(GL_UNIFORM_BUFFER, _uboIds[index]);
}

void	UniformBufferObject::setBoundLayout(unsigned int index, int binding)
{
    if (_uboBoundLayouts.empty())
        D_THROW(std::runtime_error, "not allocated");

    if (index >= _uboBoundLayouts.size())
        D_THROW(std::out_of_range, "index out of range, input=" << index << ", max=" << _uboBoundLayouts.size());

    _uboBoundLayouts[index] = binding;
}

void UniformBufferObject::unbind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

bool	UniformBufferObject::isAllocated() const
{
    return !_uboIds.empty();
}

void    UniformBufferObject::updateBuffer(unsigned int index, const void* data, int dataSize, bool dynamic /*= false*/) const
{
    if (!isAllocated())
        D_THROW(std::runtime_error, "not allocated");

    this->bind(index);

    GLenum usage = (dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

    glBufferData(GL_UNIFORM_BUFFER, dataSize, data, usage);

    UniformBufferObject::unbind();
}

void    UniformBufferObject::bindBase(unsigned int index) const
{
    if (_uboIds.empty())
        D_THROW(std::runtime_error, "not allocated");

    if (index >= _uboIds.size())
        D_THROW(std::out_of_range, "index out of range, input=" << index << ", max=" << _uboIds.size());

    int bindingIndex = _uboBoundLayouts[index];
    if (bindingIndex < 0)
        D_THROW(std::out_of_range, "invalid binding value for index, input=" << index);

    glBindBufferBase(GL_UNIFORM_BUFFER, bindingIndex, _uboIds[index]);
}

void    UniformBufferObject::bindRange(unsigned int index,
                                       unsigned int offset,
                                       unsigned int size) const
{
    if (_uboIds.empty())
        D_THROW(std::runtime_error, "not allocated");

    if (index >= _uboIds.size())
        D_THROW(std::out_of_range, "index out of range, input=" << index << ", max=" << _uboIds.size());

    int bindingIndex = _uboBoundLayouts[index];
    if (bindingIndex < 0)
        D_THROW(std::out_of_range, "invalid binding value for index, input=" << index);

    glBindBufferRange(GL_UNIFORM_BUFFER, bindingIndex, _uboIds[index], offset, size);
}

GLint   UniformBufferObject::getOffsetAlignment()
{
    if (_sOffsetAlignment == -1)
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &_sOffsetAlignment);

    return _sOffsetAlignment;
}
