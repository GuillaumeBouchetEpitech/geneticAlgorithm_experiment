
#pragma once

#include "thirdparty/OpenGLES.hpp"

#include <vector>

class VertexArrayObject
{
private:
	std::vector<GLuint> _vaoIds;

public:
	~VertexArrayObject();

public:
	void	allocate(unsigned int size = 1);
	void	deallocate();

public:
	bool	isAllocated() const;

public:
	void	bind(unsigned int index = 0) const;

public:
	static void	unbind();
};
