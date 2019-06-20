
#include "VertexBufferObject.hpp"

#include "demo/utilities/ErrorHandler.hpp"

#include <stdexcept> // <= std::invalid_argument / runtime_error / out_of_range

VertexBufferObject::~VertexBufferObject()
{
	if (!_ids.empty())
		deallocate();
}

void	VertexBufferObject::allocate(unsigned int size /* = 1 */)
{
	if (size == 0)
    	D_THROW(std::invalid_argument, "invalid size, input=" << size);

	if (!_ids.empty())
		deallocate();

	_ids.resize(size);
	glGenBuffers(_ids.size(), _ids.data());
}

void	VertexBufferObject::deallocate()
{
	if (_ids.empty())
    	D_THROW(std::runtime_error, "not allocated");

	glDeleteBuffers(_ids.size(), _ids.data());
	_ids.clear();
}

bool	VertexBufferObject::isAllocated() const
{
	return !_ids.empty();
}

void	VertexBufferObject::bind(unsigned int index /* = 0 */) const
{
	if (_ids.empty())
    	D_THROW(std::runtime_error, "not allocated");

	if (index >= _ids.size())
    	D_THROW(std::out_of_range, "index out of range, input=" << index << ", max=" << _ids.size());

	glBindBuffer(GL_ARRAY_BUFFER, _ids.at(index));
}

void	VertexBufferObject::unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
