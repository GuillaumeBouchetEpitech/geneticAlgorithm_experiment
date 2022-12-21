
#include "TrianglesStackRenderer.hpp"

#include "demo/logic/Context.hpp"

#include "framework/system/asValue.hpp"
#include "framework/system/math/constants.hpp"

void TrianglesStackRenderer::initialise(ShaderIds shaderId,
                                        GeometryIds geometryId) {

  auto& resourceManager = Context::get().graphic.resourceManager;

  _shader = resourceManager.getShader(asValue(shaderId));

  auto geoDef = resourceManager.getGeometryDefinition(asValue(geometryId));
  _geometry.initialise(*_shader, geoDef);
  _geometry.setPrimitiveCount(0);

  constexpr std::size_t preAllocatedSize = 1024 * 32; // 32Ko
  _vertices.reserve(preAllocatedSize);
}

void TrianglesStackRenderer::setMatricesData(
  const Camera::MatricesData& matricesData) {
  _matricesData = matricesData;
}

std::size_t TrianglesStackRenderer::getTrianglesCount() const {
  return _vertices.size();
}

void TrianglesStackRenderer::pushTriangle(
  const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& posC,
  const glm::vec4& colorA, const glm::vec4& colorB, const glm::vec4& colorC) {
  if (_vertices.size() + 3 >= _vertices.capacity())
    flush();

  _vertices.emplace_back(posA, colorA);
  _vertices.emplace_back(posB, colorB);
  _vertices.emplace_back(posC, colorC);
}

void TrianglesStackRenderer::pushTriangle(const glm::vec3& posA,
                                          const glm::vec3& posB,
                                          const glm::vec3& posC,
                                          const glm::vec4& color) {
  pushTriangle(posA, posB, posC, color, color, color);
}

void TrianglesStackRenderer::flush() {
  if (_vertices.empty())
    return;

  if (!_shader)
    D_THROW(std::runtime_error, "shader not setup");

  _shader->bind();
  _shader->setUniform("u_composedMatrix", _matricesData.composed);

  _geometry.updateBuffer(0, _vertices, true);
  _geometry.setPrimitiveCount(uint32_t(_vertices.size()));
  _geometry.render();

  _vertices.clear();
}

void TrianglesStackRenderer::pushThickTriangle2dLine(
  const glm::vec2& posA, const glm::vec2& posB, float thicknessA,
  float thicknessB, const glm::vec4& colorA, const glm::vec4& colorB, float z) {
  const float angle = std::atan2(posB.y - posA.y, posB.x - posA.x) + math::hpi;

  const glm::vec2 direction = {std::cos(angle), std::sin(angle)};
  const glm::vec2 stepA = direction * thicknessA * 0.5f;
  const glm::vec2 stepB = direction * thicknessB * 0.5f;

  std::array<glm::vec3, 4> vertices{{
    {posA.x - stepA.x, posA.y - stepA.y, z},
    {posA.x + stepA.x, posA.y + stepA.y, z},
    {posB.x - stepB.x, posB.y - stepB.y, z},
    {posB.x + stepB.x, posB.y + stepB.y, z},
  }};

  pushTriangle(vertices.at(0), vertices.at(3), vertices.at(2), colorA, colorB,
               colorB);

  pushTriangle(vertices.at(0), vertices.at(1), vertices.at(3), colorA, colorA,
               colorB);
}

void TrianglesStackRenderer::pushQuad(const glm::vec2& center,
                                      const glm::vec2& size,
                                      const glm::vec4& color, float z) {
  const glm::vec2 hsize = size * 0.5f;

  const std::array<glm::vec3, 4> vertices = {{
    {center.x + hsize.x, center.y + hsize.y, z},
    {center.x - hsize.x, center.y + hsize.y, z},
    {center.x + hsize.x, center.y - hsize.y, z},
    {center.x - hsize.x, center.y - hsize.y, z},
  }};

  // std::array<glm::uvec3, 2> indices = {{{0, 1, 2}, {2, 1, 3}}};
  std::array<glm::uvec3, 2> indices = {{{1, 0, 2}, {2, 1, 3}}};

  for (const glm::uvec3& index : indices)
    pushTriangle(vertices.at(index[0]), vertices.at(index[1]),
                 vertices.at(index[2]), color);
}

void TrianglesStackRenderer::pushQuad(const glm::vec2& center,
                                      const glm::vec2& size,
                                      const glm::vec4& color) {
  pushQuad(center, size, color, 0.0f);
}

