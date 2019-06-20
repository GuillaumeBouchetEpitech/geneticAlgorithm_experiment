
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

void	StackRenderer::pushLine(const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& color)
{
    if (_vertices.size() + 2 >= _vertices.capacity())
        flush();

    _vertices.push_back({ posA, color });
    _vertices.push_back({ posB, color });
}

void	StackRenderer::pushCross(const glm::vec3& pos, const glm::vec3& color, float halfExtent)
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

void	StackRenderer::flush()
{
    auto& geometry = Data::get()->graphic.geometries.basic.stackRenderer;

    geometry.updateBuffer(0, _vertices, true);
    geometry.setPrimitiveCount(_vertices.size());

    geometry.render();

    _vertices.clear();
}
