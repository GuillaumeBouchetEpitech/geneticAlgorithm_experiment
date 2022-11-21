
#include "CoreUsageRenderer.hpp"

#include "demo/logic/Data.hpp"

#include "demo/logic/graphic/helpers/writeTime.hpp"

void CoreUsageRenderer::renderWireframe()
{

  auto& data = Data::get();
  auto& graphic = data.graphic;

  auto& stackRenderer = graphic.stackRenderer;

  const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);

  const glm::vec2 borderPos = { position.x, position.y + 16 + 8 };
  const glm::vec2 borderSize = { size.x, size.y - 16  * 3 };

  const auto& cores = data.logic.cores;
  const auto& profileData = cores.profileData;

  unsigned int commonMaxDelta = 0;
  for (unsigned int ii = 0; ii < profileData.getTotalCores(); ++ii)
    for (unsigned int jj = 0; jj < profileData.getMaxStateHistory(); ++jj)
      commonMaxDelta = std::max(commonMaxDelta, profileData.getCoreHistoryData(ii, jj).delta);

  constexpr float divider = 5000.0f; // 5ms
  const float maxVal = (std::ceil(float(commonMaxDelta) / divider)) * divider;

  { // background

    glm::vec4 bgColor = commonMaxDelta > 16000 ? glm::vec4(1,0,0, 1.00f) : glm::vec4(0,0,0, 0.75f);

    stackRenderer.pushQuad(glm::vec3(borderPos + borderSize * 0.5f, -0.1f), borderSize, bgColor);
    stackRenderer.pushRectangle(borderPos, borderSize, whiteColor);

  } // background

  //
  //

  { // dividers

    for (float ii = divider; ii < maxVal; ii += divider)
    {
      const float coef = ii / maxVal;

      const glm::vec2 coordA(0, borderSize.y * coef);
      const glm::vec2 coordB(borderSize.x, borderSize.y * coef);

      stackRenderer.pushLine(borderPos + coordA, borderPos + coordB, whiteColor);
    }

  } // dividers

  //
  //

  for (std::size_t coreIndex = 0; coreIndex < profileData.getTotalCores(); ++coreIndex)
  {
    const float widthStep = borderSize.x / profileData.getMaxStateHistory();

    for (unsigned int ii = 0; ii + 1 < profileData.getMaxStateHistory(); ++ii)
    {
      const auto& prevState = profileData.getCoreHistoryData(coreIndex, ii);
      const auto& currState = profileData.getCoreHistoryData(coreIndex, ii + 1);

      //

      const float prevRatio = float(prevState.delta) / maxVal;
      const float currRatio = float(currState.delta) / maxVal;

      const glm::vec2 prevCoord(ii * widthStep, borderSize.y * prevRatio);
      const glm::vec2 currCoord((ii + 1) * widthStep, borderSize.y * currRatio);

      stackRenderer.pushLine(borderPos + prevCoord, borderPos + currCoord, whiteColor);
    }
  }

  stackRenderer.flush();
}

void CoreUsageRenderer::renderHudText()
{

  auto& data = Data::get();
  auto& graphic = data.graphic;
  auto& logic = data.logic;
  auto& textRenderer = graphic.textRenderer;

  {
    std::stringstream sstr;

#if defined D_WEB_WEBWORKER_BUILD

    sstr << "WORKERS: " << logic.cores.profileData.getTotalCores() << std::endl;

#else

    sstr << "THREADS: " << logic.cores.profileData.getTotalCores() << std::endl;

#endif

    std::string str = sstr.str();
    textRenderer.push({ position.x, position.y }, str, glm::vec3(1), 1.0f);
  }

  {

    unsigned int globalDelta = 0;
    for (std::size_t ii = 0; ii < logic.cores.profileData.getTotalCores(); ++ii)
    {
      const auto& coreState = logic.cores.profileData.getCoreData(ii);

      globalDelta += coreState.delta;
    }

    std::stringstream sstr;

    sstr << "Total: " << writeTime(globalDelta);

    std::string str = sstr.str();

    unsigned int commonMaxDelta = 0;
    const auto& profileData = logic.cores.profileData;
    for (unsigned int ii = 0; ii < profileData.getTotalCores(); ++ii)
      for (unsigned int jj = 0; jj < profileData.getMaxStateHistory(); ++jj)
        commonMaxDelta = std::max(commonMaxDelta, profileData.getCoreHistoryData(ii, jj).delta);

    glm::vec3 textColor = commonMaxDelta > 16000 ? glm::vec3(1,0,0) : glm::vec3(1,1,1);

    textRenderer.push({ position.x, position.y - 16 }, str, textColor, 1.0f);
  }

}
