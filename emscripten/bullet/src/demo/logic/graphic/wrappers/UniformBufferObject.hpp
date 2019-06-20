
#pragma once

#include "thirdparty/OpenGLES.hpp"

#include <vector>
#include <string>

class Shader;

class UniformBufferObject
{
private:
	static GLint _sOffsetAlignment;

private:
	std::vector<GLuint>	_uboIds;
	std::vector<int>	_uboBoundLayouts;

public:
	~UniformBufferObject();

public:
	void	allocate(unsigned int size = 1);
	void	deallocate();

public:
	bool	isAllocated() const;

public:
	void	bind(unsigned int index = 0) const;

public:
	void	setBoundLayout(unsigned int index, int binding);

public:
	static void	unbind();

public:
    void    updateBuffer(unsigned int index, const void* data,
						 int dataSize, bool dynamic = false) const;

public:
    void    bindBase(unsigned int index) const;
    void    bindRange(unsigned int index,
					  unsigned int offset, unsigned int size) const;

public:
	static GLint getOffsetAlignment();

};
