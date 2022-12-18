
#pragma once

#include "AnimationManager.hpp"

#include "framework/helpers/GLMath.hpp"

class ThirdPersonCamera {
private:
  glm::vec2 _position;
  glm::vec2 _size;

  AnimationManager::AnimationRef _animRef;

public:
  ThirdPersonCamera() = default;
  ~ThirdPersonCamera() = default;

public:
  void initialise();

public:
  void fadeIn();
  void fadeOut();

  void render();
};
