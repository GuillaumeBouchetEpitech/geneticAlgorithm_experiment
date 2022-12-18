
#pragma once

#include "RenderBuffer.hpp"
#include "Texture.hpp"

#include "framework/helpers/GLMath.hpp"

#include <cstdint>
#include <vector>

class Image;

class FrameBuffer {
public:
  struct Definition {
    struct ColorTexture {
      uint32_t index = 0;
      Texture* texture = nullptr;
    };

    std::vector<ColorTexture> colorTextures;

    RenderBuffer* renderBuffer = nullptr;
    Texture* depthTexture = nullptr;
  };

private:
  uint32_t _frameBufferId = 0;
  bool _isLinked = false;

public:
  FrameBuffer() = default;
  ~FrameBuffer();

public:
  bool initialise(const Definition& def, bool throwException = true);
  void dispose();

private:
  void _attachColorTexture(uint32_t index, const Texture& colorTexture);
  void _attachDepthTexture(const Texture& depthTexture);
  void _attachDepthRenderBuffer(const RenderBuffer& buffer);

public:
  void bind() const;

public:
  static void unbind();

public:
  bool isValid() const;
  bool isLinked() const;

public:
  void getAsImage(Image& image, uint32_t posX, uint32_t posY, uint32_t width,
                  uint32_t height) const;
};
