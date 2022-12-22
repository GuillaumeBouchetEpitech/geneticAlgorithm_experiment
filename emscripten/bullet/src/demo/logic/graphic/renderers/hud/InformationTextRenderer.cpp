
#include "InformationTextRenderer.hpp"

#include "demo/logic/Context.hpp"
#include "helpers/writeTime.hpp"

#include "framework/system/math/easingFunctions.hpp"

#include <iomanip>

void InformationTextRenderer::fadeIn(float delay, float duration) {
  _timer.start(delay, duration);

  _alphaEasing = GenericEasing<2>()
                   .push(0.0f, _alpha, easing::easeOutCubic)
                   .push(1.0f, 1.0f);
}

void InformationTextRenderer::fadeOut(float delay, float duration) {
  _timer.start(delay, duration);

  _alphaEasing =
    GenericEasing<2>().push(0.0f, _alpha, easing::easeInCubic).push(1.0f, 0.0f);
}

void InformationTextRenderer::update(float elapsedTime) {
  if (!_timer.isDone()) {
    _timer.update(elapsedTime);
    _alpha = _alphaEasing.get(_timer.getCoefElapsed());
  }
}

void InformationTextRenderer::render() {
  auto& context = Context::get();
  auto& logic = context.logic;
  auto& graphic = context.graphic;
  auto& textRenderer = graphic.hud.textRenderer;
  auto& vSize = graphic.camera.viewportSize;

  const glm::vec4 color = glm::vec4(0.8f, 0.8f, 0.8f, _alpha);
  const float scale = 1.0f;
  const float depth = 0.25f;

  { // top-left header text

    textRenderer.push({8, vSize.y - 16 - 8}, logic.hudText.header, color, scale,
                      depth);

  } // top-left header text

  { // top-left performance stats

    std::stringstream sstr;

    sstr << "Update: " << writeTime(logic.metrics.updateTime) << std::endl
         << "Render: " << writeTime(logic.metrics.renderTime);

    const std::string str = sstr.str();

    textRenderer.push({8, vSize.y - 5 * 16 - 8}, str, color, scale, depth);

  } // top-left performance stats
}
