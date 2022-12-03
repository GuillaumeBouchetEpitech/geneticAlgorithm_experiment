
#include "AnimatedCircuitRenderer.hpp"

#include "demo/logic/graphicIds.hpp"

#include "framework/asValue.hpp"

#include "framework/graphic/ResourceManager.hpp"
#include "framework/graphic/GeometryBuilder.hpp"

#include "framework/graphic/GlContext.hpp"

#include "framework/helpers/GLMath.hpp"

#include "demo/logic/Data.hpp"

void AnimatedCircuitRenderer::initialise(
    const std::vector<glm::vec3>& skeletonVertices,
    const AnimatedVertices& groundVertices,
    const AnimatedVertices& wallsVertices,
    float maxUpperValue)
{
  _maxUpperValue = maxUpperValue;
  _shaderWireframe = Data::get().graphic.resourceManager.getShader(asValue(Shaders::wireframes));
  _shaderCircuitLit = Data::get().graphic.resourceManager.getShader(asValue(Shaders::animatedCircuitLit));
  _shaderCircuit = Data::get().graphic.resourceManager.getShader(asValue(Shaders::animatedCircuit));

  GeometryBuilder geometryBuilder;

  { // compute circuit skeleton wireframe geometry

    geometryBuilder
      .reset()
      .setShader(*_shaderWireframe)
      .setPrimitiveType(Geometry::PrimitiveType::lines)
      .addVbo()
      .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
      .build(_geometries.skeleton);

    _geometries.skeleton.updateBuffer(0, skeletonVertices);
    _geometries.skeleton.setPrimitiveCount(skeletonVertices.size());

  } // compute circuit skeleton wireframe geometry

  { // compute circuit ground geometries

    geometryBuilder
      .reset()
      .setShader(*_shaderCircuitLit)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
      .addVboAttribute("a_color", Geometry::AttrType::Vec3f, 3)
      .addVboAttribute("a_normal", Geometry::AttrType::Vec3f, 6)
      .addVboAttribute("a_animated_normal", Geometry::AttrType::Vec3f, 9)
      .addVboAttribute("a_index", Geometry::AttrType::Float, 12);

    geometryBuilder.build(_geometries.grounds);
    _geometries.grounds.updateBuffer(0, groundVertices);
    _geometries.grounds.setPrimitiveCount(groundVertices.size());

  } // compute circuit ground geometries

  { // compute circuit walls geometries

    geometryBuilder
      .reset()
      .setShader(*_shaderCircuit)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .setVboStride(13 * 4)
      .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
      .addVboAttribute("a_color", Geometry::AttrType::Vec3f, 3)
      .addVboAttribute("a_animated_normal", Geometry::AttrType::Vec3f, 9)
      .addVboAttribute("a_index", Geometry::AttrType::Float, 12);

    geometryBuilder.build(_geometries.walls);
    _geometries.walls.updateBuffer(0, wallsVertices);
    _geometries.walls.setPrimitiveCount(wallsVertices.size());

  } // compute circuit walls geometries

  _maxPrimitiveCount = groundVertices.size();

}

void AnimatedCircuitRenderer::setMatricesData(const Camera::MatricesData& matricesData)
{
  _matricesData = matricesData;
}

void AnimatedCircuitRenderer::update(float elapsedTime)
{
  auto& logic = Data::get().logic;
  const auto& simulation = *logic.simulation;

  if (logic.isAccelerated)
  {
    _targetValue = _maxUpperValue;
    _lowerValue = _maxUpperValue;
    _upperValue = _maxUpperValue;
  }
  else
  {
    _targetValue = 3.0f; // <= default value

    int bestGroundIndex = -1;
    for (unsigned int ii = 0; ii < simulation.getTotalCars(); ++ii)
    {
      const auto& carData = simulation.getCarResult(ii);

      if (!carData.isAlive || bestGroundIndex > carData.groundIndex)
        continue;

      bestGroundIndex = carData.groundIndex;
    }

    // do we have a car to focus the camera on?
    if (bestGroundIndex >= 0)
      _targetValue += bestGroundIndex;

    // lower value, closest from the cars

    if (_lowerValue > _targetValue + 10.0f)
    {
      // fall really quickly
      _lowerValue -= 60.0f * elapsedTime;
      if (_lowerValue < _targetValue)
        _lowerValue = _targetValue;
    }
    else if (_lowerValue > _targetValue)
    {
      // fall quickly
      _lowerValue -= 18.0f * elapsedTime;
      if (_lowerValue < _targetValue)
        _lowerValue = _targetValue;
    }
    else
    {
      // rise slowly
      _lowerValue += 12.0f * elapsedTime;
      if (_lowerValue > _targetValue)
        _lowerValue = _targetValue;
    }

    // upper value, farthest from the cars

    if (_upperValue > _targetValue + 10.0f)
    {
      // fall really quickly
      _upperValue -= 36.0f * elapsedTime;
      if (_upperValue < _targetValue)
        _upperValue = _targetValue;
    }
    else if (_upperValue > _targetValue)
    {
      // fall slowly
      _upperValue -= 6.0f * elapsedTime;
      if (_upperValue < _targetValue)
        _upperValue = _targetValue;
    }
    else
    {
      // rise really quickly
      _upperValue += 60.0f * elapsedTime;
      if (_upperValue > _targetValue)
        _upperValue = _targetValue;
    }

  }

  const unsigned int verticesLength = 36; // <= 3 * 12 triangles
  int indexValue = std::ceil(_upperValue) * verticesLength;
  if (indexValue > int(_maxPrimitiveCount))
    indexValue = int(_maxPrimitiveCount);

  _geometries.grounds.setPrimitiveCount(indexValue);
  _geometries.walls.setPrimitiveCount(indexValue * 2); // <= 2 walls
}

void AnimatedCircuitRenderer::render()
{
  if (!_shaderWireframe || !_shaderCircuit)
    D_THROW(std::runtime_error, "shader not setup");

  { // circuit skeleton

    _shaderWireframe->bind();
    _shaderWireframe->setUniform("u_composedMatrix", _matricesData.composed);
    _shaderWireframe->setUniform("u_color", 0.6f, 0.6f, 0.6f, 1.0f);

    _geometries.skeleton.render();

  } // circuit skeleton

  GlContext::disable(GlContext::States::depthTest);

  {

    _shaderCircuitLit->bind();
    _shaderCircuitLit->setUniform("u_projectionMatrix", _matricesData.projection);
    _shaderCircuitLit->setUniform("u_modelViewMatrix", _matricesData.view);
    _shaderCircuitLit->setUniform("u_lowerLimit", _lowerValue);
    _shaderCircuitLit->setUniform("u_upperLimit", _upperValue);
    _shaderCircuitLit->setUniform("u_alpha", 0.8f);

    _geometries.grounds.render();

  }

  {

    _shaderCircuit->bind();
    _shaderCircuit->setUniform("u_composedMatrix", _matricesData.composed);
    _shaderCircuit->setUniform("u_lowerLimit", _lowerValue);
    _shaderCircuit->setUniform("u_upperLimit", _upperValue);
    _shaderCircuit->setUniform("u_alpha", 0.8f);

    _shaderCircuit->setUniform("u_alpha", 0.2f);

    _geometries.walls.render();

  }

  GlContext::enable(GlContext::States::depthTest);

}
