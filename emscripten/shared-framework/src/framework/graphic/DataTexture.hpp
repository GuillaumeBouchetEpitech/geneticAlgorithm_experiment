
#pragma once

#include "Texture.hpp"

#include "framework/helpers/GLMath.hpp"

#include <vector>

class DataTexture {

private:
  Texture _texture;
  uint32_t _dataLength = 0;

public:
  DataTexture() = default;
  ~DataTexture() = default;

public:
  void upload(const glm::vec4* inDataPointer, uint32_t inDataLength);
  void upload(const std::vector<glm::vec4>& data);

public:
  uint32_t getDataLength() const;

public:
  const Texture& getTexture() const;
};
