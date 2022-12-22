
#pragma once

#include "demo/logic/Context.hpp"

#include "framework/graphic/camera/Camera.hpp"
#include "framework/helpers/GLMath.hpp"

class Scene {
public:
  static void initialise();
  static void renderSimple();
  static void renderAll();

  static void updateMatrices();
  static void renderScene(const Camera& camera);

private:
  static void _clear();

private:
  static void _renderLeadingCarSensors();
  static void _renderHUD_ortho();
  static void _renderHUD();
};
