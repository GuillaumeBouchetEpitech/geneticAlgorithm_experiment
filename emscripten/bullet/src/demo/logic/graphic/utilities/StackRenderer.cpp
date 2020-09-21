
#include "StackRenderer.hpp"

#include "demo/logic/Data.hpp"

StackRenderer::StackRenderer()
{
    _lineVertices.reserve(128 * 2); // pre-allocate
    _triangleVertices.reserve(128 * 3); // pre-allocate
}

StackRenderer::~StackRenderer()
{}

//

// void StackRenderer::push(const t_vertex& vertex)
// {
//     if (_lineVertices.size() == _lineVertices.capacity())
//         flush();

//     _lineVertices.emplace_back(vertex);
// }

void StackRenderer::pushLine(const glm::vec3& posA,
                             const glm::vec3& posB,
                             const glm::vec3& color)
{
    if (_lineVertices.size() + 2 >= _lineVertices.capacity())
        flush();

    _lineVertices.emplace_back(posA, color);
    _lineVertices.emplace_back(posB, color);
}

void StackRenderer::pushCross(const glm::vec3& pos,
                              const glm::vec3& color,
                              float halfExtent)
{
    if (halfExtent <= 0)
        return;

    std::array<glm::vec3[2], 3> vertices{{
        { { pos.x - halfExtent, pos.y, pos.z },
          { pos.x + halfExtent, pos.y, pos.z } },
        { { pos.x, pos.y - halfExtent, pos.z },
          { pos.x, pos.y + halfExtent, pos.z } },
        { { pos.x, pos.y, pos.z - halfExtent },
          { pos.x, pos.y, pos.z + halfExtent } }
    }};

    for (const auto& elem : vertices)
        pushLine(elem[0], elem[1], color);
}

void StackRenderer::pushTriangle(const glm::vec3& posA, const glm::vec3& posB,
                                 const glm::vec3& posC, const glm::vec3& color)
{
    if (_triangleVertices.size() + 3 >= _triangleVertices.capacity())
        flush();

    _triangleVertices.emplace_back(posA, color);
    _triangleVertices.emplace_back(posB, color);
    _triangleVertices.emplace_back(posC, color);
}

void StackRenderer::pushQuad(const glm::vec2& center, const glm::vec2& size, const glm::vec3& color)
{
    const glm::vec2 hsize = size * 0.5f;

    const std::array<glm::vec3, 4> vertices = {{
        { center.x + hsize.x, center.y + hsize.y, 0.0f },
        { center.x - hsize.x, center.y + hsize.y, 0.0f },
        { center.x + hsize.x, center.y - hsize.y, 0.0f },
        { center.x - hsize.x, center.y - hsize.y, 0.0f },
    }};
    std::array<int, 6> indices = {{ 0,1,2, 2,1,3, }};

    pushTriangle(vertices[indices[0]], vertices[indices[1]], vertices[indices[2]], color);
    pushTriangle(vertices[indices[3]], vertices[indices[4]], vertices[indices[5]], color);
}

void StackRenderer::pushLine(const glm::vec2& posA,
                             const glm::vec2& posB,
                             const glm::vec3& color)
{
    StackRenderer::pushLine({ posA.x, posA.y, 0 }, { posB.x, posB.y, 0 }, color);
}

void StackRenderer::pushRectangle(const glm::vec2& pos,
                                  const glm::vec2& size,
                                  const glm::vec3& color)
{
    const glm::vec2 farPos = pos + size;

    StackRenderer::pushLine(pos, {farPos.x, pos.y}, color);
    StackRenderer::pushLine({farPos.x, pos.y}, farPos, color);
    StackRenderer::pushLine(farPos, {pos.x, farPos.y}, color);
    StackRenderer::pushLine({pos.x, farPos.y}, pos, color);
}

void StackRenderer::flush()
{
    flushLines();
    flushTriangles();
}

void StackRenderer::flushLines()
{
    auto& geometry = Data::get().graphic.geometries.stackRenderer.lines;

    geometry.updateBuffer(0, _lineVertices, true);
    geometry.setPrimitiveCount(_lineVertices.size());

    geometry.render();

    _lineVertices.clear();
}

void StackRenderer::flushTriangles()
{
    auto& geometry = Data::get().graphic.geometries.stackRenderer.triangles;

    geometry.updateBuffer(0, _triangleVertices, true);
    geometry.setPrimitiveCount(_triangleVertices.size());

    geometry.render();

    _triangleVertices.clear();
}
