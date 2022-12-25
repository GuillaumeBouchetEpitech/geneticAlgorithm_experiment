
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
  auto& stackRenderers = graphic.hud.stackRenderers;

  const glm::vec4 color = glm::vec4(0.8f, 0.8f, 0.8f, _alpha);
  const glm::vec4 outlineColor = glm::vec4(0.2f, 0.2f, 0.2f, _alpha);
  const glm::vec4 bgColor = glm::vec4(0.0f, 0.0f, 0.0f, _alpha * 0.75f);
  const float scale = 1.0f;
  const float depth = 0.25f;
  const float bgDepth = depth - 0.1f;

  std::vector<TextRenderer::Rectangle> outRectangles;

  { // top-left header text

    const glm::vec2 textPos = {8, vSize.y - 16 - 8};

    textRenderer.push(textPos, logic.hudText.header, color, scale, depth, outlineColor);

    textRenderer.getSizes(outRectangles, textPos, logic.hudText.header, scale);

    for (const auto& rec : outRectangles)
      stackRenderers.triangles.pushQuad(rec.pos + rec.size * 0.5f, rec.size, bgColor, bgDepth);

  } // top-left header text

  { // top-left performance stats

    std::stringstream sstr;
    sstr << "Update:" << std::endl;
    sstr << writeTime(logic.metrics.updateTime) << std::endl;
    sstr << std::endl;
    sstr << "Render:" << std::endl;
    sstr << writeTime(logic.metrics.renderTime);
    const std::string str = sstr.str();

    const glm::vec2 textPos = {8, vSize.y - 5 * 16 - 8};

    textRenderer.push({8, vSize.y - 5 * 16 - 8}, str, color, scale, depth, outlineColor);

    textRenderer.getSizes(outRectangles, textPos, str, scale);

    for (const auto& rec : outRectangles)
      if (rec.size.x > 0.0f)
        stackRenderers.triangles.pushQuad(rec.pos + rec.size * 0.5f, rec.size, bgColor, bgDepth);

  } // top-left performance stats
}
