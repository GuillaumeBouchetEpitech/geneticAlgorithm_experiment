
#include "Texture.hpp"

#include "Image.hpp"
#include "FrameBuffer.hpp"

#include "GlContext.hpp"

#include "framework/ErrorHandler.hpp"

Texture::~Texture()
{
  dispose();
}

//

void Texture::setFromImage(
  const Image& image,
  bool pixelated /*= false*/,
  bool repeat  /*= false*/)
{
  allocateBlank(image.getSize(), pixelated, repeat, image.getPixels());
}

void Texture::allocateBlank(
  const glm::uvec2& size,
  bool pixelated /*= false*/,
  bool repeat /*= false*/,
  const void* pixels /*= nullptr*/)
{
  if (size.x == 0 || size.y == 0)
    D_THROW(std::runtime_error, "texture allocated with incorrect size, size.x: " << size.x << ", size.y: " << size.y);

  // TODO: check max texture size
  // if (_size.x < 1 || _size.y < 1)
  //   D_THROW(std::runtime_error, "image allocated with incorrect size");

  _size = size;

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

void Texture::allocateDepth(const glm::uvec2& size)
{
  _size = size;

  // TODO: check max texture size
  // if (_size.x < 1 || _size.y < 1)
  //   D_THROW(std::runtime_error, "image allocated with incorrect size");

  if (_textureId == 0)
    _textureId = GlContext::genTexture();

  GlContext::bindTexture(_textureId);
  GlContext::setAsDepthTexture(_size.x, _size.y);
  GlContext::bindTexture(0);
}

void Texture::dispose()
{
  if (!isValid())
    return;

  _size.x = 0;
  _size.y = 0;
  GlContext::deleteTexture(_textureId);
  _textureId = 0;
}

void Texture::getAsImage(Image& image)
{
  if (!isValid())
    D_THROW(std::runtime_error, "texture not valid");

  image.dispose();

  FrameBuffer::Definition def;
  def.colorTextures.push_back({ 0, this });

  FrameBuffer tmpFrameBuffer;
  tmpFrameBuffer.initialise(def);
  tmpFrameBuffer.getAsImage(image, 0, 0, _size.x, _size.y);
}

const glm::uvec2& Texture::getSize() const
{
  return _size;
}

bool Texture::isValid() const
{
  return _size.x > 0 && _size.x > 0 && _textureId != 0;
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
