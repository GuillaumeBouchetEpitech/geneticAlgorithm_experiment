
#include "ScreenTitles.hpp"

#include "demo/logic/Context.hpp"

#include "framework/system/math/easingFunctions.hpp"

#include <iomanip>

void
ScreenTitles::fadeIn(float delay, float duration) {
  _timer.start(delay, duration);

  constexpr float step1 = 0.25f;
  constexpr float step2 = 0.50f;
  constexpr float step3 = 0.75f;

  _backgroundEasing =
    GenericEasing<3>().push(0.00f, _backgroundAlpha).push(step1, 1.0f);

  _mainTitleEasing = GenericEasing<3>()
                       .push(0.00f, _mainTitleAlpha)
                       .push(step1, _mainTitleAlpha, easing::easeOutCubic)
                       .push(step2, 1.0f);

  _fitnessTitleEasing = GenericEasing<3>()
                          .push(0.00f, _fitnessTitleAlpha)
                          .push(step2, _fitnessTitleAlpha, easing::easeOutCubic)
                          .push(step3, 1.0f);

  _commentTitleEasing = GenericEasing<3>()
                          .push(0.00f, _commentTitleAlpha)
                          .push(step3, _commentTitleAlpha, easing::easeOutCubic)
                          .push(1.00f, 1.0f);
}

void
ScreenTitles::fadeOut(float delay, float duration) {
  _timer.start(delay, duration);

  constexpr float step1 = 0.2f;
  constexpr float step2 = 0.4f;
  constexpr float step3 = 0.6f;

  _mainTitleEasing =
    GenericEasing<3>().push(0.00f, _mainTitleAlpha).push(step1, 0.0f);

  _fitnessTitleEasing = GenericEasing<3>()
                          .push(0.00f, _fitnessTitleAlpha)
                          .push(step1, _fitnessTitleAlpha, easing::easeInCubic)
                          .push(step2, 0.0f);

  _commentTitleEasing = GenericEasing<3>()
                          .push(0.00f, _commentTitleAlpha)
                          .push(step2, _commentTitleAlpha, easing::easeInCubic)
                          .push(step3, 0.0f);

  _backgroundEasing = GenericEasing<3>()
                        .push(0.00f, _backgroundAlpha)
                        .push(step3, _backgroundAlpha, easing::easeInCubic)
                        .push(1.00f, 0.0f);
}

void
ScreenTitles::update(float elapsedTime) {
  if (!_timer.isDone()) {
    _timer.update(elapsedTime);

    const float val = _timer.getCoefElapsed();

    _backgroundAlpha = _backgroundEasing.get(val);
    _mainTitleAlpha = _mainTitleEasing.get(val);
    _fitnessTitleAlpha = _fitnessTitleEasing.get(val);
    _commentTitleAlpha = _commentTitleEasing.get(val);
  }
}

void
ScreenTitles::render() {
  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& textRenderer = graphic.hud.textRenderer;
  auto& vSize = graphic.camera.viewportSize;

  if (_backgroundAlpha > 0.0f) {

    graphic.hud.stackRenderers.triangles.pushQuad(
      glm::vec2(vSize * 0.5f), vSize,
      glm::vec4(0, 0, 0, _backgroundAlpha * 0.5f), 0.4f);
  }

  const float depth = 0.5f;

  if (_mainTitleAlpha > 0.0f) {

    std::stringstream sstr;
    sstr << "Generation: " << context.logic.simulation->getGenerationNumber();
    const std::string message = sstr.str();

    const glm::vec4 color = glm::vec4(0.8f, 0.8f, 0.8f, _mainTitleAlpha);
    const glm::vec4 outlineColor = glm::vec4(0.2f, 0.2f, 0.2f, _mainTitleAlpha);

    constexpr float scale = 3.0f;

    glm::vec2 textPos;
    textPos.x = vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale;
    textPos.y = vSize.y * 0.5f + 16 * scale;

    textRenderer.push(textPos, message, color, scale, depth, outlineColor);
  }

  {

    constexpr float scale = 2.0f;

    const float prevFitness = context.logic.fitnessStats.get(-2);
    const float currFitness = context.logic.fitnessStats.get(-1);

    if (currFitness > 0.0f) {

      if (_fitnessTitleAlpha > 0.0f) {

        std::stringstream sstr;
        sstr << "Fitness: " << std::fixed << std::setprecision(1)
             << currFitness;
        std::string message = sstr.str();

        const glm::vec4 color = glm::vec4(0.8f, 0.8f, 0.8f, _fitnessTitleAlpha);
        const glm::vec4 outlineColor =
          glm::vec4(0.2f, 0.2f, 0.2f, _fitnessTitleAlpha);

        glm::vec2 textPos;
        textPos.x = vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale;
        textPos.y = vSize.y * 0.5f + 0 * scale;

        textRenderer.push(textPos, message, color, scale, depth, outlineColor);
      }

      if (_commentTitleAlpha > 0.0f && currFitness != prevFitness) {

        std::stringstream sstr;

        const float diff = currFitness - prevFitness;

        sstr << std::fixed << std::setprecision(1);
        if (currFitness > prevFitness)
          sstr << "Smarter result (+" << diff << ")";
        else if (currFitness < prevFitness)
          sstr << "Worse result (" << diff << ")";

        const std::string message = sstr.str();

        const glm::vec4 color = glm::vec4(0.8f, 0.8f, 0.8f, _commentTitleAlpha);

        glm::vec4 outlineColor =
          glm::vec4(0.0f, 0.0f, 0.0f, _commentTitleAlpha);
        if (currFitness > prevFitness)
          outlineColor.y = 0.5f;
        else if (currFitness < prevFitness)
          outlineColor.x = 0.5f;

        glm::vec2 textPos;
        textPos.x = vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale;
        textPos.y = vSize.y * 0.5f - 16 * scale;

        textRenderer.push(textPos, message, color, scale, depth, outlineColor);
      }
    }
  }
}
