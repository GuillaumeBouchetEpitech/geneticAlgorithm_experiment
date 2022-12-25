
#include "Texture.hpp"

#include "FrameBuffer.hpp"
#include "Image.hpp"

#include "GlContext.hpp"

#include "framework/system/ErrorHandler.hpp"

Texture::DepthCompatibleValues Texture::s_depthCompatibleValues;

Texture::~Texture() { dispose(); }

//

void Texture::setFromImage(const Image& image,
                           Quality quality /*= Quality::pixelated*/,
                           Pattern pattern /*= Pattern::clamped*/) {
  allocateBlank(image.getSize(), quality, pattern, image.getPixels());
}

void Texture::allocateBlank(const glm::uvec2& size,
                            Quality quality /*= Quality::pixelated*/,
                            Pattern pattern /*= Pattern::clamped*/,
                            const void* pixels /*= nullptr*/) {
  if (size.x == 0 || size.y == 0)
    D_THROW(std::runtime_error,
            "texture allocated with incorrect size, size.x: "
              << size.x << ", size.y: " << size.y);

  // TODO: check max texture size
  // if (_size.x < 1 || _size.y < 1)
  //   D_THROW(std::runtime_error, "image allocated with incorrect size");

  _size = size;

  if (_textureId == 0)
    _textureId = GlContext::Texture::generateOne();

  GlContext::Texture::bind(_textureId);

  GlContext::Texture::uploadPixels(uint32_t(_size.x), uint32_t(_size.y),
                                   pixels);

  GlContext::Texture::setTextureAsRepeat(pattern == Pattern::repeat);

  if (quality == Quality::pixelated) {
    GlContext::Texture::setTextureAsPixelated();
  } else {
    GlContext::Texture::setTextureAsSmoothed(quality ==
                                             Quality::smoothedAndMipMapped);
  }

  GlContext::Texture::bind(0);
}

void Texture::allocateSingleFloat(const glm::uvec2& size) {
  if (size.x == 0 || size.y == 0)
    D_THROW(std::runtime_error,
            "texture allocated with incorrect size, size.x: "
              << size.x << ", size.y: " << size.y);

  // TODO: check max texture size
  // if (_size.x < 1 || _size.y < 1)
  //   D_THROW(std::runtime_error, "image allocated with incorrect size");

  _size = size;

  if (_textureId == 0)
    _textureId = GlContext::Texture::generateOne();

  GlContext::Texture::bind(_textureId);

  GlContext::Texture::uploadSingleFloatPixels(uint32_t(_size.x),
                                              uint32_t(_size.y));

  GlContext::Texture::bind(0);
}

namespace {
GlContext::Texture::DepthFormat
getRawDepthFormat(Texture::DepthFormat depthFormat) {
  switch (depthFormat) {
  case Texture::DepthFormat::depth16:
    return GlContext::Texture::DepthFormat::depth16;
  case Texture::DepthFormat::depth24:
    return GlContext::Texture::DepthFormat::depth24;
  case Texture::DepthFormat::depth32:
    return GlContext::Texture::DepthFormat::depth32;
  case Texture::DepthFormat::depth32f:
    return GlContext::Texture::DepthFormat::depth32f;
  }
}
GlContext::Texture::DepthType getRawDepthType(Texture::DepthType depthType) {
  switch (depthType) {
  case Texture::DepthType::float32:
    return GlContext::Texture::DepthType::float32;
  case Texture::DepthType::unsingedInt:
    return GlContext::Texture::DepthType::unsingedInt;
  default:
    return GlContext::Texture::DepthType::unsingedShort;
  }
}
} // namespace

void Texture::allocateDepth(const glm::uvec2& size, DepthFormat depthFormat,
                            DepthType depthType) {
  _size = size;

  // TODO: check max texture size
  // if (_size.x < 1 || _size.y < 1)
  //   D_THROW(std::runtime_error, "image allocated with incorrect size");

  if (_textureId == 0)
    _textureId = GlContext::Texture::generateOne();

  const GlContext::Texture::DepthFormat rawFormat =
    getRawDepthFormat(depthFormat);
  const GlContext::Texture::DepthType rawType = getRawDepthType(depthType);

  GlContext::Texture::bind(_textureId);
  GlContext::Texture::setAsDepthTexture(_size.x, _size.y, rawFormat, rawType);
  GlContext::Texture::bind(0);
}

void Texture::allocateCompatibleDepth(const glm::uvec2& size) {
  Texture::ensureCompatibleDepth();
  allocateDepth(size, s_depthCompatibleValues.depthFormat,
                s_depthCompatibleValues.depthType);
}

void Texture::ensureCompatibleDepth() {
  if (s_depthCompatibleValues.computed == true)
    return;

  s_depthCompatibleValues.computed = true;

  struct DepthParameters {
    Texture::DepthFormat format;
    Texture::DepthType type;
    std::string_view msg;
  };

  std::array<DepthParameters, 4> allConfig = {{
    // { Texture::DepthFormat::depth32f, Texture::DepthType::float32, "float32"
    // },
    {Texture::DepthFormat::depth32, Texture::DepthType::unsingedInt,
     "depth32-int"},
    {Texture::DepthFormat::depth24, Texture::DepthType::unsingedInt,
     "depth24-int"},
    {Texture::DepthFormat::depth16, Texture::DepthType::unsingedInt,
     "depth16-int"},
    {Texture::DepthFormat::depth16, Texture::DepthType::unsingedShort,
     "depth16-short"},
  }};

  Texture tmpColorTexture;
  Texture tmpDepthTexture;
  FrameBuffer tmpFrameBuffer;
  const glm::uvec2 tmpSize = {10, 10};

  tmpColorTexture.allocateBlank(tmpSize);

  for (auto& currConfig : allConfig) {
    tmpDepthTexture.allocateDepth(tmpSize, currConfig.format, currConfig.type);

    FrameBuffer::Definition def;
    def.colorTextures.push_back({0, &tmpColorTexture});
    def.depthTexture = &tmpDepthTexture;
    if (tmpFrameBuffer.initialise(def, false)) {
      s_depthCompatibleValues.depthFormat = currConfig.format;
      s_depthCompatibleValues.depthType = currConfig.type;
      // D_MYLOG("currConfig.msg " << currConfig.msg);
      break;
    }
  }

  if (!tmpFrameBuffer.isLinked())
    D_THROW(std::runtime_error, "depth texture are unavailable");
}

void Texture::dispose() {
  if (!isValid())
    return;

  _size.x = 0;
  _size.y = 0;
  GlContext::Texture::deleteOne(_textureId);
  _textureId = 0;
}

void Texture::getAsImage(Image& image) {
  if (!isValid())
    D_THROW(std::runtime_error, "texture not valid");

  image.dispose();

  FrameBuffer::Definition def;
  def.colorTextures.push_back({0, this});

  FrameBuffer tmpFrameBuffer;
  tmpFrameBuffer.initialise(def);
  tmpFrameBuffer.getAsImage(image, 0, 0, _size.x, _size.y);
}

const glm::uvec2& Texture::getSize() const { return _size; }

bool Texture::isValid() const {
  return _size.x > 0 && _size.x > 0 && _textureId != 0;
}

//

void Texture::bind() const {
  if (_textureId == 0)
    D_THROW(std::runtime_error, "not allocated");

  GlContext::Texture::bind(_textureId);
}

void Texture::unbind() { GlContext::Texture::bind(0); }
