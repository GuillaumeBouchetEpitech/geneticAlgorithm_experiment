
#include "Texture.hpp"

#include "helpers/STBImage.hpp"

#include "demo/utilities/ErrorHandler.hpp"

Texture::~Texture()
{
    if (_textureId != 0)
        glDeleteTextures(1, &_textureId);
}

//

void Texture::load(const std::string& filename, bool pixelated /*= false*/, bool repeat /*= false*/)
{
    // TODO: make an "image" class?

    int width;
    int height;
    int bpp;
    unsigned char* pixels = stbi_load(filename.c_str(), &width, &height, &bpp, 0);

    if ((width & (width - 1)) != 0)
        D_THROW(std::runtime_error, "image width not a power of 2, filename=\"" << filename << "\"");

    if ((height & (height - 1)) != 0)
        D_THROW(std::runtime_error, "image height not a power of 2, filename=\"" << filename << "\"");

    allocateBlank({ width, height }, pixelated, repeat, pixels);

    stbi_image_free(pixels);
}

void Texture::allocateBlank(const glm::ivec2& size, bool pixelated /*= false*/,
                            bool repeat /*= false*/, const GLvoid* pixels /*= nullptr*/)
{
    _size = size;

    if (_size.x < 1 || _size.y < 1)
        D_THROW(std::runtime_error, "image allocated with incorrect size");

    if (_textureId == 0)
        glGenTextures(1, &_textureId);

    glBindTexture(GL_TEXTURE_2D, _textureId);

    GLint level = 0;
    GLint border = 0;
    glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, _size.x, _size.y, border, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    const int wrapValue = (repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapValue);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapValue);

    if (pixelated)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);

        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

const glm::ivec2& Texture::getSize() const
{
    return _size;
}

//

void Texture::bind() const
{
    if (_textureId == 0)
        D_THROW(std::runtime_error, "not allocated");

    glBindTexture(GL_TEXTURE_2D, _textureId);
}

void Texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
