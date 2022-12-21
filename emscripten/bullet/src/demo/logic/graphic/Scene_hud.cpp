
#include "Scene.hpp"

#include "demo/states/StateManager.hpp"

#include "renderers/hud/NewLeaderRenderer.hpp"

#include "framework/graphic/GlContext.hpp"
#include "framework/system/TraceLogger.hpp"
#include "framework/system/asValue.hpp"
#include "framework/system/math/clamp.hpp"

#include <iomanip>
#include <sstream>

void Scene::_renderHUD_ortho() {

  auto& graphic = Context::get().graphic;
  auto& stackRenderer = graphic.hud.stackRenderers;
  auto& textRenderer = graphic.hud.textRenderer;
  const auto& vSize = graphic.camera.viewportSize;

  textRenderer.clear();

  {
    NewLeaderRenderer newLeaderRndr;
    newLeaderRndr.compute();

    newLeaderRndr.renderHudText();
    graphic.hud.fitnessDataRenderer.renderHudText();
    graphic.hud.coreUsageRenderer.renderHudText();

    newLeaderRndr.renderWireframe();
    graphic.hud.informationTextRenderer.render();
    graphic.hud.coreUsageRenderer.renderWireframe();
    graphic.hud.fitnessDataRenderer.renderWireframe();
    graphic.hud.topologyRenderer.render();
    graphic.hud.leaderEyeRenderer.render();
    graphic.hud.screenTitles.render();

  } // wireframes

  { // big titles

    StateManager::States currentState = StateManager::get()->getState();

#if defined D_WEB_WEBWORKER_BUILD
    if (currentState == StateManager::States::WorkersLoading) {
      float scale = 2.0f;

      std::stringstream sstr;
      sstr << "WEB WORKERS" << std::endl << "  LOADING  " << std::endl;
      std::string message = sstr.str();

      glm::vec2 textPos;
      textPos.x = vSize.x * 0.5f - 5 * 16 * scale;
      textPos.y = vSize.y * 0.5f - 8 * scale;

      textRenderer.push(textPos, message, glm::vec4(1), scale, 0.75f);
    }
#endif

    if (currentState == StateManager::States::Paused) {
      float scale = 5.0f;

      std::string message = "PAUSED";

      glm::vec2 textPos;
      textPos.x = vSize.x * 0.5f - float(message.size()) / 2 * 16 * scale;
      textPos.y = vSize.y * 0.5f - 8 * scale;

      textRenderer.push(textPos, message, glm::vec4(1), scale, 0.75f);
    }

  } // big titles

  stackRenderer.wireframes.flush();
  stackRenderer.triangles.flush();
  textRenderer.render();
}

void Scene::_renderHUD() {
  auto& context = Context::get();
  auto& graphic = context.graphic;

  GlContext::enable(GlContext::States::depthTest);
  GlContext::disable(GlContext::States::cullFace);

  graphic.hud.postProcess.render();

  Scene::_renderHUD_ortho();
  graphic.hud.thirdPersonCamera.render();
}
