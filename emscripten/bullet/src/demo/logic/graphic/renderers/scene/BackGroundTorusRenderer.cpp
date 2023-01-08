
#include "BackGroundTorusRenderer.hpp"

#include "demo/logic/Context.hpp"
#include "demo/logic/graphicIds.hpp"

#include "framework/graphic/GlContext.hpp"
#include "framework/helpers/GLMath.hpp"
#include "framework/system/asValue.hpp"
#include "framework/system/containers/static_heap_grid_array.hpp"
#include "framework/system/math/DeterministicRng.hpp"
#include "framework/system/math/constants.hpp"

constexpr float k_ringRadius = 500.0f;
constexpr float k_tubeRadius = 100.0f;

namespace {

struct RealVertex {
  glm::vec3 position;
  glm::vec2 texCoord;
};

void
generateTorusVertices(
  uint32_t ringQuality, uint32_t tubeQuality, float ringRadius,
  float tubeRadius, float startAngle, float sweepAngle,
  std::vector<RealVertex>& outVertices) {
  std::array<int, 6> indices{{0, 1, 2, 0, 2, 3}};

  struct SubVertex {
    glm::vec3 position;
    glm::vec3 normal;

    glm::vec3
    getForward(float radius) const {
      return position + normal * radius;
    }
  };

  std::vector<SubVertex> tubeSliceVertices;
  tubeSliceVertices.resize(tubeQuality);

  for (uint32_t tubeIndex = 0; tubeIndex < tubeQuality; ++tubeIndex) {
    const float coef = float(tubeIndex) / float(tubeQuality);
    const float angle = coef * math::pi2;

    const float cosVal = std::cos(angle);
    const float sinVal = std::sin(angle);

    const glm::vec3 normal = {cosVal, 0.0f, sinVal};

    tubeSliceVertices[tubeIndex].normal = normal;
    tubeSliceVertices[tubeIndex].position = normal * tubeRadius;
  }

  std::vector<glm::vec3> ringCenters;
  ringCenters.reserve(ringQuality);

  static_heap_grid_array<SubVertex> ringVertices;
  ringVertices.allocate(ringQuality, tubeQuality);

  for (uint32_t ringIndex = 0; ringIndex < ringQuality; ++ringIndex) {
    const float coef = float(ringIndex) / float(ringQuality);
    const float angle = startAngle + coef * sweepAngle;

    const float radAngle = glm::radians(angle);
    ringCenters.push_back(
      {ringRadius * std::cos(radAngle), ringRadius * std::sin(radAngle), 0});

    const glm::mat4 transformNormal =
      glm::rotate(glm::identity<glm::mat4>(), radAngle, glm::vec3(0, 0, 1));
    const glm::mat4 transformVertex =
      glm::translate(transformNormal, glm::vec3(ringRadius, 0, 0));

    for (std::size_t tubeIndex = 0; tubeIndex < std::size_t(tubeQuality);
         ++tubeIndex) {
      const SubVertex& sliceVertex = tubeSliceVertices[tubeIndex];

      SubVertex& vertex = ringVertices(ringIndex, tubeIndex);
      vertex.position = transformVertex * glm::vec4(sliceVertex.position, 1.0f);
      vertex.normal = transformNormal * glm::vec4(sliceVertex.normal, 0.0f);
    }
  }

  for (std::size_t ringIndex = 0; ringIndex + 1 < std::size_t(ringQuality);
       ++ringIndex) {
    const std::size_t indexA1 = ringIndex;
    const std::size_t indexB1 = (ringIndex + 1) % std::size_t(ringQuality);

    const float texCoordX1 = float(indexA1) / float(ringQuality) * 4.0f;
    const float texCoordX2 = float(indexB1) / float(ringQuality) * 4.0f;

    for (std::size_t tubeIndex = 0; tubeIndex < std::size_t(tubeQuality);
         ++tubeIndex) {
      const std::size_t indexA2 = tubeIndex;
      const std::size_t indexB2 = (tubeIndex + 1) % std::size_t(tubeQuality);

      const float texCoordY1 = float(indexA2) / float(tubeQuality) * 2.0f;
      const float texCoordY2 = float(tubeIndex + 1) / float(tubeQuality) * 2.0f;

      const SubVertex& vertexAA = ringVertices(indexA1, indexA2);
      const SubVertex& vertexAB = ringVertices(indexA1, indexB2);
      const SubVertex& vertexBA = ringVertices(indexB1, indexA2);
      const SubVertex& vertexBB = ringVertices(indexB1, indexB2);

      const glm::vec3 posAA = vertexAA.getForward(tubeRadius);
      const glm::vec3 posAB = vertexAB.getForward(tubeRadius);
      const glm::vec3 posBA = vertexBA.getForward(tubeRadius);
      const glm::vec3 posBB = vertexBB.getForward(tubeRadius);

      std::array<RealVertex, 4> patchVertices{{
        {posAA, {texCoordX1, texCoordY1}},
        {posAB, {texCoordX1, texCoordY2}},
        {posBB, {texCoordX2, texCoordY2}},
        {posBA, {texCoordX2, texCoordY1}},
      }};

      for (int index : indices)
        outVertices.push_back(patchVertices.at(index));
    }
  }
}
} // namespace

