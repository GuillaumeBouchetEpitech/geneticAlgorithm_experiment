
#include "ModelsRenderer.hpp"

#include "demo/logic/Data.hpp"

#include "demo/logic/graphicIds.hpp"

#include "framework/asValue.hpp"

#include "framework/graphic/ResourceManager.hpp"
#include "framework/graphic/GeometryBuilder.hpp"

#include "framework/graphic/loaders/loadObjModel.hpp"

#include "framework/math/RandomNumberGenerator.hpp"

void ModelsRenderer::initialise()
{
  _shader = ResourceManager::get().getShader(asValue(Shaders::models));

  {
    GeometryBuilder geometryBuilder;

    geometryBuilder
      .reset()
      .setShader(*_shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .setVboStride(9 * 4)
      .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
      .addVboAttribute("a_color", Geometry::AttrType::Vec3f, 3)
      // .addVboAttribute("a_normal", Geometry::AttrType::Vec3f, 6)
      .addVbo()
      .setVboAsInstanced()
      .addVboAttribute("a_offsetTransform", Geometry::AttrType::Mat4f, 0)
      .addVboAttribute("a_offsetColor", Geometry::AttrType::Vec3f, 16);

    geometryBuilder.build(_geometries.cars);
    geometryBuilder.build(_geometries.wheels);

    { // chassis geometry (instanced)

      loader::ModelVertices modelVertices;
      loader::loadObjModel(
        "./assets/model/CarNoWheels.obj",
        "./assets/model/",
        modelVertices);

      // {
      //     modelVertices.reserve(8 * 1024);

      //     auto addRectangle = [&modelVertices](
      //         const glm::vec3& center,
      //         const glm::vec3& size,
      //         const glm::vec3& color)
      //     {
      //         const glm::vec3 hsize = size * 0.5f;

      //         std::array<glm::vec3, 8> tmpVertices
      //         {{
      //             { center.x - hsize.x, center.y - hsize.y, center.z - hsize.z },
      //             { center.x + hsize.x, center.y - hsize.y, center.z - hsize.z },
      //             { center.x - hsize.x, center.y + hsize.y, center.z - hsize.z },
      //             { center.x + hsize.x, center.y + hsize.y, center.z - hsize.z },

      //             { center.x - hsize.x, center.y - hsize.y, center.z + hsize.z },
      //             { center.x + hsize.x, center.y - hsize.y, center.z + hsize.z },
      //             { center.x - hsize.x, center.y + hsize.y, center.z + hsize.z },
      //             { center.x + hsize.x, center.y + hsize.y, center.z + hsize.z },
      //         }};

      //         std::array<int, 36> indices
      //         {{
      //             0,1,2,  1,2,3, // bottom
      //             4,5,6,  5,6,7, // top

      //             1,3,5,  3,5,7, // right
      //             0,2,4,  2,4,6, // left

      //             0,1,4, 1,4,5, // back
      //             2,3,6, 3,6,7, // front
      //         }};

      //         for (int index : indices)
      //             modelVertices.push_back({ tmpVertices[index], color, glm::vec3(1) });
      //     };

      //     addRectangle({ 0, 0, 0.2f }, { 0.75f, 4, 0.5f }, glm::vec3(1));
      //     addRectangle({ 0, 0, 0.2f }, { 1.50f, 2, 0.4f }, glm::vec3(1));

      //     // addRectangle({ 0, -0.5f, 0.4f }, { 1, 1, 0.25 }, glm::vec3(1,0,0));

      //     addRectangle({ 0, -0.5f, 0.6f }, { 1, 1, 0.2 }, glm::vec3(0)); // cockpit
      // }

      _geometries.cars.updateBuffer(0, modelVertices);
      _geometries.cars.setPrimitiveCount(modelVertices.size());
    }

    { // wheel geometry (instanced)

      loader::ModelVertices modelVertices;
      loader::loadObjModel(
        "./assets/model/CarWheel.obj",
        "./assets/model/",
        modelVertices);

      // {
      //     modelVertices.reserve(8 * 1024);
      //     const int quality = 16;
      //     for (int ii = 1; ii <= quality; ii += 2)
      //     {
      //         const float prevAngle = (float(ii - 1) / quality) * M_PI * 2;
      //         const float currAngle = (float(ii % quality) / quality) * M_PI * 2;

      //         constexpr float radius = 0.5f;

      //         std::array<glm::vec3, 4> tmpVertices
      //         {{
      //             { -0.25f, std::cos(prevAngle) * radius, std::sin(prevAngle) * radius },
      //             { +0.25f, std::cos(prevAngle) * radius, std::sin(prevAngle) * radius },
      //             { -0.25f, std::cos(currAngle) * radius, std::sin(currAngle) * radius },
      //             { +0.25f, std::cos(currAngle) * radius, std::sin(currAngle) * radius },
      //         }};

      //         std::array<int, 6> indices{{ 0,1,2,  1,2,3 }};
      //         for (int index : indices)
      //             modelVertices.push_back({ tmpVertices[index], glm::vec3(1), glm::vec3(1) });
      //     }
      // }

      _geometries.wheels.updateBuffer(0, modelVertices);
      _geometries.wheels.setPrimitiveCount(modelVertices.size());
    }

  }
}

void ModelsRenderer::render(const Camera &cameraInstance)
{
  if (!_shader)
    D_THROW(std::runtime_error, "shader not setup");

  const auto& logic = Data::get().logic;
  const auto& simulation = *logic.simulation;

  const IFrustumCulling& frustumCulling = cameraInstance.getFrustumCulling();

  _shader->bind();
  _shader->setUniform("u_composedMatrix", cameraInstance.getSceneMatricsData().composed);

  unsigned int totalCars = simulation.getTotalCars();

  struct Attributes
  {
    glm::mat4   tranform;
    glm::vec3   color;

    Attributes(const glm::mat4& tranform, const glm::vec3& color)
      : tranform(tranform)
      , color(color)
    {}
  };

  std::vector<Attributes> modelsChassisMatrices;
  std::vector<Attributes> modelWheelsMatrices;

  modelsChassisMatrices.reserve(totalCars); // pre-allocate
  modelWheelsMatrices.reserve(totalCars * 4); // pre-allocate

  glm::vec3 modelHeight(0.0f, 0.0f, 0.2f);

  const glm::vec3 whiteColor(1, 1, 1);
  const glm::vec3 greenColor(0, 1, 0);
  const glm::vec3 redColor(1, 0, 0);
  const glm::vec3& leaderColor = whiteColor;
  const glm::vec3& lifeColor = greenColor;
  const glm::vec3& deathColor = redColor;

  for (unsigned int ii = 0; ii < totalCars; ++ii)
  {
    const auto& carData = simulation.getCarResult(ii);

    if (!carData.isAlive)
      continue;

    //
    // 3d clipping

    const glm::mat4 chassisTransform = glm::translate(carData.liveTransforms.chassis, modelHeight);
    const glm::vec4 carOrigin = chassisTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    if (!frustumCulling.sphereInFrustum(carOrigin, 5.0f))
      continue;

    //
    // color

    const bool isLeader = (logic.leaderCar.index == int(ii));
    const glm::vec3 targetColor = isLeader ? leaderColor : lifeColor;
    const glm::vec3 color = glm::mix(deathColor, targetColor, carData.life);

    //
    // transforms

    modelsChassisMatrices.emplace_back(chassisTransform, color);
    for (const auto& wheelTransform : carData.liveTransforms.wheels)
      modelWheelsMatrices.emplace_back(wheelTransform, whiteColor);
  }

  if (!modelsChassisMatrices.empty())
  {
    _geometries.cars.updateBuffer(1, modelsChassisMatrices);
    _geometries.cars.setInstancedCount(modelsChassisMatrices.size());
    _geometries.cars.render();
  }

  if (!modelWheelsMatrices.empty())
  {
    _geometries.wheels.updateBuffer(1, modelWheelsMatrices);
    _geometries.wheels.setInstancedCount(modelWheelsMatrices.size());
    _geometries.wheels.render();
  }
}
