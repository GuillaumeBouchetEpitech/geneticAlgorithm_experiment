
#pragma once

#include "thirdparty/OpenGLES.hpp"
#include "thirdparty/GLMath.hpp"

#include <string>

class Texture
{
private:
	GLuint		_textureId = 0;
	glm::ivec2	_size = { 0, 0 };

public:
	Texture() = default;
	~Texture();
	
public:
	bool	load(const std::string& filename, bool pixelated = false, bool repeat = false);

public:
	const glm::ivec2&	getSize() const;

public:
	void	bind() const;

public:
	static void	unbind();

};
