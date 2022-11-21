
#include "VertexBufferObject.hpp"

#include "../GlContext.hpp"

#include "framework/ErrorHandler.hpp"

#include <stdexcept> // <= std::invalid_argument / runtime_error / out_of_range
#include <cstdint>

VertexBufferObject::~VertexBufferObject()
{
  if (!_ids.empty())
    deallocate();
}

void VertexBufferObject::allocate(std::size_t size /* = 1 */)
{
  if (size == 0)
    D_THROW(std::invalid_argument, "invalid size, input=" << size);

  if (!_ids.empty())
    deallocate();

  _ids.resize(size);
  GlContext::genBuffers(uint32_t(_ids.size()), _ids.data());
}

void VertexBufferObject::deallocate()
{
  if (_ids.empty())
    D_THROW(std::runtime_error, "not allocated");

  GlContext::deleteBuffers(uint32_t(_ids.size()), _ids.data());
  _ids.clear();
}

bool VertexBufferObject::isAllocated() const
{
  return !_ids.empty();
}

void VertexBufferObject::bind(unsigned int index /* = 0 */) const
{
  if (_ids.empty())
    D_THROW(std::runtime_error, "not allocated");

  if (index >= uint32_t(_ids.size()))
    D_THROW(std::out_of_range, "index out of range, input=" << index << ", max=" << _ids.size());

  GlContext::bindBuffer(_ids[index]);
}

void VertexBufferObject::unbind()
{
  GlContext::bindBuffer(0);
}
