
#include "NewLeaderRenderer.hpp"

#include "demo/logic/Data.hpp"

void NewLeaderRenderer::compute()
{
  auto& data = Data::get();
  auto& graphic = data.graphic;
  auto& logic = data.logic;

  if (logic.leaderCar.totalTimeAsLeader() > 1.0f)
      return;

  if (auto leaderData = logic.leaderCar.leaderData())
  {
    const Camera& scene = graphic.camera.scene.instance;

    if (
      // we don't advertise a dead leader
      leaderData->isAlive &&
      // we don't advertise an early leader
      leaderData->fitness > 5.0f &&
      // we don't advertise a dying leader
      leaderData->groundSensor.value < 0.5f)
    {
      const glm::vec3 carPos = leaderData->liveTransforms.chassis * glm::vec4(0, 0, 0, 1);

      const bool isVisible = scene.sceneToHudCoord(carPos, _screenCoord);

      if (
        isVisible &&
        // out of range?
        _screenCoord.z < 1.0f)
      {
        _isVisible = true;
      }
    }
  }
}

void NewLeaderRenderer::renderWireframe()
{
  if (!_isVisible)
      return;

  auto& data = Data::get();
  auto& graphic = data.graphic;

  glm::vec2 textPos = { _screenCoord.x + 50, _screenCoord.y + 50 };

  graphic.stackRenderer.pushLine(_screenCoord, textPos, {1, 1, 1});
}

void NewLeaderRenderer::renderHudText()
{
  if (!_isVisible)
      return;

  auto& data = Data::get();
  auto& graphic = data.graphic;
  auto& textRenderer = graphic.textRenderer;

  glm::vec2 textPos = { _screenCoord.x + 50, _screenCoord.y + 50 };

  textRenderer.push(textPos, "NEW\nLEADER", glm::vec3(1), 1.1f);
}
