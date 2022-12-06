
#include "PostProcess.hpp"

#include "framework/asValue.hpp"

#include "framework/math/easingFunctions.hpp"

#include "framework/graphic/GeometryBuilder.hpp"
#include "framework/graphic/ShaderProgramBuilder.hpp"

#include "framework/graphic/GlContext.hpp"

// #define D_EXPERIMENTAL_CURVED_HUD

void PostProcess::initialise(const glm::uvec2& frameSize) {
  {

    ShaderProgramBuilder shaderProgramBuilder;

    _shader = shaderProgramBuilder.reset()
                .setVertexFilename("assets/shaders/postProcess.glsl.vert")
                .setFragmentFilename("assets/shaders/postProcess.glsl.frag")
                .addAttribute("a_position")
                .addAttribute("a_animationNormal")
                .addAttribute("a_texCoord")
                .addUniform("u_composedMatrix")
                .addUniform("u_animationCoef")
                .addUniform("u_colorTexture")
                // .addUniform("u_depthTexture")
                // .addUniform("u_depthTexture")
                .build();
  }

  {

    GeometryBuilder geometryBuilder;

    geometryBuilder.reset()
      .setShader(*_shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
      .addVboAttribute("a_animationNormal", Geometry::AttrType::Vec3f, 3)
      .addVboAttribute("a_texCoord", Geometry::AttrType::Vec2f, 6);
    geometryBuilder.build(_screenQuad);
  }

  resize(frameSize);
}

void PostProcess::bind() {
  _frameBuffer.bind();
  GlContext::clearColor(0, 0, 0, 0);
  GlContext::clearDepth(1.0f);
  GlContext::clear(asValue(GlContext::Buffers::color) |
                   asValue(GlContext::Buffers::depth));
}

void PostProcess::unbind() { FrameBuffer::unbind(); }

void PostProcess::render() {
  GlContext::disable(GlContext::States::cullFace);
  GlContext::disable(GlContext::States::depthTest);

  // #if not defined D_EXPERIMENTAL_CURVED_HUD

  const glm::vec2 hsize = glm::vec2(_frameSize) * 0.5f;
  const glm::mat4 projection =
    glm::ortho(-hsize.x, hsize.x, -hsize.y, hsize.y, -10.0f, 10.0f);

  const glm::vec3 eye = {0.0f, 0.0f, 1.0f};
  const glm::vec3 center = {0.0f, 0.0f, 0.0f};
  const glm::vec3 upAxis = {0.0f, 1.0f, 0.0f};
  const glm::mat4 viewMatrix = glm::lookAt(eye, center, upAxis);

  // #else

  //   const glm::vec2 vSize = glm::vec2(_frameSize);

  //   const float fovy = glm::radians(70.0f);
  //   const float aspectRatio = vSize.x / vSize.y;
  //   glm::mat4 projection = glm::perspective(fovy, aspectRatio, 1.0f,
  //   1000.0f);

  //   glm::vec3 eye = { 0, 0, 425 };
  //   glm::vec3 center = { 0, 0, 0 };
  //   glm::vec3 upAxis = { 0, 1, 0 };
  //   glm::mat4 viewMatrix = glm::lookAt(eye, center, upAxis);

  // #endif

  const glm::mat4 composed = projection * viewMatrix;

  _shader->bind();
  _shader->setUniform("u_composedMatrix", composed);

  if (_animation.duration > 0.0f) {
    const float progress = _animation.timeLeft / _animation.duration;
    // if (coef < 0.5f)
    //   coef = coef * 2.0f;
    // else
    //   coef = 1.0f - coef * 2.0f;

    // const float coef = easing::easeOutElastic(progress);
    const float coef = easing::easeInBounce(progress);

    _shader->setUniform("u_animationCoef", coef * 15.0f);
  } else {
    _shader->setUniform("u_animationCoef", 0.0f);
  }

  _shader->setUniform("u_colorTexture", 0);
  GlContext::activeTexture(0);
  _colorTexture.bind();

  // _shader->setUniform("u_depthTexture", 1);
  // GlContext::activeTexture(1);
  // _depthTexture.bind();

  _screenQuad.render();

  GlContext::activeTexture(0);

  GlContext::enable(GlContext::States::depthTest);
}

void PostProcess::resize(const glm::uvec2& frameSize) {
  _frameSize = frameSize;

  {

    // _frameBuffer.dispose();

  }

  {

    _colorTexture.allocateBlank(_frameSize, true, false);
    // _depthTexture.allocateDepth(_frameSize);
    _depthRenderBuffer.allocateDepth(_frameSize);

    FrameBuffer::Definition def;
    def.colorTextures.push_back({0, &_colorTexture});
    // def.depthTexture = &_depthTexture;
    def.renderBuffer = &_depthRenderBuffer;
    _frameBuffer.initialise(def);
  }

  {

    struct Vertex {
      glm::vec3 position;
      glm::vec3 animationNormal;
      glm::vec2 texCoord;
    };

// #if not defined D_EXPERIMENTAL_CURVED_HUD
#if 0

    const glm::vec2 hsize = glm::vec2(_frameSize) * 0.5f;

    std::array<Vertex, 4> quadVertices
    {{
      { { +hsize.x, -hsize.y, 0.0f }, { 1.0f, 0.0f } },
      { { -hsize.x, -hsize.y, 0.0f }, { 0.0f, 0.0f } },
      { { +hsize.x, +hsize.y, 0.0f }, { 1.0f, 1.0f } },
      { { -hsize.x, +hsize.y, 0.0f }, { 0.0f, 1.0f } }
    }};

    std::array<std::size_t, 6> indices{{ 1,0,2,  1,3,2 }};

#else

    const std::array<int, 6> refIndices{{1, 0, 2, 1, 3, 2}};

    const int divider = 64;
    // const int divider = 1; // disable perspective hud for now

    const glm::vec2 vSize = glm::vec2(_frameSize);
    const glm::vec2 hSize = vSize * 0.5f;

    glm::vec2 quadSize = {vSize.x / divider, vSize.y / divider};

    const glm::vec2 texCoord = {(1.0f / divider) * 1.0f,
                                (1.0f / divider) * 1.0f};

    std::vector<Vertex> quadVertices;
    std::vector<int> indices;
    quadVertices.reserve(128);
    indices.reserve(128);

    int tmp_index = 0;

    for (int yy = 0; yy < divider; ++yy)
      for (int xx = 0; xx < divider; ++xx) {
        const float verticalForwardDir = ((xx % 2) == 0) ? -1.0f : +1.0f;
        const float verticalBackwardDir = -verticalForwardDir;

        quadVertices.push_back({
          {-hSize.x + quadSize.x * (xx + 1), -hSize.y + quadSize.y * (yy + 0),
           0.0f},
          {0, verticalBackwardDir, 0},
          {texCoord.x * (xx + 1), texCoord.y * (yy + 0)},
        });
        quadVertices.push_back({
          {-hSize.x + quadSize.x * (xx + 0), -hSize.y + quadSize.y * (yy + 0),
           0.0f},
          {0, verticalForwardDir, 0},
          {texCoord.x * (xx + 0), texCoord.y * (yy + 0)},
        });
        quadVertices.push_back({
          {-hSize.x + quadSize.x * (xx + 1), -hSize.y + quadSize.y * (yy + 1),
           0.0f},
          {0, verticalBackwardDir, 0},
          {texCoord.x * (xx + 1), texCoord.y * (yy + 1)},
        });
        quadVertices.push_back({
          {-hSize.x + quadSize.x * (xx + 0), -hSize.y + quadSize.y * (yy + 1),
           0.0f},
          {0, verticalForwardDir, 0},
          {texCoord.x * (xx + 0), texCoord.y * (yy + 1)},
        });

        for (int idx : refIndices)
          indices.push_back(tmp_index * 4 + idx);

        ++tmp_index;
      }

#endif

    std::vector<Vertex> vertices;
    vertices.reserve(indices.size()); // pre-allocate
    for (std::size_t index : indices)
      vertices.push_back(quadVertices.at(index));

    _screenQuad.updateBuffer(0, vertices);
    _screenQuad.setPrimitiveCount(uint32_t(vertices.size()));
  }
}

void PostProcess::update(float elapsedTime) {
  if (_animation.timeLeft <= 0.0f)
    return;

  _animation.timeLeft -= elapsedTime;
  if (_animation.timeLeft < 0.0f) {
    _animation.timeLeft = 0.0f;
    // animate();
  }
}

void PostProcess::animate() {
  constexpr float duration = 3.0f;
  _animation.duration = std::max(0.0f, duration);
  _animation.timeLeft = _animation.duration;
}
