
#include "StackRenderer.hpp"

#include "demo/logic/Data.hpp"

StackRenderer::StackRenderer()
{
    _lineVertices.reserve(128 * 2);
    _triangleVertices.reserve(128 * 3);
}

StackRenderer::~StackRenderer()
{}

//

// void StackRenderer::push(const t_vertex& vertex)
// {
//     if (_lineVertices.size() == _lineVertices.capacity())
//         flush();

//     _lineVertices.push_back(vertex);
// }

void StackRenderer::pushLine(const glm::vec3& posA,
                             const glm::vec3& posB,
                             const glm::vec3& color)
{
    if (_lineVertices.size() + 2 >= _lineVertices.capacity())
        flush();

    _lineVertices.push_back({ posA, color });
    _lineVertices.push_back({ posB, color });
}

void StackRenderer::pushCross(const glm::vec3& pos,
                              const glm::vec3& color,
                              float halfExtent)
{
    if (halfExtent <= 0)
        return;

    glm::vec3 vertices[][2] = {
        { { pos.x - halfExtent, pos.y, pos.z },
          { pos.x + halfExtent, pos.y, pos.z } },
        { { pos.x, pos.y - halfExtent, pos.z },
          { pos.x, pos.y + halfExtent, pos.z } },
        { { pos.x, pos.y, pos.z - halfExtent },
          { pos.x, pos.y, pos.z + halfExtent } }
    };

    for (int ii = 0; ii < 3; ++ii)
        pushLine(vertices[ii][0], vertices[ii][1], color);
}

void StackRenderer::pushTriangle(const glm::vec3& posA, const glm::vec3& posB,
                                 const glm::vec3& posC, const glm::vec3& color)
{
    if (_triangleVertices.size() + 3 >= _triangleVertices.capacity())
        flush();

    _triangleVertices.push_back({ posA, color });
    _triangleVertices.push_back({ posB, color });
    _triangleVertices.push_back({ posC, color });
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
    auto& geometry = Data::get()->graphic.geometries.stackRenderer.lines;

    geometry.updateBuffer(0, _lineVertices, true);
    geometry.setPrimitiveCount(_lineVertices.size());

    geometry.render();

    _lineVertices.clear();
}

void StackRenderer::flushTriangles()
{
    auto& geometry = Data::get()->graphic.geometries.stackRenderer.triangles;

    geometry.updateBuffer(0, _triangleVertices, true);
    geometry.setPrimitiveCount(_triangleVertices.size());

    geometry.render();

    _triangleVertices.clear();
}