void
BackGroundTorusRenderer::initialise() {

  auto& resourceManager = Context::get().graphic.resourceManager;

  _shader = resourceManager.getShader(asValue(ShaderIds::backGroundTorus));

  { // generate the texture

    constexpr Texture::Quality quality = Texture::Quality::smoothedAndMipMapped;
    constexpr Texture::Pattern pattern = Texture::Pattern::repeat;

    const glm::ivec2 texSize = {32, 32};
    auto pixelsPtr =
      std::make_unique<unsigned char[]>(texSize.x * texSize.y * 4);
    unsigned char* rawPixels = pixelsPtr.get();

    DeterministicRng rng;
    rng.setSeed(0);

    for (int yy = 0; yy < texSize.y; ++yy)
      for (int xx = 0; xx < texSize.x; ++xx) {

        const int texelIndex = yy * 4 * texSize.x + xx * 4;

        const int color = rng.getRangedValue(16, 64);
        rawPixels[texelIndex + 0] = color;
        rawPixels[texelIndex + 1] = color;
        rawPixels[texelIndex + 2] = color;
        rawPixels[texelIndex + 3] = 255;
      }

    _texture.allocateBlank(texSize, quality, pattern, rawPixels);

  } // generate the texture

  {

    std::vector<RealVertex> vertices;
    vertices.reserve(2048); // pre-allocate

    constexpr uint32_t l_ringQuality = 128;
    constexpr uint32_t k_tubeQuality = 64;
    constexpr float k_startAngle = 0.0f;
    constexpr float k_sweepAngle = 360.0f;

    generateTorusVertices(
      l_ringQuality, k_tubeQuality, k_ringRadius, k_tubeRadius, k_startAngle,
      k_sweepAngle, vertices);

    auto geoDef = resourceManager.getGeometryDefinition(
      asValue(GeometryIds::backGroundTorus));

    _geometry.initialise(*_shader, geoDef);
    _geometry.updateBuffer(0, vertices);
    _geometry.setPrimitiveCount(vertices.size());
  }
}

void
BackGroundTorusRenderer::update(float elapsedTime) {
  _animationTime += elapsedTime * 0.01f;
  while (_animationTime > 1.0f)
    _animationTime -= 1.0f;
}

void
BackGroundTorusRenderer::render(const Camera& inCamera) {

  if (!_shader)
    D_THROW(std::runtime_error, "shader not setup");

  const auto& camera = Context::get().graphic.camera;
  const auto& viewportSize = camera.viewportSize;

  Camera tmpCamera;
  tmpCamera.setSize(viewportSize.x, viewportSize.y);
  tmpCamera.setPerspective(70.0f, 95.0f, 6000.0f);
  tmpCamera.lookAt(inCamera.getEye(), inCamera.getTarget(), inCamera.getUp());
  tmpCamera.computeMatrices();

  _shader->bind();

  glm::mat4 model = glm::identity<glm::mat4>();
  model = glm::translate(
    model, inCamera.getTarget() + glm::vec3(0, k_ringRadius * 1.0f, 0));
  model = glm::rotate(model, math::hpi, glm::vec3(0, 0, 1));
  model = glm::scale(model, glm::vec3(1, 2, 1));

  const glm::mat4 composed = tmpCamera.getMatricesData().composed * model;

  _shader->setUniform("u_composedMatrix", composed);
  _shader->setUniform("u_animationCoef", _animationTime);

  _texture.bind();
  _geometry.render();
}
