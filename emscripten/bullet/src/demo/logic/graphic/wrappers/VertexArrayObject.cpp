
#include "VertexArrayObject.hpp"

#include "demo/utilities/ErrorHandler.hpp"

#include <stdexcept> // <= std::invalid_argument / runtime_error / out_of_range

VertexArrayObject::~VertexArrayObject()
{
    if (!_vaoIds.empty())
        deallocate();
}

void VertexArrayObject::allocate(unsigned int size /* = 1 */)
{
    if (size < 1)
        D_THROW(std::invalid_argument, "invalid size, input=" << size);

    if (!_vaoIds.empty())
        deallocate();

    _vaoIds.resize(size);
    glGenVertexArrays(_vaoIds.size(), _vaoIds.data());
}

void VertexArrayObject::deallocate()
{
    if (_vaoIds.empty())
        D_THROW(std::runtime_error, "not allocated");

    glDeleteVertexArrays(_vaoIds.size(), _vaoIds.data());
    _vaoIds.clear();
}

void VertexArrayObject::bind(unsigned int index /* = 0 */) const
{
    if (_vaoIds.empty())
        D_THROW(std::runtime_error, "not allocated");

    if (index >= _vaoIds.size())
        D_THROW(std::out_of_range, "index out of range, input=" << index << ", max=" << _vaoIds.size());

    glBindVertexArray(_vaoIds[index]);
}

void VertexArrayObject::unbind()
{
    glBindVertexArray(0);
}

bool VertexArrayObject::isAllocated() const
{
    return !_vaoIds.empty();
}
