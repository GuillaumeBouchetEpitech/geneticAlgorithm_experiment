
#include "Texture.hpp"

#include <SDL2/SDL_image.h> // <= IMG_Load

#include "demo/utilities/ErrorHandler.hpp"

Texture::~Texture()
{
	glDeleteTextures(1, &_textureId);
}

//

bool	Texture::load(const std::string& filename, bool pixelated /*= false*/, bool repeat /*= false*/)
{
    // TODO: make an "image" class?

	SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface)
        D_THROW(std::runtime_error, "fail to load image, filename=\"" << filename << "\"");

    _size = { surface->w, surface->h };

    if ((surface->w & (surface->w - 1)) != 0)
        D_THROW(std::runtime_error, "image width not a power of 2, filename=\"" << filename << "\"");

    if ((surface->h & (surface->h - 1)) != 0)
        D_THROW(std::runtime_error, "image height not a power of 2, filename=\"" << filename << "\"");

    glGenTextures(1, &_textureId);

    glBindTexture(GL_TEXTURE_2D, _textureId);

    int filterValue = (pixelated ? GL_NEAREST : GL_LINEAR);
    int wrapValue = (repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterValue);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterValue);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapValue);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapValue);

    GLint level = 0;
    GLint border = 0;
    glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, surface->w, surface->h, border, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glBindTexture(GL_TEXTURE_2D, 0);

	SDL_FreeSurface(surface);

    return true;
}

const glm::ivec2&	Texture::getSize() const
{
    return _size;
}

//

void	Texture::bind() const
{
    if (_textureId == 0)
        D_THROW(std::runtime_error, "not allocated");

    glBindTexture(GL_TEXTURE_2D, _textureId);
}

void	Texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
