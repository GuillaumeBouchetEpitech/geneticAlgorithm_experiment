
#include "PostProcess.hpp"

#include "demo/logic/Context.hpp"
#include "demo/logic/graphicIds.hpp"

#include "framework/asValue.hpp"
#include "framework/graphic/GlContext.hpp"

void PostProcess::setMatricesData(const Camera::MatricesData& matricesData) {
  _matricesData = matricesData;
}

void PostProcess::initialise(const glm::uvec2& frameSize) {

  auto& resourceManager = Context::get().graphic.resourceManager;

  _shader = resourceManager.getShader(asValue(ShaderIds::postProcess));

  auto geoDef =
    resourceManager.getGeometryDefinition(asValue(GeometryIds::postProcess));
  _screenQuad.initialise(*_shader, geoDef);
  _screenQuad.setPrimitiveCount(0);

  resize(frameSize);
}

void PostProcess::startRecording() {
  _frameBuffer.bind();

  GlContext::setViewport(0, 0, _frameSize.x, _frameSize.y);
  GlContext::clearColor(0, 0, 0, 0);
  GlContext::clearDepth(1.0f);
  GlContext::clear(asValue(GlContext::Buffers::color) |
                   asValue(GlContext::Buffers::depth));

  GlContext::setDepthFunc(GlContext::DepthFuncs::less);
}

void PostProcess::stopRecording() {
  FrameBuffer::unbind();
  GlContext::setViewport(0, 0, _frameSize.x, _frameSize.y);
}

void PostProcess::render() {

  _shader->bind();
  _shader->setUniform("u_composedMatrix", _matricesData.composed);

  _shader->setUniform("u_colorTexture", 0);
  GlContext::Texture::active(0);
  _colorTexture.bind();

  _shader->setUniform("u_outlineTexture", 1);
  GlContext::Texture::active(1);
  _outlineTexture.bind();

  _screenQuad.render();

  GlContext::Texture::active(0);
}

void PostProcess::resize(const glm::uvec2& inFrameSize) {

  _frameSize = inFrameSize;

  {

    _colorTexture.allocateBlank(_frameSize, Texture::Quality::pixelated,
                                Texture::Pattern::clamped);
    _outlineTexture.allocateBlank(_frameSize, Texture::Quality::pixelated,
                                  Texture::Pattern::clamped);
    _depthRenderBuffer.allocateCompatibleDepth(_frameSize);

    FrameBuffer::Definition def;
    def.colorTextures.push_back({0, &_colorTexture});
    def.colorTextures.push_back({1, &_outlineTexture});
    def.renderBuffer = &_depthRenderBuffer;
    _frameBuffer.initialise(def);
  }

  {

    const glm::vec2 fSize = glm::vec2(_frameSize);

    std::array<glm::vec3, 4> quadVertices{{{+fSize.x, -fSize.y, -2.0f},
                                           {-fSize.x, -fSize.y, -2.0f},
                                           {+fSize.x, +fSize.y, -2.0f},
                                           {-fSize.x, +fSize.y, -2.0f}}};

    std::array<std::size_t, 6> indices{{1, 0, 2, 1, 3, 2}};

    std::vector<glm::vec3> vertices;
    vertices.reserve(indices.size()); // pre-allocate
    for (std::size_t index : indices)
      vertices.push_back(quadVertices.at(index));

    _screenQuad.updateBuffer(0, vertices);
    _screenQuad.setPrimitiveCount(uint32_t(vertices.size()));
  }
}
