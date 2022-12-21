
#include "FloorRenderer.hpp"

#include "demo/logic/Context.hpp"
#include "demo/logic/graphicIds.hpp"

#include "framework/graphic/GlContext.hpp"
#include "framework/helpers/GLMath.hpp"
#include "framework/system/asValue.hpp"

void FloorRenderer::initialise(const glm::vec3& center, const glm::vec3& size) {

  auto& resourceManager = Context::get().graphic.resourceManager;

  _shader = resourceManager.getShader(asValue(ShaderIds::simpleTextureLit));

  auto geoDef = resourceManager.getGeometryDefinition(
    asValue(GeometryIds::simpleTextureLit));
  _geometry.initialise(*_shader, geoDef);
  _geometry.setPrimitiveCount(0);

  {

    const glm::ivec2 size = {512, 512};
    auto pixelsPtr = std::make_unique<unsigned char[]>(size.x * size.y * 4);
    unsigned char* rawPixels = pixelsPtr.get();

    const auto setPixel = [&size, rawPixels](int x, int y, unsigned char grey,
                                             unsigned char alpha) {
      rawPixels[y * 4 * size.x + x * 4 + 0] = grey;
      rawPixels[y * 4 * size.x + x * 4 + 1] = grey;
      rawPixels[y * 4 * size.x + x * 4 + 2] = grey;
      rawPixels[y * 4 * size.x + x * 4 + 3] = alpha;
    };

    for (int yy = 0; yy < size.y; ++yy)
      for (int xx = 0; xx < size.x; ++xx) {
        if ((xx < size.x * 0.5f && yy < size.y * 0.5f) ||
            (xx > size.x * 0.5f && yy > size.y * 0.5f)) {
          setPixel(xx, yy, 32, 220);
        } else {
          setPixel(xx, yy, 100, 255);
        }
      }

    _texture.allocateBlank(size, Texture::Quality::smoothed,
                           Texture::Pattern::repeat, rawPixels);
  }

  { // compute chessboard ground

    struct Vertex {
      glm::vec3 position;
      glm::vec3 normal;
      glm::vec2 texCoord;
    };

    glm::vec3 boardHSize = glm::ceil(size * 0.55f / 100.55f) * 100.0f;
    glm::vec3 boardPos = center;
    glm::vec3 texCoordSize = boardHSize / 100.0f;
    constexpr float boardHeight = -0.1f;

    const glm::vec3 normal = {0, 0, 1};

    std::array<Vertex, 4> quadVertices{{
      {{boardPos.x + boardHSize.x, boardPos.y - boardHSize.y, boardHeight},
       normal,
       {+texCoordSize.x, -texCoordSize.y}},
      {{boardPos.x - boardHSize.x, boardPos.y - boardHSize.y, boardHeight},
       normal,
       {-texCoordSize.x, -texCoordSize.y}},
      {{boardPos.x + boardHSize.x, boardPos.y + boardHSize.y, boardHeight},
       normal,
       {+texCoordSize.x, +texCoordSize.y}},
      {{boardPos.x - boardHSize.x, boardPos.y + boardHSize.y, boardHeight},
       normal,
       {-texCoordSize.x, +texCoordSize.y}},
    }};

    std::array<int, 6> indices{{1, 0, 2, 1, 2, 3}};

    std::vector<Vertex> vertices;
    vertices.reserve(indices.size()); // pre-allocate
    for (int index : indices)
      vertices.push_back(quadVertices.at(index));

    _geometry.updateBuffer(0, vertices);
    _geometry.setPrimitiveCount(vertices.size());

  } // compute chessboard ground
}

void FloorRenderer::setMatricesData(const Camera::MatricesData& matricesData) {
  _matricesData = matricesData;
}

void FloorRenderer::render() {
  if (!_shader)
    D_THROW(std::runtime_error, "shader not setup");

  // hide the floor if the camera is looking from beneath it
  GlContext::enable(GlContext::States::cullFace);

  // transparency friendly
  GlContext::disable(GlContext::States::depthTest);

  _shader->bind();

  _shader->setUniform("u_projectionMatrix", _matricesData.projection);
  _shader->setUniform("u_modelViewMatrix", _matricesData.view);

  _texture.bind();
  _geometry.render();

  GlContext::disable(GlContext::States::cullFace);
  GlContext::enable(GlContext::States::depthTest);
}
