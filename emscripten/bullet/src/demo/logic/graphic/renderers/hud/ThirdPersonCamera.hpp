
#pragma once

#include "demo/logic/graphic/renderers/hud/PostProcess.hpp"

#include "framework/helpers/GLMath.hpp"
#include "framework/system/Timer.hpp"
#include "framework/system/math/GenericEasing.hpp"

class ThirdPersonCamera {
private:
  glm::vec2 _position;
  glm::vec2 _size;

  bool _isVisible = false;
  Timer _timer;
  GenericEasing<2> _moveEasing;

  PostProcess _postProcess;

  glm::vec3 _eye = {0.0f, 0.0f, 0.0f};
  glm::vec3 _upAxis = {0.0f, 0.0f, 1.0f};
  Camera _camera;

public:
  ThirdPersonCamera() = default;
  ~ThirdPersonCamera() = default;

public:
  void initialise();

public:
  bool canActivate() const;

public:
  void fadeIn(float delay, float duration);
  void fadeOut(float delay, float duration);
  void resize();

  void update(float elapsedTime);

  void render();
};
