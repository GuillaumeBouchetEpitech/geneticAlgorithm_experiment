
#include "DataTexture.hpp"

#include "FrameBuffer.hpp"
#include "Image.hpp"

#include "GlContext.hpp"

#include "framework/system/ErrorHandler.hpp"

void DataTexture::upload(const glm::vec4* inDataPointer,
                         uint32_t inDataLength) {
  _dataLength = inDataLength;
  if (_dataLength == 0)
    return;

  // TODO: use the full range of the 2d texture (height > 1)

  // _texture.allocateSingleFloat({ _dataLength, 1 }, inDataPointer);
  _texture.allocateFloatBlank({_dataLength, 1}, Texture::Quality::pixelated,
                              Texture::Pattern::clamped, inDataPointer);
}

void DataTexture::upload(const std::vector<glm::vec4>& data) {
  upload(static_cast<const glm::vec4*>(data.data()), uint32_t(data.size()));
}

uint32_t DataTexture::getDataLength() const { return _dataLength; }
const Texture& DataTexture::getTexture() const { return _texture; }
