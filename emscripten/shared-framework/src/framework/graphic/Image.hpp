
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <string>

class Image
{
private:
  glm::ivec2  _size = { 0, 0 };
  unsigned char* _pixels = nullptr;

public:
  Image() = default;
  ~Image();

public:
  void load(const std::string& filename, bool supportNonPowerOfTow = true);
  void unload();

public:
  void flipY();

public:
  const glm::ivec2& getSize() const;
  const unsigned char* getPixels() const;

};
