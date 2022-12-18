
#include "RenderBuffer.hpp"

#include "GlContext.hpp"

#include "FrameBuffer.hpp"
#include "Texture.hpp"

#include "framework/ErrorHandler.hpp"

#include <array>

RenderBuffer::DepthCompatibleValues RenderBuffer::s_depthCompatibleValues;

RenderBuffer::~RenderBuffer() { dispose(); }

//

namespace {
GlContext::RenderBuffer::DepthFormat
getRawDepthFormat(RenderBuffer::DepthFormat depthFormat) {
  switch (depthFormat) {
  case RenderBuffer::DepthFormat::depth16:
    return GlContext::RenderBuffer::DepthFormat::depth16;
  case RenderBuffer::DepthFormat::depth24:
    return GlContext::RenderBuffer::DepthFormat::depth24;
  case RenderBuffer::DepthFormat::depth32:
    return GlContext::RenderBuffer::DepthFormat::depth32;
  case RenderBuffer::DepthFormat::depth32f:
    return GlContext::RenderBuffer::DepthFormat::depth32f;
  }
}
} // namespace

void RenderBuffer::allocateDepth(const glm::uvec2& size,
                                 DepthFormat depthFormat) {

  _size = size;

  if (_size.x < 1 || _size.y < 1)
    D_THROW(std::runtime_error, "image allocated with incorrect size");

  if (_bufferId == 0)
    GlContext::RenderBuffer::generateMany(1, &_bufferId);

  const GlContext::RenderBuffer::DepthFormat rawFormat =
    getRawDepthFormat(depthFormat);

  GlContext::RenderBuffer::bind(_bufferId);
  GlContext::RenderBuffer::setSize(uint32_t(_size.x), uint32_t(_size.y),
                                   rawFormat);
  GlContext::RenderBuffer::bind(0);
}

void RenderBuffer::allocateCompatibleDepth(const glm::uvec2& size) {
  RenderBuffer::ensureCompatibleDepth();
  allocateDepth(size, s_depthCompatibleValues.depthFormat);
}

void RenderBuffer::ensureCompatibleDepth() {
  if (s_depthCompatibleValues.computed == true)
    return;

  s_depthCompatibleValues.computed = true;

  struct DepthParameters {
    RenderBuffer::DepthFormat format;
    std::string_view msg;
  };

  std::array<DepthParameters, 4> allConfig = {{
    // { Texture::DepthFormat::depth32f, "float32" },
    {RenderBuffer::DepthFormat::depth32, "depth32-int"},
    {RenderBuffer::DepthFormat::depth24, "depth24-int"},
    {RenderBuffer::DepthFormat::depth16, "depth16-int"},
    {RenderBuffer::DepthFormat::depth16, "depth16-short"},
  }};

  Texture tmpColorTexture;
  RenderBuffer tmpRenderBuffer;
  FrameBuffer tmpFrameBuffer;
  const glm::uvec2 tmpSize = {10, 10};

  tmpColorTexture.allocateBlank(tmpSize);

  for (auto& currConfig : allConfig) {
    tmpRenderBuffer.allocateDepth(tmpSize, currConfig.format);

    FrameBuffer::Definition def;
    def.colorTextures.push_back({0, &tmpColorTexture});
    def.renderBuffer = &tmpRenderBuffer;
    if (tmpFrameBuffer.initialise(def, false)) {
      s_depthCompatibleValues.depthFormat = currConfig.format;
      D_MYLOG("currConfig.msg " << currConfig.msg);
      break;
    }
  }

  if (!tmpFrameBuffer.isLinked())
    D_THROW(std::runtime_error, "render buffer are unavailable");
}

void RenderBuffer::dispose() {
  if (_bufferId == 0)
    return;

  GlContext::RenderBuffer::deleteMany(1, &_bufferId);
  _bufferId = 0;
}

const glm::uvec2& RenderBuffer::getSize() const { return _size; }

//

void RenderBuffer::bind() const {
  if (_bufferId == 0)
    D_THROW(std::runtime_error, "not allocated");

  GlContext::RenderBuffer::bind(_bufferId);
}

void RenderBuffer::unbind() { GlContext::RenderBuffer::bind(0); }
