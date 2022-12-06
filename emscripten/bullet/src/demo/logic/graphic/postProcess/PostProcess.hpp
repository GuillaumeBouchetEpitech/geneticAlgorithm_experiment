
#pragma once

#include "framework/graphic/FrameBuffer.hpp"
#include "framework/graphic/Geometry.hpp"
#include "framework/graphic/RenderBuffer.hpp"
#include "framework/graphic/ShaderProgram.hpp"
#include "framework/graphic/Texture.hpp"

#include "framework/helpers/GLMath.hpp"

#include <memory>

class PostProcess {
private:
  glm::uvec2 _frameSize = {800, 600};

  Texture _colorTexture;
  Texture _depthTexture;
  RenderBuffer _depthRenderBuffer;
  FrameBuffer _frameBuffer;

  std::shared_ptr<ShaderProgram> _shader = nullptr;
  Geometry _screenQuad;

  struct Animation {
    float duration = 1.0f;
    float timeLeft = 0.0f;
  } _animation;

public:
  PostProcess() = default;
  ~PostProcess() = default;

public:
  void initialise(const glm::uvec2& frameSize);
  void dispose();
  void bind();
  void unbind();
  void render();
  void resize(const glm::uvec2& frameSize);
  void update(float elapsedTime);
  void animate();
};
