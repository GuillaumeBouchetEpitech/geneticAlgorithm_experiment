
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <string>

// forward declaration
class FrameBuffer;
class Image;

class Texture
{
  friend FrameBuffer; // required by FrameBuffer::attachColorTexture()

private:
  unsigned int _textureId = 0;
  glm::ivec2 _size = { 0, 0 };

public:
  Texture() = default;
  ~Texture();

public:
  void setFromImage(const Image& image, bool pixelated = false, bool repeat = false);
  void allocateBlank(
    const glm::ivec2& size,
    bool pixelated = false,
    bool repeat = false,
    const void* pixels = nullptr);

public:
  const glm::ivec2& getSize() const;

public:
  void bind() const;

public:
  static void unbind();

};
