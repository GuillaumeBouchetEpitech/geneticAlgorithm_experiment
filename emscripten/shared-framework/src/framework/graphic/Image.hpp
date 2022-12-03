
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <string>

class Image
{
private:
  glm::uvec2  _size = { 0, 0 };
  unsigned char* _pixels = nullptr;

public:
  Image() = default;
  ~Image();

public:
  void load(const std::string& filename, bool supportNonPowerOfTow = true);
  void dispose();

public:
  bool save(const std::string& filename);

  static bool save(
    const std::string& filename,
    uint32_t width,
    uint32_t height,
    const uint8_t* pixels);

public:
  void flipY();

public:
  const glm::uvec2& getSize() const;
  const unsigned char* getPixels() const;
  bool isValid() const;

};
