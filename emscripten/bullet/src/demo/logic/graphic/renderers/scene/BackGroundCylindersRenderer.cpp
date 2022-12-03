
#include "BackGroundCylindersRenderer.hpp"

#include "demo/logic/graphicIds.hpp"

#include "framework/asValue.hpp"

#include "framework/math/constants.hpp"

#include "framework/graphic/ResourceManager.hpp"
#include "framework/graphic/GeometryBuilder.hpp"

#include "framework/helpers/GLMath.hpp"

#include "demo/logic/Data.hpp"

void BackGroundCylindersRenderer::initialise(const glm::vec3& size)
{
  _shader = Data::get().graphic.resourceManager.getShader(asValue(Shaders::simpleTexture));

  { // cylinders (generate the texture)

    auto fillBuffer = [](const glm::ivec2& size, unsigned char* rawPixels, int minX, int maxX)
    {
      for (int yy = 0; yy < size.y; ++yy)
      for (int xx = 0; xx < size.x; ++xx)
      {
        if (xx >= minX && xx < maxX)
        {
          if (yy < size.y * 0.25f)
          {
            // blue
            rawPixels[yy * 4 * size.x + xx * 4 + 0] = 0;
            rawPixels[yy * 4 * size.x + xx * 4 + 1] = 0;
            rawPixels[yy * 4 * size.x + xx * 4 + 2] = 128;
          }
          else
          if (yy < size.y * 0.5f)
          {
            // gray
            rawPixels[yy * 4 * size.x + xx * 4 + 0] = 128;
            rawPixels[yy * 4 * size.x + xx * 4 + 1] = 128;
            rawPixels[yy * 4 * size.x + xx * 4 + 2] = 128;
          }
          else
          if (yy < size.y * 0.75f)
          {
            // red
            rawPixels[yy * 4 * size.x + xx * 4 + 0] = 128;
            rawPixels[yy * 4 * size.x + xx * 4 + 1] = 0;
            rawPixels[yy * 4 * size.x + xx * 4 + 2] = 0;
          }
          else
          {
            // green
            rawPixels[yy * 4 * size.x + xx * 4 + 0] = 0;
            rawPixels[yy * 4 * size.x + xx * 4 + 1] = 128;
            rawPixels[yy * 4 * size.x + xx * 4 + 2] = 0;
          }
          rawPixels[yy * 4 * size.x + xx * 4 + 3] = 255;
        }
        else
        {
          // transparent
          rawPixels[yy * 4 * size.x + xx * 4 + 0] = 0;
          rawPixels[yy * 4 * size.x + xx * 4 + 1] = 0;
          rawPixels[yy * 4 * size.x + xx * 4 + 2] = 0;
          rawPixels[yy * 4 * size.x + xx * 4 + 3] = 0;
        }
      }
    };

    constexpr bool pixelated = true;
    constexpr bool repeat = true;

    const glm::ivec2 size = { 512, 512 };
    auto pixelsPtr = std::make_unique<unsigned char[]>(size.x * size.y * 4);
    unsigned char* rawPixels = pixelsPtr.get();

    fillBuffer(size, rawPixels, size.x * 0.00f, size.x * 0.25f);
    _textures[0].allocateBlank(size, pixelated, repeat, rawPixels);

    fillBuffer(size, rawPixels, size.x * 0.25f, size.x * 0.50f);
    _textures[1].allocateBlank(size, pixelated, repeat, rawPixels);

    fillBuffer(size, rawPixels, size.x * 0.50f, size.x * 0.75f);
    _textures[2].allocateBlank(size, pixelated, repeat, rawPixels);

    fillBuffer(size, rawPixels, size.x * 0.75f, size.x * 1.00f);
    _textures[3].allocateBlank(size, pixelated, repeat, rawPixels);

  } // cylinders (generate the texture)

  {

    struct Vertex
    {
      glm::vec3 position;
      glm::vec2 texCoord;
    };

    glm::vec3 boardHSize = size * 0.65f;

    const float radius = boardHSize.x * 0.5f;

    std::array<int, 6> indices{{ 1,0,2,  1,2,3 }};

    std::vector<Vertex> vertices;
    vertices.reserve(2048); // pre-allocate

    {

      constexpr unsigned int cylinderQuality = 64;
      for (unsigned int ii = 0; ii < cylinderQuality; ++ii)
      {
        const float currCoef = float(ii + 0) / cylinderQuality;
        const float nextCoef = float(ii + 1) / cylinderQuality;

        const float currCos = std::cos(currCoef * math::pi2);
        const float nextCos = std::cos(nextCoef * math::pi2);
        const float currSin = std::sin(currCoef * math::pi2);
        const float nextSin = std::sin(nextCoef * math::pi2);

        std::array<Vertex, 4> patchVertices
        {{
          { { +boardHSize.x * 2.0f, radius * currCos, radius * currSin }, { 12.0f, currCoef * 8.0f } },
          { { -boardHSize.x * 2.0f, radius * currCos, radius * currSin }, {  0.0f, currCoef * 8.0f } },
          { { +boardHSize.x * 2.0f, radius * nextCos, radius * nextSin }, { 12.0f, nextCoef * 8.0f } },
          { { -boardHSize.x * 2.0f, radius * nextCos, radius * nextSin }, {  0.0f, nextCoef * 8.0f } },
        }};

        for (int index : indices)
          vertices.push_back(patchVertices[index]);
      }

    }

    GeometryBuilder geometryBuilder;

    geometryBuilder
      .reset()
      .setShader(*_shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
      .addVboAttribute("a_texCoord", Geometry::AttrType::Vec2f, 3);

    for (auto& geometry : _geometries)
    {
      geometryBuilder.build(geometry);
      geometry.updateBuffer(0, vertices);
      geometry.setPrimitiveCount(vertices.size());
    }

  }

}

void BackGroundCylindersRenderer::setMatricesData(const Camera::MatricesData& matricesData)
{
  _matricesData = matricesData;
}

void BackGroundCylindersRenderer::update(float elapsedTime)
{
  _cylinderAnimationTime += elapsedTime;
}

void BackGroundCylindersRenderer::render(const glm::vec3& center)
{

  if (!_shader)
    D_THROW(std::runtime_error, "shader not setup");

  _shader->bind();

  for (std::size_t ii = 0; ii < _textures.size(); ++ii)
  {
    glm::mat4 model = glm::identity<glm::mat4>();

    model = glm::translate(model, center);
    model = glm::rotate(model, _cylinderAnimationTime * 0.1f * (ii + 1), glm::vec3(+1,0,0));

    const glm::mat4 composed = _matricesData.projection * _matricesData.view * model;

    _shader->setUniform("u_composedMatrix", composed);

    _textures[ii].bind();
    _geometries[ii].render();
  }
}
