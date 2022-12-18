
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <string>

// forward declaration
class FrameBuffer;
class Image;

class Texture {
  friend FrameBuffer; // required by FrameBuffer::attachColorTexture()

public:
  enum class Quality {
    pixelated = 1,
    smoothed,
    smoothedAndMipMapped,
  };

  enum class Pattern {
    clamped = 1,
    repeat,
  };

public:
  enum class DepthFormat {
    depth16,
    depth24,
    depth32,
    depth32f,
  };

  enum class DepthType {
    unsingedShort,
    unsingedInt,
    float32,
  };

private:
  unsigned int _textureId = 0;
  glm::uvec2 _size = {0, 0};

  struct DepthCompatibleValues {
    bool computed = false;
    DepthFormat depthFormat = DepthFormat::depth32;
    DepthType depthType = DepthType::unsingedInt;
  };

  static DepthCompatibleValues s_depthCompatibleValues;

public:
  Texture() = default;
  ~Texture();

public:
  void setFromImage(const Image& image, Quality quality = Quality::pixelated,
                    Pattern pattern = Pattern::clamped);
  void allocateBlank(const glm::uvec2& size,
                     Quality quality = Quality::pixelated,
                     Pattern pattern = Pattern::clamped,
                     const void* pixels = nullptr);
  void allocateDepth(const glm::uvec2& size, DepthFormat depthFormat,
                     DepthType depthType);
  void allocateCompatibleDepth(const glm::uvec2& size);
  static void ensureCompatibleDepth();
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
