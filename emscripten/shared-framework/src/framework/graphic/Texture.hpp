
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <string>

// forward declaration
class FrameBuffer;
class Image;

class Texture {
  friend FrameBuffer; // required by FrameBuffer::attachColorTexture()

private:
  unsigned int _textureId = 0;
  glm::uvec2 _size = {0, 0};

public:
  Texture() = default;
  ~Texture();

public:
  void setFromImage(const Image& image, bool pixelated = false,
                    bool repeat = false);
  void allocateBlank(const glm::uvec2& size, bool pixelated = false,
                     bool repeat = false, const void* pixels = nullptr);
  void allocateDepth(const glm::uvec2& size);
  void dispose();

  void getAsImage(Image& image);

public:
  const glm::uvec2& getSize() const;
  bool isValid() const;

public:
  void bind() const;

public:
  static void unbind();
};
