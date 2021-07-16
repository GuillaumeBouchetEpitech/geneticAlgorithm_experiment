
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

// void StackRenderer::push(const Vertex& vertex)
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
                                 const glm::vec3& posC, const glm::vec4& color)
{
    if (_triangleVertices.size() + 3 >= _triangleVertices.capacity())
        flush();

    _triangleVertices.emplace_back(posA, color);
    _triangleVertices.emplace_back(posB, color);
    _triangleVertices.emplace_back(posC, color);
}

void StackRenderer::pushTriangle(const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& posC,
                                 const glm::vec3& colorA, const glm::vec3& colorB, const glm::vec3& colorC)
{
    if (_triangleVertices.size() + 3 >= _triangleVertices.capacity())
        flush();

    _triangleVertices.emplace_back(posA, glm::vec4(colorA, 1.0f));
    _triangleVertices.emplace_back(posB, glm::vec4(colorB, 1.0f));
    _triangleVertices.emplace_back(posC, glm::vec4(colorC, 1.0f));
}

void StackRenderer::pushQuad(const glm::vec2& center, const glm::vec2& size, const glm::vec4& color)
{
    pushQuad(glm::vec3(center, 0.0f), size, color);
}

void StackRenderer::pushQuad(const glm::vec3& center, const glm::vec2& size, const glm::vec4& color)
{
    const glm::vec2 hsize = size * 0.5f;

    const std::array<glm::vec3, 4> vertices = {{
        { center.x + hsize.x, center.y + hsize.y, center.z },
        { center.x - hsize.x, center.y + hsize.y, center.z },
        { center.x + hsize.x, center.y - hsize.y, center.z },
        { center.x - hsize.x, center.y - hsize.y, center.z },
    }};
    std::array<int, 6> indices = {{ 0,1,2, 2,1,3, }};

    pushTriangle(vertices[indices[0]], vertices[indices[1]], vertices[indices[2]], color);
    pushTriangle(vertices[indices[3]], vertices[indices[4]], vertices[indices[5]], color);
}

void StackRenderer::pushThickTriangleLine(const glm::vec2& posA, const glm::vec2& posB, float thickness, const glm::vec4& color)
{
    const float angle = std::atan2(posB.y - posA.y, posB.x - posA.x) + M_PI * 0.5f;

    const float stepX = std::cos(angle) * thickness * 0.5f;
    const float stepY = std::sin(angle) * thickness * 0.5f;

    std::array<glm::vec3, 4> vertices{{
        { posA.x - stepX, posA.y - stepY, 0.0f },
        { posA.x + stepX, posA.y + stepY, 0.0f },
        { posB.x - stepX, posB.y - stepY, 0.0f },
        { posB.x + stepX, posB.y + stepY, 0.0f },
    }};

    pushTriangle(vertices[0], vertices[3], vertices[2], color);
    pushTriangle(vertices[0], vertices[1], vertices[3], color);
}

void StackRenderer::pushThickTriangleLine(const glm::vec2& posA, const glm::vec2& posB, float thicknessA, float thicknessB, const glm::vec4& color)
{
    const float angle = std::atan2(posB.y - posA.y, posB.x - posA.x) + M_PI * 0.5f;

    const float stepAX = std::cos(angle) * thicknessA * 0.5f;
    const float stepAY = std::sin(angle) * thicknessA * 0.5f;
    const float stepBX = std::cos(angle) * thicknessB * 0.5f;
    const float stepBY = std::sin(angle) * thicknessB * 0.5f;

    std::array<glm::vec3, 4> vertices{{
        { posA.x - stepAX, posA.y - stepAY, 0.0f },
        { posA.x + stepAX, posA.y + stepAY, 0.0f },
        { posB.x - stepBX, posB.y - stepBY, 0.0f },
        { posB.x + stepBX, posB.y + stepBY, 0.0f },
    }};

    pushTriangle(vertices[0], vertices[3], vertices[2], color);
    pushTriangle(vertices[0], vertices[1], vertices[3], color);
}

