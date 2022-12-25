
#pragma once

#include "framework/graphic/Geometry.hpp"
#include "framework/graphic/ShaderProgram.hpp"
#include "framework/graphic/camera/Camera.hpp"
#include "framework/helpers/GLMath.hpp"
#include "framework/system/Timer.hpp"

#include <array>
#include <memory>
#include <vector>

class ModelsRenderer {
private:
  struct RenderPass {
    std::shared_ptr<ShaderProgram> shader;
    Geometry geometry;
  };

  RenderPass _chassis;
  RenderPass _wheels;

  struct Attributes {
    glm::mat4 tranform;
    glm::vec4 color;
    glm::vec4 outlineColor;

    Attributes(const glm::mat4& tranform, const glm::vec4& color,
               const glm::vec4& outlineColor)
      : tranform(tranform), color(color), outlineColor(outlineColor) {}
  };

  std::vector<Attributes> _modelsChassisMatrices;
  std::vector<Attributes> _modelWheelsMatrices;

  Timer _timer;
  float _colorAlpha = 0.0f;
  float _outlineColoralpha = 0.0f;

public:
  ModelsRenderer() = default;

public:
  void initialise();

public:
  void fadeIn(float delay, float duration);
  void update(float elapsedTime);
  void render(const Camera& cameraInstance);
};