void TrianglesStackRenderer::pushCircle(const glm::vec2& center, float radius,
                                        const glm::vec4& color, float z) {
  constexpr int quality = 16;

  std::array<glm::vec2, quality> cachedValues;

  for (int ii = 0; ii < quality; ++ii) {
    const float coef = float(ii) / quality;

    cachedValues.at(ii).x = std::cos(coef * math::pi2);
    cachedValues.at(ii).y = std::sin(coef * math::pi2);
  }

  for (int ii = 0; ii < quality; ++ii) {
    const int indexA = ii;
    const int indexB = (ii + 1) % quality;

    glm::vec3 vertexA = {center.x + radius * cachedValues.at(indexA).x,
                         center.y + radius * cachedValues.at(indexA).y, z};

    glm::vec3 vertexB = {center.x + radius * cachedValues.at(indexB).x,
                         center.y + radius * cachedValues.at(indexB).y, z};

    pushTriangle(glm::vec3(center, z), vertexA, vertexB, color);
  }
}

void TrianglesStackRenderer::pushCircle(const glm::vec2& center, float radius,
                                        const glm::vec4& color) {
  pushCircle(center, radius, color, 0.0f);
}

void TrianglesStackRenderer::pushThickTriangle2dLine(
  const glm::vec2& posA, const glm::vec2& posB, float thicknessA,
  float thicknessB, const glm::vec4& colorA, const glm::vec4& colorB) {
  pushThickTriangle2dLine(posA, posB, thicknessA, thicknessB, colorA, colorB,
                          0.0f);
}

void TrianglesStackRenderer::pushThickTriangle2dLine(const glm::vec2& posA,
                                                     const glm::vec2& posB,
                                                     float thickness,
                                                     const glm::vec4& color,
                                                     float z) {
  pushThickTriangle2dLine(posA, posB, thickness, thickness, color, color, z);
}

void TrianglesStackRenderer::pushThickTriangle2dLine(const glm::vec2& posA,
                                                     const glm::vec2& posB,
                                                     float thickness,
                                                     const glm::vec4& color) {
  pushThickTriangle2dLine(posA, posB, thickness, thickness, color, color, 0.0f);
}

void TrianglesStackRenderer::pushThickTriangle3dLine(
  const glm::vec3& posA, const glm::vec3& posB, float thicknessA,
  float thicknessB, const glm::vec4& colorA, const glm::vec4& colorB) {
  const glm::vec3 diff = posB - posA;
  const float horizontalAngle = std::atan2(diff.y, diff.x);
  const float verticalAngle =
    std::atan2(diff.z, glm::length(glm::vec2(diff.x, diff.y)));

  const glm::vec2 direction = {std::cos(horizontalAngle),
                               std::sin(horizontalAngle)};

  const float verticalUpAngle = verticalAngle - math::hpi;
  const float upRadius = std::cos(verticalUpAngle);
  const glm::vec3 upAxis = {
    std::sin(verticalUpAngle),
    upRadius * direction.x,
    upRadius * direction.y,
  };

  const float forwardRadius = std::cos(verticalAngle);
  const glm::vec3 forwardAxis = {
    std::sin(verticalAngle),
    forwardRadius * direction.x,
    forwardRadius * direction.y,
  };

  const glm::vec3 leftAxis = {
    upAxis.y * forwardAxis.z - upAxis.z * forwardAxis.y,
    upAxis.z * forwardAxis.x - upAxis.x * forwardAxis.z,
    upAxis.x * forwardAxis.y - upAxis.y * forwardAxis.x,
  };

  const std::array<glm::vec3, 3> sideA = {
    {posA - leftAxis * thicknessA - upAxis * thicknessA,
     posA + leftAxis * thicknessA - upAxis * thicknessA,
     posA + upAxis * thicknessA}};

  const std::array<glm::vec3, 3> sideB = {
    {posB - leftAxis * thicknessB - upAxis * thicknessB,
     posB + leftAxis * thicknessB - upAxis * thicknessB,
     posB + upAxis * thicknessB}};

  struct Vertex {
    const glm::vec3& position;
    const glm::vec4& color;
  };

  std::array<std::array<Vertex, 4>, 3> allQuads{{
    {{{sideA.at(0), colorA},
      {sideA.at(1), colorA},
      {sideB.at(0), colorB},
      {sideB.at(1), colorB}}},
    {{{sideA.at(0), colorA},
      {sideA.at(2), colorA},
      {sideB.at(0), colorB},
      {sideB.at(2), colorB}}},
    {{{sideA.at(1), colorA},
      {sideA.at(2), colorA},
      {sideB.at(1), colorB},
      {sideB.at(2), colorB}}},
  }};

  std::array<glm::uvec3, 2> indices = {{{0, 3, 2}, {0, 1, 3}}};

  for (auto& quad : allQuads)
    for (const glm::uvec3& index : indices)
      pushTriangle(quad.at(index[0]).position, quad.at(index[1]).position,
                   quad.at(index[2]).position, quad.at(index[0]).color,
                   quad.at(index[1]).color, quad.at(index[2]).color);
}

void TrianglesStackRenderer::pushThickTriangle3dLine(const glm::vec3& posA,
                                                     const glm::vec3& posB,
                                                     float thickness,
                                                     const glm::vec4& color) {
  pushThickTriangle3dLine(posA, posB, thickness, thickness, color, color);
}