void StackRenderer::pushThickTriangleLine(const glm::vec3& posA, const glm::vec3& posB, float thickness, const glm::vec4& color)
{
    const float angle = std::atan2(posB.y - posA.y, posB.x - posA.x) + M_PI * 0.5f;

    const float stepX = std::cos(angle) * thickness * 0.5f;
    const float stepY = std::sin(angle) * thickness * 0.5f;

    std::array<glm::vec3, 4> vertices{{
        { posA.x - stepX, posA.y - stepY, posA.z },
        { posA.x + stepX, posA.y + stepY, posA.z },
        { posB.x - stepX, posB.y - stepY, posB.z },
        { posB.x + stepX, posB.y + stepY, posB.z },
    }};

    pushTriangle(vertices[0], vertices[3], vertices[2], color);
    pushTriangle(vertices[0], vertices[1], vertices[3], color);
}

void StackRenderer::pushThickTriangle3DLine(const glm::vec3& posA, const glm::vec3& posB,
                                            float thicknessA, float thicknessB,
                                            const glm::vec3& colorA, const glm::vec3& colorB)
{
    const glm::vec3 diff = posB - posA;
    const float horizontalAngle = std::atan2(diff.y, diff.x);
    const float verticalAngle = std::atan2(diff.z, glm::length(glm::vec2(diff)));

    const float verticalUpAngleRad = verticalAngle - float(M_PI) * 0.5f;
    const float verticalForwardAngleRad = verticalAngle;
    const float horizontalAngleRad = horizontalAngle;

    glm::vec3 upAxis;
    glm::vec3 forwardAxis;
    glm::vec3 leftAxis;

    const float upRadius = std::cos(verticalUpAngleRad);
    upAxis.z = std::sin(verticalUpAngleRad);
    upAxis.x = upRadius * std::cos(horizontalAngleRad);
    upAxis.y = upRadius * std::sin(horizontalAngleRad);

    const float forwardRadius = std::cos(verticalForwardAngleRad);
    forwardAxis.z = std::sin(verticalForwardAngleRad);
    forwardAxis.x = forwardRadius * std::cos(horizontalAngleRad);
    forwardAxis.y = forwardRadius * std::sin(horizontalAngleRad);

    leftAxis.x = upAxis.y * forwardAxis.z - upAxis.z * forwardAxis.y;
    leftAxis.y = upAxis.z * forwardAxis.x - upAxis.x * forwardAxis.z;
    leftAxis.z = upAxis.x * forwardAxis.y - upAxis.y * forwardAxis.x;

    glm::vec3 posA_bottomLeft = posA - leftAxis * thicknessA - upAxis * thicknessA;
    glm::vec3 posA_bottomRight = posA + leftAxis * thicknessA - upAxis * thicknessA;
    glm::vec3 posA_topCenter = posA + upAxis * thicknessA;

    glm::vec3 posB_bottomLeft = posB - leftAxis * thicknessB - upAxis * thicknessB;
    glm::vec3 posB_bottomRight = posB + leftAxis * thicknessB - upAxis * thicknessB;
    glm::vec3 posB_topCenter = posB + upAxis * thicknessB;

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 color;
    };

    std::array<std::array<Vertex, 4>, 3> allQuads{{
        {{ { posA_bottomLeft, colorA }, { posA_bottomRight, colorA }, { posB_bottomLeft, colorB }, { posB_bottomRight, colorB } }},
        {{ { posA_bottomLeft, colorA }, { posA_topCenter, colorA }, { posB_bottomLeft, colorB }, { posB_topCenter, colorB } }},
        {{ { posA_bottomRight, colorA }, { posA_topCenter, colorA }, { posB_bottomRight, colorB }, { posB_topCenter, colorB } }},
    }};

    for (auto& quad : allQuads)
    {
        pushTriangle(quad[0].position, quad[3].position, quad[2].position, quad[0].color, quad[3].color, quad[2].color);
        pushTriangle(quad[0].position, quad[1].position, quad[3].position, quad[0].color, quad[3].color, quad[2].color);
    }
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
