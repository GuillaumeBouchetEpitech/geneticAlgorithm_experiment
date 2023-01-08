
#pragma once

#include "framework/graphic/FrameBuffer.hpp"
#include "framework/graphic/Geometry.hpp"
#include "framework/graphic/RenderBuffer.hpp"
#include "framework/graphic/ShaderProgram.hpp"
#include "framework/graphic/Texture.hpp"

#include "framework/graphic/camera/Camera.hpp"

#include "framework/helpers/GLMath.hpp"

#include <memory>

class PostProcess {
private:
  glm::uvec2 _frameSize = {800, 600};

  Texture _colorTexture;
  Texture _outlineTexture;
  RenderBuffer _depthRenderBuffer;
  FrameBuffer _frameBuffer;

  std::shared_ptr<ShaderProgram> _shader = nullptr;
  Geometry _screenQuad;

  struct Animation {
    float duration = 1.0f;
    float timeLeft = 0.0f;
  } _animation;

  Camera::MatricesData _matricesData;

public:
  PostProcess() = default;
  ~PostProcess() = default;

public:
  void setMatricesData(const Camera::MatricesData& matricesData);

  void initialise(const glm::uvec2& frameSize);
  void dispose();
  void startRecording();
  void stopRecording();
  void render();
  void resize(const glm::uvec2& frameSize);
  void
  setGeometry(const glm::vec2& pos, const glm::vec2& size, float depth = 0.0f);
};
