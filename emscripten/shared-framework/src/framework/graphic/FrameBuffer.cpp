
#include "FrameBuffer.hpp"
#include "Image.hpp"

#include "framework/system/ErrorHandler.hpp"

#include "GlContext.hpp"

FrameBuffer::~FrameBuffer() { dispose(); }

//

bool FrameBuffer::initialise(const Definition& def,
                             bool throwException /*= true*/) {
  if (def.colorTextures.empty())
    D_THROW(std::runtime_error, "empty frame buffer color texture array");
  for (const auto& currColorTexture : def.colorTextures) {
    if (currColorTexture.texture == nullptr)
      D_THROW(std::runtime_error,
              "null color texture in frame buffer color texture array");

    uint32_t totalSameIndex = 0;
    for (const auto& otherColorTexture : def.colorTextures)
      if (currColorTexture.index == otherColorTexture.index)
        totalSameIndex += 1;
    if (totalSameIndex > 1)
      D_THROW(
        std::runtime_error,
        "duplicated color texture index in frame buffer color texture array");

    uint32_t totalSameTexture = 0;
    for (const auto& otherColorTexture : def.colorTextures)
      if (currColorTexture.texture == otherColorTexture.texture)
        totalSameTexture += 1;
    if (totalSameTexture > 1)
      D_THROW(
        std::runtime_error,
        "duplicated color texture pointer in frame buffer color texture array");
  }
  if (def.depthTexture && def.renderBuffer)
    D_THROW(std::runtime_error,
            "cannot specify frame buffer depth texture and render buffer");
  // if (!def.depthTexture && !def.renderBuffer)
  //   D_THROW(std::runtime_error, "must specify frame buffer depth texture or
  //   render buffer");

  //
  //
  //

  if (_frameBufferId == 0)
    _frameBufferId = GlContext::FrameBuffers::generateOne();

  _isLinked = false;

  bind();

  for (const auto& currColorTexture : def.colorTextures)
    _attachColorTexture(currColorTexture.index, *currColorTexture.texture);

  if (def.depthTexture)
    _attachDepthTexture(*def.depthTexture);
  else if (def.renderBuffer)
    _attachDepthRenderBuffer(*def.renderBuffer);

  if (def.colorTextures.size() > 1) {

    std::vector<uint32_t> drawBuffers;
    drawBuffers.reserve(def.colorTextures.size());
    for (const auto& currColorTexture : def.colorTextures)
      drawBuffers.push_back(
        GlContext::FrameBuffers::getColorAttachment(currColorTexture.index));

    GlContext::FrameBuffers::drawMany(uint32_t(drawBuffers.size()),
                                      drawBuffers.data());
  }

  _isLinked = GlContext::FrameBuffers::check(throwException);

  FrameBuffer::unbind();

  return _isLinked;
}

void FrameBuffer::dispose() {
  if (!isValid())
    return;

  GlContext::FrameBuffers::deleteOne(_frameBufferId);
  _frameBufferId = 0;
}

void FrameBuffer::_attachColorTexture(uint32_t index,
                                      const Texture& colorTexture) {
  colorTexture.bind();
  GlContext::FrameBuffers::attachTexture2D(
    GlContext::FrameBuffers::getColorAttachment(index),
    colorTexture._textureId);
}

void FrameBuffer::_attachDepthTexture(const Texture& depthTexture) {
  depthTexture.bind();
  GlContext::FrameBuffers::attachDepthTexture2D(depthTexture._textureId);
}

void FrameBuffer::_attachDepthRenderBuffer(const RenderBuffer& buffer) {
  buffer.bind();
  GlContext::FrameBuffers::attachRenderbuffer(buffer._bufferId);
}

//

void FrameBuffer::bind() const {
  if (!isValid())
    D_THROW(std::runtime_error, "framebuffer not valid");

  GlContext::FrameBuffers::bind(_frameBufferId);
}

void FrameBuffer::unbind() { GlContext::FrameBuffers::bind(0); }

bool FrameBuffer::isValid() const { return _frameBufferId != 0; }

bool FrameBuffer::isLinked() const { return _isLinked; }

void FrameBuffer::getAsImage(Image& image, uint32_t posX, uint32_t posY,
                             uint32_t width, uint32_t height) const {
  if (!isValid())
    D_THROW(std::runtime_error, "framebuffer not valid");

  image.dispose();

  const uint32_t bufferSize = width * height * 4;
  uint8_t* pixels = new uint8_t[bufferSize];

  bind();
  GlContext::FrameBuffers::downloadPixels(posX, posY, width, height, pixels);
  unbind();

  image._rawPixels = pixels;
  image._size = {width, height};
}
