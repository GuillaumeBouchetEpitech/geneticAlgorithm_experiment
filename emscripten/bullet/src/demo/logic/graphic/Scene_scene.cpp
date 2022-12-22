
#include "Scene.hpp"

#include "framework/graphic/GlContext.hpp"

void Scene::_renderLeadingCarSensors() {
  auto& context = Context::get();
  const auto& leaderCar = context.logic.leaderCar;
  auto& stackRenderer = context.graphic.scene.stackRenderers.wireframes;

  if (auto leaderCarData = leaderCar.leaderData()) {
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
    for (const auto& sensor : leaderCarData->eyeSensors) {
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

void Scene::renderScene(const Camera& inCamera) {
  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& scene = graphic.scene;

  {
    const auto& matriceData = inCamera.getMatricesData();

    scene.stackRenderers.wireframes.setMatricesData(matriceData);
    scene.stackRenderers.triangles.setMatricesData(matriceData);
    scene.particleManager.setMatricesData(matriceData);
    scene.floorRenderer.setMatricesData(matriceData);
    scene.animatedCircuitRenderer.setMatricesData(matriceData);
    scene.flockingManager.setMatricesData(matriceData);
    scene.carTailsRenderer.setMatricesData(matriceData);
  }

  {
    scene.backGroundTorusRenderer.render(inCamera);
    GlContext::clear(asValue(GlContext::Buffers::depth));

    scene.floorRenderer.render();

    scene.animatedCircuitRenderer.renderWireframe();
    scene.animatedCircuitRenderer.renderWalls();

    Scene::_renderLeadingCarSensors();

    scene.stackRenderers.wireframes.flush();
    scene.stackRenderers.triangles.flush();

    scene.particleManager.render();

    scene.stackRenderers.wireframes.flush();
    scene.stackRenderers.triangles.flush();

    scene.modelsRenderer.render(inCamera);
    scene.animatedCircuitRenderer.renderGround();
    scene.carTailsRenderer.render();
    scene.flockingManager.render();

    scene.stackRenderers.wireframes.flush();
    scene.stackRenderers.triangles.flush();
  }
}
