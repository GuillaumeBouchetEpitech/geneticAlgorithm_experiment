
#pragma once

#include "Texture.hpp"
#include "RenderBuffer.hpp"

#include "framework/helpers/GLMath.hpp"

#include <vector>
#include <cstdint>

class Image;

class FrameBuffer
{
public:
  struct Definition
  {
    struct ColorTexture
    {
      uint32_t index = 0;
      Texture* texture = nullptr;
    };

    std::vector<ColorTexture> colorTextures;

    RenderBuffer* renderBuffer = nullptr;
    Texture* depthTexture = nullptr;
  };

private:
  uint32_t _frameBufferId = 0;

public:
  FrameBuffer() = default;
  ~FrameBuffer();

public:
  void initialise(const Definition& def);
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

public:
  void getAsImage(Image& image, uint32_t posX, uint32_t posY, uint32_t width, uint32_t height) const;

};
