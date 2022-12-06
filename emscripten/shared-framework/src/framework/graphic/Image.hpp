
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <string>

class FrameBuffer;

class Image {
  friend FrameBuffer;

private:
  glm::uvec2 _size = {0, 0};
  uint8_t* _stbPixels = nullptr;
  uint8_t* _rawPixels = nullptr;

public:
  Image() = default;
  ~Image();

public:
  void load(const std::string& filename, bool supportNonPowerOfTow = true);
  void dispose();

public:
  bool save(const std::string& filename);

  static bool save(const std::string& filename, uint32_t width, uint32_t height,
                   const uint8_t* pixels);

public:
  void flipY();

public:
  const glm::uvec2& getSize() const;
  const uint8_t* getPixels() const;
  bool isValid() const;
};
