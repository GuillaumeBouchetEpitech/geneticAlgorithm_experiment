
#include "WireframesStackRenderer.hpp"

#include "demo/logic/Context.hpp"

#include "framework/system/asValue.hpp"
#include "framework/system/math/constants.hpp"

void WireframesStackRenderer::initialise(ShaderIds shaderId,
                                         GeometryIds geometryId) {

  auto& resourceManager = Context::get().graphic.resourceManager;

  _shader = resourceManager.getShader(asValue(shaderId));

  auto geoDef = resourceManager.getGeometryDefinition(asValue(geometryId));
  _geometry.initialise(*_shader, geoDef);
  _geometry.setPrimitiveCount(0);

  constexpr std::size_t preAllocatedSize = 1024 * 32; // 32Ko
  _vertices.reserve(preAllocatedSize);
}

void WireframesStackRenderer::setMatricesData(
  const Camera::MatricesData& matricesData) {
  _matricesData = matricesData;
}

//

void WireframesStackRenderer::pushLine(const glm::vec3& posA,
                                       const glm::vec3& posB,
                                       const glm::vec4& colorA,
                                       const glm::vec4& colorB) {
  if (_vertices.size() + 2 >= _vertices.capacity())
    flush();

  _vertices.emplace_back(posA, colorA);
  _vertices.emplace_back(posB, colorB);
}

void WireframesStackRenderer::pushLine(const glm::vec3& posA,
                                       const glm::vec3& posB,
                                       const glm::vec4& color) {

  pushLine(posA, posB, color, color);
}

void WireframesStackRenderer::pushLine(const glm::vec3& posA,
                                       const glm::vec3& posB,
                                       const glm::vec3& colorA,
                                       const glm::vec3& colorB) {
  pushLine(posA, posB, glm::vec4(colorA, 1.0f), glm::vec4(colorB, 1.0f));
}

void WireframesStackRenderer::pushLine(const glm::vec3& posA,
                                       const glm::vec3& posB,
                                       const glm::vec3& color) {
  pushLine(posA, posB, glm::vec4(color, 1.0f));
}

//

void WireframesStackRenderer::pushCross(const glm::vec3& pos,
                                        const glm::vec3& color,
                                        float halfExtent) {
  if (halfExtent <= 0)
    return;

  std::array<std::array<glm::vec3, 2>, 3> vertices{
    {{{{pos.x - halfExtent, pos.y, pos.z}, {pos.x + halfExtent, pos.y, pos.z}}},
     {{{pos.x, pos.y - halfExtent, pos.z}, {pos.x, pos.y + halfExtent, pos.z}}},
     {{{pos.x, pos.y, pos.z - halfExtent},
       {pos.x, pos.y, pos.z + halfExtent}}}}};

  for (const auto& elem : vertices)
    pushLine(elem.at(0), elem.at(1), color);
}

void WireframesStackRenderer::pushRectangle(const glm::vec2& pos,
                                            const glm::vec2& size,
                                            const glm::vec3& color,
                                            float depth /*= 0.0f*/) {
  const glm::vec2 farPos = pos + size;

  std::array<glm::vec3, 4> vertices = {{
    glm::vec3(pos, depth),
    glm::vec3(farPos.x, pos.y, depth),
    glm::vec3(farPos, depth),
    glm::vec3(pos.x, farPos.y, depth),
  }};

  WireframesStackRenderer::pushLine(vertices.at(0), vertices.at(1), color);
  WireframesStackRenderer::pushLine(vertices.at(1), vertices.at(2), color);
  WireframesStackRenderer::pushLine(vertices.at(2), vertices.at(3), color);
  WireframesStackRenderer::pushLine(vertices.at(3), vertices.at(0), color);
}

void WireframesStackRenderer::flush() {
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

std::size_t WireframesStackRenderer::getLinesCount() const {
  return _vertices.size();
}
