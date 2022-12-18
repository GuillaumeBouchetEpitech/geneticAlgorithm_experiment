
#include "NewLeaderRenderer.hpp"

#include "demo/logic/Context.hpp"

void NewLeaderRenderer::compute() {
  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& logic = context.logic;

  if (logic.leaderCar.totalTimeAsLeader() > 1.0f)
    return;

  if (auto leaderData = logic.leaderCar.leaderData()) {
    const Camera& scene = graphic.camera.main.scene;

    if (
      // we don't advertise a dead leader
      leaderData->isAlive &&
      // we don't advertise an early leader
      leaderData->fitness > 5.0f &&
      // we don't advertise a dying leader
      leaderData->groundSensor.value < 0.5f) {
      const glm::vec3 carPos =
        leaderData->liveTransforms.chassis * glm::vec4(0, 0, 0, 1);

      const bool isVisible = scene.sceneToHudCoord(carPos, _screenCoord);

      if (isVisible &&
          // out of range?
          _screenCoord.z < 1.0f) {
        _isVisible = true;
      }
    }
  }
}

void NewLeaderRenderer::renderWireframe() {
  if (!_isVisible)
    return;

  glm::vec2 textPos = {_screenCoord.x + 50, _screenCoord.y + 50};

  Context::get().graphic.hud.stackRenderers.wireframes.pushLine(
    _screenCoord, textPos, {1, 1, 1});
}

void NewLeaderRenderer::renderHudText() {
  if (!_isVisible)
    return;

  glm::vec2 textPos = {_screenCoord.x + 50, _screenCoord.y + 50};

  Context::get().graphic.hud.textRenderer.push(textPos, "NEW\nLEADER",
                                               glm::vec4(1), 1.1f);
}
