
#include "Scene.hpp"

#include "framework/graphic/GlContext.hpp"

void Scene::_renderLeadingCarSensors()
{
  auto&       data = Data::get();
  const auto& leaderCar = data.logic.leaderCar;
  auto&       stackRenderer = data.graphic.stackRenderer;

  if (auto leaderCarData = leaderCar.leaderData())
  {
    // leading car alive?
    if (!leaderCarData->isAlive)
      return;

    // do not show the sensor until far enough
    if (leaderCarData->groundIndex < 2)
      return;

    const glm::vec3 greenColor(0.0f, 1.0f, 0.0f);
    const glm::vec3 yellowColor(1.0f, 1.0f, 0.0f);
    const glm::vec3 orangeColor(1.0f, 0.5f, 0.0f);
    const glm::vec3 redColor(1.0f, 0.0f, 0.0f);
    const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);

    // eye sensors
    for (const auto& sensor : leaderCarData->eyeSensors)
    {
      constexpr float redThreshold = 1.0f / 8.0f;
      constexpr float orangeThreshold = 1.0f / 4.0f;
      constexpr float yellowThreshold = 1.0f / 2.0f;

      glm::vec3 color = greenColor;
      if (sensor.value < redThreshold)
        color = redColor;
      else if (sensor.value < orangeThreshold)
        color = orangeColor;
      else if (sensor.value < yellowThreshold)
        color = yellowColor;

      stackRenderer.pushLine(sensor.near, sensor.far, color);
      stackRenderer.pushCross(sensor.far, color, 1.0f);
    }

    // ground sensor
    const auto& groundSensor = leaderCarData->groundSensor;
    stackRenderer.pushLine(groundSensor.near, groundSensor.far, whiteColor);
    stackRenderer.pushCross(groundSensor.far, whiteColor, 1.0f);
  }
}

void Scene::_renderFloor(const Camera& camera)
{
  auto& graphic = Data::get().graphic;

  // hide the floor if the camera is looking from beneath it
  GlContext::enable(GlContext::States::cullFace);

  // transparency friendly
  GlContext::disable(GlContext::States::depthTest);

  graphic.backGroundCylindersRenderer.render(camera.getTarget());
  graphic.floorRenderer.render();

  GlContext::disable(GlContext::States::cullFace);
  GlContext::enable(GlContext::States::depthTest);
}
