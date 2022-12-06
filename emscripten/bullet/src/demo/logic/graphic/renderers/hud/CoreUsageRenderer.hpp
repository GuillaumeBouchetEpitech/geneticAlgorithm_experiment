
#pragma once

#include "framework/helpers/GLMath.hpp"

struct CoreUsageRenderer {
  glm::vec2 position;
  glm::vec2 size;

  void renderWireframe();
  void renderHudText();
};