
#pragma once

#include "framework/helpers/GLMath.hpp"

// forward declaration
class FrameBuffer;

class RenderBuffer {
  friend FrameBuffer; // required by FrameBuffer::attachDepthRenderBuffer()

public:
  enum class DepthFormat {
    depth16,
    depth24,
    depth32,
    depth32f,
  };

private:
  unsigned int _bufferId = 0;
  glm::uvec2 _size = {0, 0};

  struct DepthCompatibleValues {
    bool computed = false;
    DepthFormat depthFormat = DepthFormat::depth32;
  };

  static DepthCompatibleValues s_depthCompatibleValues;

public:
  RenderBuffer() = default;
  ~RenderBuffer();

public:
  void allocateDepth(const glm::uvec2& size, DepthFormat depthFormat);
  void allocateCompatibleDepth(const glm::uvec2& size);
  static void ensureCompatibleDepth();
  void dispose();

public:
  const glm::uvec2& getSize() const;
  bool isValid() const;

public:
  void bind() const;

public:
  static void unbind();
};
