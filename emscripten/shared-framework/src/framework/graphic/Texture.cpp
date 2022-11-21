
#include "Texture.hpp"

#include "Image.hpp"

#include "GlContext.hpp"

#include "framework/ErrorHandler.hpp"

Texture::~Texture()
{
  if (_textureId != 0)
    GlContext::deleteTexture(_textureId);
}

//

void Texture::setFromImage(const Image& image, bool pixelated /*= false*/, bool repeat  /*= false*/)
{
  allocateBlank(image.getSize(), pixelated, repeat, image.getPixels());
}

void Texture::allocateBlank(
  const glm::ivec2& size,
  bool pixelated /*= false*/,
  bool repeat /*= false*/,
  const void* pixels /*= nullptr*/)
{
  _size = size;

  if (_size.x < 1 || _size.y < 1)
    D_THROW(std::runtime_error, "image allocated with incorrect size");

  if (_textureId == 0)
    _textureId = GlContext::genTexture();

  GlContext::bindTexture(_textureId);

  GlContext::uploadPixels(uint32_t(_size.x), uint32_t(_size.y), pixels);

  GlContext::setTextureAsRepeat(repeat);

  if (pixelated)
  {
    GlContext::setTextureAsPixelated();
  }
  else
  {
    GlContext::setTextureAsSmoothed(true);
  }

  GlContext::bindTexture(0);
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

  GlContext::bindTexture(_textureId);
}

void Texture::unbind()
{
  GlContext::bindTexture(0);
}
