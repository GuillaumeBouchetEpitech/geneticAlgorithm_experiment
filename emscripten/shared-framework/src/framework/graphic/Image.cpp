
#include "Image.hpp"

#include "framework/ErrorHandler.hpp"

#include "framework/helpers/internals/STBImage.hpp"

#include <filesystem> // C++17
namespace fs = std::filesystem;

//

Image::~Image() { dispose(); }

//

void Image::loadFromFile(const std::string& filename,
                         bool supportNonPowerOfTwo /*= true*/) {
  loadFromFile(FileUtils::getDefaulCallback(), filename, supportNonPowerOfTwo);
}

void Image::loadFromFile(FileManager& fileManager, const std::string& filename,
                         bool supportNonPowerOfTwo /*= true*/) {
  loadFromFile(FileUtils::getFileManagerCallback(fileManager), filename,
               supportNonPowerOfTwo);
}

void Image::loadFromFile(const FileUtils::LoadCallback& loadFileCallback,
                         const std::string& filename,
                         bool supportNonPowerOfTwo /*= true*/) {
  std::string fileContent;
  loadFileCallback(filename, fileContent);

  loadFromMemory(fileContent, supportNonPowerOfTwo);
}

void Image::loadFromMemory(const std::string& content,
                           bool supportNonPowerOfTwo /*= true*/) {
  dispose();

  int width;
  int height;
  int bpp;
  _stbPixels = stbi_load_from_memory(
    reinterpret_cast<const unsigned char*>(content.c_str()),
    int(content.size()), &width, &height, &bpp, 0);

  if (!_stbPixels)
    D_THROW(std::runtime_error, "image not found");
  if (bpp != 4)
    D_THROW(std::runtime_error, "image format not supported, bpp: " << bpp);

  if (supportNonPowerOfTwo == false) {
    if ((width & (width - 1)) != 0)
      D_THROW(std::runtime_error, "image width not a power of 2");

    if ((height & (height - 1)) != 0)
      D_THROW(std::runtime_error, "image height not a power of 2");
  }

  _rawPixels = _stbPixels;
  _size.x = uint32_t(width);
  _size.y = uint32_t(height);
}

void Image::dispose() {
  if (!isValid())
    return;

  _size.x = 0;
  _size.y = 0;

  if (_stbPixels) {
    stbi_image_free(_stbPixels);
    _stbPixels = nullptr;
    _rawPixels = nullptr;
  } else if (_rawPixels) {
    delete[] _rawPixels, _rawPixels = nullptr;
  }
}

bool Image::save(const std::string& filename) {
  if (!isValid())
    D_THROW(std::runtime_error,
            "image not initialised, filename: " << filename);

  return Image::save(filename, _size.x, _size.y, _rawPixels);
}

bool Image::save(const std::string& filename, uint32_t width, uint32_t height,
                 const uint8_t* pixels) {
  fs::path filePath = filename;
  if (filePath.extension() == ".png")
    return stbi_write_png(filename.c_str(), int(width), int(height), 4, pixels,
                          0);
  if (filePath.extension() == ".jpg")
    return stbi_write_jpg(filename.c_str(), int(width), int(height), 4, pixels,
                          0);
  if (filePath.extension() == ".bmp")
    return stbi_write_bmp(filename.c_str(), int(width), int(height), 4, pixels);

  D_THROW(std::runtime_error, "unknown file extension, filename: " << filename);
}

void Image::flipY() {
  if (!isValid())
    D_THROW(std::runtime_error, "image not initialised");

  const uint32_t hsize = _size.y / 2;
  for (uint32_t yy = 0; yy < hsize; ++yy)
    for (uint32_t xx = 0; xx < _size.x; ++xx)
      std::swap(_rawPixels[yy * _size.x + xx],
                _rawPixels[(_size.y - yy) * _size.x + xx]);
}

//

const glm::uvec2& Image::getSize() const { return _size; }
const uint8_t* Image::getPixels() const { return _rawPixels; }

bool Image::isValid() const {
  return _size.x > 0 && _size.y > 0 && _rawPixels != nullptr;
}
