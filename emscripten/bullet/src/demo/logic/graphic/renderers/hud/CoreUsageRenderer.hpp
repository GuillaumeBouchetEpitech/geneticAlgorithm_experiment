
#pragma once

#include "AnimationManager.hpp"

#include "framework/helpers/GLMath.hpp"

class CoreUsageRenderer {

private:
  glm::vec2 _position;
  glm::vec2 _size;

  AnimationManager::AnimationRef _animRef;

public:
  CoreUsageRenderer();

  void fadeIn();
  void fadeOut();

  void renderWireframe();
  void renderHudText();
};