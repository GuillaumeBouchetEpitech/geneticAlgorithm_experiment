
#include "Image.hpp"

#include "framework/helpers/STBImage.hpp"

#include "framework/ErrorHandler.hpp"

//

Image::~Image()
{
  unload();
}

//

void Image::load(const std::string& filename, bool supportNonPowerOfTow /* = true */)
{
  unload();

  int width;
  int height;
  int bpp;
  _pixels = stbi_load(filename.c_str(), &width, &height, &bpp, 0);

  if (!_pixels)
    D_THROW(std::runtime_error, "image not found, filename=\"" << filename << "\"");

  if (supportNonPowerOfTow == false)
  {
    if ((width & (width - 1)) != 0)
      D_THROW(std::runtime_error, "image width not a power of 2, filename=\"" << filename << "\"");

    if ((height & (height - 1)) != 0)
      D_THROW(std::runtime_error, "image height not a power of 2, filename=\"" << filename << "\"");
  }

  _size.x = width;
  _size.y = height;
}

void Image::unload()
{
  if (_pixels)
    stbi_image_free(_pixels);

  _size.x = 0;
  _size.y = 0;
}

void Image::flipY()
{
  const int hsize = _size.y / 2;
  for (int yy = 0; yy < hsize; ++yy)
  for (int xx = 0; xx < _size.x; ++xx)
    std::swap(_pixels[yy * _size.x + xx], _pixels[(_size.y - yy) * _size.x + xx]);
}

//

const glm::ivec2& Image::getSize() const
{
  return _size;
}

const unsigned char* Image::getPixels() const
{
  return _pixels;
}
