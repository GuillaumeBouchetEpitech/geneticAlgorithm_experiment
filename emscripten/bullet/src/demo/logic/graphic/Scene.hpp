
#pragma once

#include "demo/logic/Context.hpp"

#include "framework/helpers/GLMath.hpp"

class Scene {
public:
  static void initialise();
  static void renderSimple();
  static void renderAll();

  static void updateMatrices(float elapsedTime);

private:
  static void _clear();

public:
  static void _renderLeadingCarSensors();
  static void _renderFloor(const Camera& camera);

private:
  static void _renderHUD_ortho();
  static void _renderHUD_thirdPerson();
  static void _renderHUD();
};
