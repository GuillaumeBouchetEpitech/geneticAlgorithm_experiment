
#pragma once

#include "framework/helpers/GLMath.hpp"

// forward declaration
class FrameBuffer;

class RenderBuffer
{
  friend FrameBuffer; // required by FrameBuffer::attachDepthRenderBuffer()

private:
  unsigned int _bufferId = 0;
  glm::ivec2 _size = { 0, 0 };

public:
  RenderBuffer() = default;
  ~RenderBuffer();

public:
  void allocateDepth(const glm::ivec2& size);
  void dispose();

public:
  const glm::ivec2& getSize() const;

public:
  void bind() const;

public:
  static void unbind();

};

