
#include "FloorRenderer.hpp"

#include "demo/logic/graphicIds.hpp"

#include "framework/asValue.hpp"

#include "framework/graphic/ResourceManager.hpp"
#include "framework/graphic/GeometryBuilder.hpp"

#include "framework/helpers/GLMath.hpp"

#include "demo/logic/Data.hpp"

void FloorRenderer::initialise(
  const glm::vec3& center,
  const glm::vec3& size)
{
  _shader = Data::get().graphic.resourceManager.getShader(asValue(Shaders::litTexture));

  {

    GeometryBuilder geometryBuilder;

    geometryBuilder
      .reset()
      .setShader(*_shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
      .addVboAttribute("a_normal", Geometry::AttrType::Vec3f, 3)
      .addVboAttribute("a_texCoord", Geometry::AttrType::Vec2f, 6);

    geometryBuilder.build(_geometry);
    _geometry.setPrimitiveCount(0);

  }

  {

    const glm::ivec2 size = { 512, 512 };
    auto pixelsPtr = std::make_unique<unsigned char[]>(size.x * size.y * 4);
    unsigned char* rawPixels = pixelsPtr.get();

    const auto setPixel = [&size, rawPixels](
      int x, int y,
      unsigned char grey,
      unsigned char alpha)
    {
      rawPixels[y * 4 * size.x + x * 4 + 0] = grey;
      rawPixels[y * 4 * size.x + x * 4 + 1] = grey;
      rawPixels[y * 4 * size.x + x * 4 + 2] = grey;
      rawPixels[y * 4 * size.x + x * 4 + 3] = alpha;
    };

    for (int yy = 0; yy < size.y; ++yy)
    for (int xx = 0; xx < size.x; ++xx)
    {
      if (
        (xx < size.x * 0.5 && yy < size.y * 0.5) ||
        (xx > size.x * 0.5 && yy > size.y * 0.5)
      )
      {
        setPixel(xx, yy, 32, 220);
      }
      else
      {
        setPixel(xx, yy, 100, 255);
      }
    }

    bool pixelated = false;
    bool repeat = true;
    _texture.allocateBlank(size, pixelated, repeat, rawPixels);

  }

  { // compute chessboard ground

    struct Vertex
    {
      glm::vec3 position;
      glm::vec3 normal;
      glm::vec2 texCoord;
    };

    glm::vec3 boardHSize = glm::ceil(size * 0.55f / 100.55f) * 100.0f;
    glm::vec3 boardPos = center;
    glm::vec3 texCoordSize = boardHSize / 100.0f;
    constexpr float boardHeight = -0.1f;

    const glm::vec3 normal = { 0, 0, 1 };

    std::array<Vertex, 4> quadVertices
    {{
      { { boardPos.x + boardHSize.x, boardPos.y - boardHSize.y, boardHeight }, normal, { +texCoordSize.x, -texCoordSize.y } },
      { { boardPos.x - boardHSize.x, boardPos.y - boardHSize.y, boardHeight }, normal, { -texCoordSize.x, -texCoordSize.y } },
      { { boardPos.x + boardHSize.x, boardPos.y + boardHSize.y, boardHeight }, normal, { +texCoordSize.x, +texCoordSize.y } },
      { { boardPos.x - boardHSize.x, boardPos.y + boardHSize.y, boardHeight }, normal, { -texCoordSize.x, +texCoordSize.y } },
    }};

    std::array<int, 6> indices{{ 1,0,2,  1,2,3 }};

    std::vector<Vertex> vertices;
    vertices.reserve(indices.size()); // pre-allocate
    for (int index : indices)
      vertices.push_back(quadVertices[index]);

    _geometry.updateBuffer(0, vertices);
    _geometry.setPrimitiveCount(vertices.size());

  } // compute chessboard ground

}

void FloorRenderer::setMatricesData(const Camera::MatricesData& matricesData)
{
  _matricesData = matricesData;
}

void FloorRenderer::render()
{
  if (!_shader)
    D_THROW(std::runtime_error, "shader not setup");
  _shader->bind();

  _shader->setUniform("u_projectionMatrix", _matricesData.projection);
  _shader->setUniform("u_modelViewMatrix", _matricesData.view);

  _texture.bind();
  _geometry.render();
}
