
#include "StackRenderer.hpp"

#include "demo/logic/Data.hpp"

StackRenderer::StackRenderer()
{
    _vertices.reserve(256);
}

StackRenderer::~StackRenderer()
{}

//

void	StackRenderer::push(const t_vertex& vertex)
{
    if (_vertices.size() == _vertices.capacity())
        flush();

    _vertices.push_back(vertex);
}

void	StackRenderer::pushLine(const glm::vec3& posA, const glm::vec3& posB,
                                const glm::vec3& color)
{
    if (_vertices.size() + 2 >= _vertices.capacity())
        flush();

    _vertices.push_back({ posA, color });
    _vertices.push_back({ posB, color });
}

void	StackRenderer::pushCross(const glm::vec3& pos, const glm::vec3& color,
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

void	StackRenderer::pushLine(const glm::vec2& posA, const glm::vec2& posB,
                                const glm::vec3& color)
{
    StackRenderer::pushLine({ posA.x, posA.y, 0 }, { posB.x, posB.y, 0 }, color);
}

void	StackRenderer::pushRectangle(const glm::vec2& pos, const glm::vec2& size,
                                     const glm::vec3& color)
{
    const glm::vec2 farPos = pos + size;

    StackRenderer::pushLine(pos, {farPos.x, pos.y}, color);
    StackRenderer::pushLine({farPos.x, pos.y}, farPos, color);
    StackRenderer::pushLine(farPos, {pos.x, farPos.y}, color);
    StackRenderer::pushLine({pos.x, farPos.y}, pos, color);
}

void	StackRenderer::flush()
{
    auto& geometry = Data::get()->graphic.geometries.stackRenderer.lines;

    geometry.updateBuffer(0, _vertices, true);
    geometry.setPrimitiveCount(_vertices.size());

    geometry.render();

    _vertices.clear();
}
