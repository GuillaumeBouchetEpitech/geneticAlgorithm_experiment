
#include "ModelsRenderer.hpp"

#include "demo/logic/Context.hpp"

#include "demo/logic/graphicIds.hpp"

#include "framework/asValue.hpp"

#include "framework/graphic/ResourceManager.hpp"
#include "framework/graphic/GeometryBuilder.hpp"

#include "framework/graphic/loaders/loadObjModel.hpp"

#include "framework/math/RandomNumberGenerator.hpp"


namespace
{

  void updateVerticesNormals(loader::ModelVertices& vertices)
  {
    for (std::size_t index = 0; index < vertices.size(); index += 3)
    {
      loader::ModelVertex& vertexA = vertices.at(index + 0);
      loader::ModelVertex& vertexB = vertices.at(index + 1);
      loader::ModelVertex& vertexC = vertices.at(index + 2);

      const glm::vec3 normal = glm::cross(
        vertexA.position - vertexB.position,
        vertexA.position - vertexC.position);

      vertexA.normal = normal;
      vertexB.normal = normal;
      vertexC.normal = normal;
    }
  }

}

void ModelsRenderer::initialise()
{
  _shader = Context::get().graphic.resourceManager.getShader(asValue(Shaders::models));

  {
    GeometryBuilder geometryBuilder;

    geometryBuilder
      .reset()
      .setShader(*_shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f, 0)
      .addVboAttribute("a_vertex_color", Geometry::AttrType::Vec3f, 3)
      .addVboAttribute("a_vertex_normal", Geometry::AttrType::Vec3f, 6)
      .addVbo()
      .setVboAsInstanced()
      .addVboAttribute("a_offset_transform", Geometry::AttrType::Mat4f, 0)
      .addVboAttribute("a_offset_color", Geometry::AttrType::Vec3f, 16);

    geometryBuilder.build(_geometries.cars);
    geometryBuilder.build(_geometries.wheels);

    { // chassis geometry (instanced)

      loader::ModelVertices modelVertices;
      loader::loadObjModel(
        "./assets/model/CarNoWheels.obj",
        "./assets/model/",
        modelVertices);

      updateVerticesNormals(modelVertices);

      _geometries.cars.updateBuffer(0, modelVertices);
      _geometries.cars.setPrimitiveCount(modelVertices.size());
    }

    { // wheel geometry (instanced)

      loader::ModelVertices modelVertices;
      loader::loadObjModel(
        "./assets/model/CarWheel.obj",
        "./assets/model/",
        modelVertices);

      updateVerticesNormals(modelVertices);

      _geometries.wheels.updateBuffer(0, modelVertices);
      _geometries.wheels.setPrimitiveCount(modelVertices.size());
    }

  }
}

void ModelsRenderer::render(const Camera &cameraInstance)
{
  if (!_shader)
    D_THROW(std::runtime_error, "shader not setup");

  const auto& logic = Context::get().logic;
  const auto& simulation = *logic.simulation;

  const unsigned int totalCars = simulation.getTotalCars();
  if (totalCars == 0)
    return;

  const IFrustumCulling& frustumCulling = cameraInstance.getFrustumCulling();

  const auto& matricesData = cameraInstance.getMatricesData();

  _shader->bind();
  _shader->setUniform("u_projectionMatrix", matricesData.projection);
  _shader->setUniform("u_modelViewMatrix", matricesData.view);

  _modelsChassisMatrices.clear();
  _modelWheelsMatrices.clear();
  _modelsChassisMatrices.reserve(totalCars); // pre-allocate
  _modelWheelsMatrices.reserve(totalCars * 4); // pre-allocate

  const glm::vec3 modelHeight(0.0f, 0.0f, 0.2f);

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

    const bool isLeader = (logic.leaderCar.leaderIndex() == int(ii));
    const glm::vec3 targetColor = isLeader ? leaderColor : lifeColor;
    const glm::vec3 color = glm::mix(deathColor, targetColor, carData.life);

    //
    // transforms

    _modelsChassisMatrices.emplace_back(chassisTransform, color);
    for (const auto& wheelTransform : carData.liveTransforms.wheels)
      _modelWheelsMatrices.emplace_back(wheelTransform, whiteColor);
  }

  if (!_modelsChassisMatrices.empty())
  {
    _geometries.cars.updateBuffer(1, _modelsChassisMatrices);
    _geometries.cars.setInstancedCount(_modelsChassisMatrices.size());
    _geometries.cars.render();
  }

  if (!_modelWheelsMatrices.empty())
  {
    _geometries.wheels.updateBuffer(1, _modelWheelsMatrices);
    _geometries.wheels.setInstancedCount(_modelWheelsMatrices.size());
    _geometries.wheels.render();
  }
}
