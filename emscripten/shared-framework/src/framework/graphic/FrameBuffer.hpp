
#pragma once

#include "Texture.hpp"
#include "RenderBuffer.hpp"

#include "framework/helpers/GLMath.hpp"

class FrameBuffer
{
private:
  unsigned int _frameBufferId = 0;

public:
  FrameBuffer() = default;
  ~FrameBuffer();

public:
  void initialise();
  void attachColorTexture(Texture& texture);
  void attachDepthRenderBuffer(RenderBuffer& buffer);

public:
  void executeCheck() const;

public:
  void bind() const;

public:
  static void unbind();

};
