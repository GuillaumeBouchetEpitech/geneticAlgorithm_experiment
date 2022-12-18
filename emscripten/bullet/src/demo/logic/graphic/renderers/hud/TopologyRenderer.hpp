
#pragma once

#include "AnimationManager.hpp"

#include "framework/helpers/GLMath.hpp"

class TopologyRenderer {
private:
  float _animationTime = 0.0f;

  glm::vec2 _position;
  glm::vec2 _size;

  AnimationManager::AnimationRef _animRef;

public:
  TopologyRenderer() = default;

public:
  void initialise();

  void fadeIn();
  void fadeOut();

  void update(float elapsedTime);

  void render();
};
