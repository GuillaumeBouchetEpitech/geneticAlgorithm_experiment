
#pragma once

#include "framework/helpers/GLMath.hpp"

class TopologyRenderer
{
private:
  float _animationTime = 0.0f;

public:
  TopologyRenderer() = default;

  void update(float elapsedTime);

  void render(
    const glm::vec2& position,
    const glm::vec2& size);

};
