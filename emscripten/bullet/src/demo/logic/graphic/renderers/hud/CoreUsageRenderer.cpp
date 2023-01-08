
#include "CoreUsageRenderer.hpp"

#include "demo/logic/Context.hpp"
#include "helpers/writeTime.hpp"

#include "framework/system/math/easingFunctions.hpp"

namespace {

constexpr unsigned int k_slowdownDelta = 16 * 1000;
constexpr float k_divider = 5000.0f; // 5ms

constexpr float k_faceInX = +8.0f;
constexpr float k_faceOutX = -400.0f;

} // namespace

CoreUsageRenderer::CoreUsageRenderer() {
  _position.x = k_faceOutX;
  _position.y = 4 * 16 + 7;

  _size = {150, 100};
}

void
CoreUsageRenderer::fadeIn(float delay, float duration) {
  _timer.start(delay, duration);

  _moveEasing = GenericEasing<2>()
                  .push(0.0f, _position.x, easing::easeOutCubic)
                  .push(1.0f, k_faceInX);

  _isVisible = true;
}

void
CoreUsageRenderer::fadeOut(float delay, float duration) {
  _timer.start(delay, duration);

  _moveEasing = GenericEasing<2>()
                  .push(0.0f, _position.x, easing::easeInCubic)
                  .push(1.0f, k_faceOutX);

  _isVisible = false;
}

void
CoreUsageRenderer::update(float elapsedTime) {
  if (!_timer.isDone()) {
    _timer.update(elapsedTime);
    _position.x = _moveEasing.get(_timer.getCoefElapsed());
  }
}

void
CoreUsageRenderer::resize() {
  if (_isVisible)
    fadeIn(0.0f, 0.2f);
  else
    fadeOut(0.0f, 0.2f);
}

void
CoreUsageRenderer::renderWireframe() {

  auto& context = Context::get();
  auto& graphic = context.graphic;

  auto& stackRenderers = graphic.hud.stackRenderers;

  const glm::vec3 whiteColor(0.8f, 0.8f, 0.8f);
  const glm::vec3 redColor(1.0f, 0.0f, 0.0f);

  const glm::vec3 borderPos = {_position.x, _position.y + 16 + 8, 0.1f};
  const glm::vec2 borderSize = {_size.x, _size.y - 16 * 3};

  const auto& profileData = context.logic.cores.profileData;

  const unsigned int allTimeMaxDelta = profileData.getAllTimeMaxDelta();

  const float verticalSize =
    (std::ceil(float(allTimeMaxDelta) / k_divider)) * k_divider;

  { // background

    const glm::vec4 bgColor = allTimeMaxDelta > k_slowdownDelta
                                ? glm::vec4(0.5f, 0.0f, 0.0f, 0.75f)
                                : glm::vec4(0.0f, 0.0f, 0.0f, 0.75f);

    stackRenderers.triangles.pushQuad(
      glm::vec3(glm::vec2(borderPos) + borderSize * 0.5f, borderPos.z),
      borderSize, bgColor, -0.2f);
    stackRenderers.wireframes.pushRectangle(
      borderPos, borderSize, whiteColor, -0.1f);

  } // background

  //
  //

  { // dividers

    for (float currDivider = k_divider; currDivider < verticalSize;
         currDivider += k_divider) {
      const float ratio = currDivider / verticalSize;

      stackRenderers.wireframes.pushLine(
        borderPos + glm::vec3(0, borderSize.y * ratio, 0.0f),
        borderPos + glm::vec3(borderSize.x, borderSize.y * ratio, 0.0f),
        whiteColor);
    }

  } // dividers

  //
  //

  {

    const float widthStep = borderSize.x / profileData.getMaxStateHistory();

    for (std::size_t coreIndex = 0; coreIndex < profileData.getTotalCores();
         ++coreIndex)
      for (unsigned int statIndex = 0;
           statIndex + 1 < profileData.getMaxStateHistory(); ++statIndex) {
        const float prevDelta =
          float(profileData.getCoreHistoryData(coreIndex, statIndex + 0).delta);
        const float currDelta =
          float(profileData.getCoreHistoryData(coreIndex, statIndex + 1).delta);

        const float prevHeight = borderSize.y * prevDelta / verticalSize;
        const float currHeight = borderSize.y * currDelta / verticalSize;

        stackRenderers.wireframes.pushLine(
          borderPos + glm::vec3((statIndex + 0) * widthStep, prevHeight, 0.0f),
          borderPos + glm::vec3((statIndex + 1) * widthStep, currHeight, 0.0f),
          prevDelta < k_slowdownDelta ? whiteColor : redColor,
          currDelta < k_slowdownDelta ? whiteColor : redColor);
      }
  }

  stackRenderers.wireframes.flush();
  stackRenderers.triangles.flush();
}

void
CoreUsageRenderer::renderHudText() {

  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& logic = context.logic;
  auto& textRenderer = graphic.hud.textRenderer;
  auto& profileData = logic.cores.profileData;
  auto& stackRenderers = graphic.hud.stackRenderers;

  std::vector<TextRenderer::Rectangle> outRectangles;
  const glm::vec4 color = glm::vec4(0.8, 0.8, 0.8, 1);
  const glm::vec4 bgColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.75f);

  {
    std::stringstream sstr;

#if defined D_WEB_WEBWORKER_BUILD

    sstr << "WORKERS: " << profileData.getTotalCores();

#else

    sstr << "THREADS: " << profileData.getTotalCores();

#endif

    std::string str = sstr.str();

    const glm::vec2 textPos = {_position.x, _position.y};
    const float textScale = 1.0f;
    const float textDepth = 0.25f;

    textRenderer.push(textPos, str, color, textScale, textDepth);

    textRenderer.getSizes(outRectangles, textPos, str, textScale);
    for (const auto& rec : outRectangles)
      if (rec.size.x > 0.0f)
        stackRenderers.triangles.pushQuad(
          rec.pos + rec.size * 0.5f, rec.size, bgColor, textDepth - 0.1f);
  }

  {

    const unsigned int totalDelta = profileData.getLatestTotalDelta();

    std::stringstream sstr;
    sstr << "CPU time: " << writeTime(totalDelta);
    std::string str = sstr.str();

    const glm::vec2 textPos = {_position.x, _position.y - 16};
    const float textScale = 1.0f;
    const float textDepth = 0.25f;

    const glm::vec4 textColor =
      profileData.getAllTimeMaxDelta() > k_slowdownDelta
        ? glm::vec4(0.5f, 0.0f, 0.0f, 1)
        : glm::vec4(0.0f, 0.0f, 0.0f, 1);

    textRenderer.push(textPos, str, color, textScale, textDepth, textColor);

    textRenderer.getSizes(outRectangles, textPos, str, textScale);
    for (const auto& rec : outRectangles)
      if (rec.size.x > 0.0f)
        stackRenderers.triangles.pushQuad(
          rec.pos + rec.size * 0.5f, rec.size, bgColor, textDepth - 0.1f);
  }
}
