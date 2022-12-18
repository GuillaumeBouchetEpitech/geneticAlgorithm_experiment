
#include "CoreUsageRenderer.hpp"

#include "demo/logic/graphic/helpers/writeTime.hpp"

#include "demo/logic/Context.hpp"

namespace {

constexpr unsigned int k_slowdownDelta = 16 * 1000;
constexpr float k_divider = 5000.0f; // 5ms

} // namespace

void CoreUsageRenderer::renderWireframe() {

  auto& context = Context::get();
  auto& graphic = context.graphic;

  auto& stackRenderers = graphic.hud.stackRenderers;

  const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);

  const glm::vec2 borderPos = {position.x, position.y + 16 + 8};
  const glm::vec2 borderSize = {size.x, size.y - 16 * 3};

  const auto& profileData = context.logic.cores.profileData;

  const unsigned int allTimeMaxDelta = profileData.getAllTimeMaxDelta();

  const float verticalSize =
    (std::ceil(float(allTimeMaxDelta) / k_divider)) * k_divider;

  { // background

    const glm::vec4 bgColor = allTimeMaxDelta > k_slowdownDelta
                                ? glm::vec4(1, 0, 0, 1.00f)
                                : glm::vec4(0, 0, 0, 0.75f);

    stackRenderers.triangles.pushQuad(
      glm::vec3(borderPos + borderSize * 0.5f, -0.1f), borderSize, bgColor);
    stackRenderers.wireframes.pushRectangle(borderPos, borderSize, whiteColor);

  } // background

  //
  //

  { // dividers

    for (float currDivider = k_divider; currDivider < verticalSize;
         currDivider += k_divider) {
      const float ratio = currDivider / verticalSize;

      stackRenderers.wireframes.pushLine(
        borderPos + glm::vec2(0, borderSize.y * ratio),
        borderPos + glm::vec2(borderSize.x, borderSize.y * ratio), whiteColor);
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
          borderPos + glm::vec2((statIndex + 0) * widthStep, prevHeight),
          borderPos + glm::vec2((statIndex + 1) * widthStep, currHeight),
          whiteColor);
      }
  }

  stackRenderers.wireframes.flush();
  stackRenderers.triangles.flush();
}

void CoreUsageRenderer::renderHudText() {

  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& logic = context.logic;
  auto& textRenderer = graphic.hud.textRenderer;
  auto& profileData = logic.cores.profileData;

  {
    std::stringstream sstr;

#if defined D_WEB_WEBWORKER_BUILD

    sstr << "WORKERS: " << profileData.getTotalCores();

#else

    sstr << "THREADS: " << profileData.getTotalCores();

#endif

    std::string str = sstr.str();
    textRenderer.push({position.x, position.y}, str, glm::vec4(1), 1.0f);
  }

  {

    const unsigned int totalDelta = profileData.getLatestTotalDelta();

    std::stringstream sstr;
    sstr << "CPU time: " << writeTime(totalDelta);
    std::string str = sstr.str();

    const glm::vec4 textColor =
      profileData.getAllTimeMaxDelta() > k_slowdownDelta ? glm::vec4(1, 0, 0, 1)
                                                         : glm::vec4(1, 1, 1, 1);

    textRenderer.push({position.x, position.y - 16}, str, textColor, 1.0f);
  }
}
