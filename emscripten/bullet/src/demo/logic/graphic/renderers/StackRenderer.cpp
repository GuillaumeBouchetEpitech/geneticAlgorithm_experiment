
#include "StackRenderer.hpp"

#include "demo/logic/graphicIds.hpp"

#include "framework/asValue.hpp"

#include "framework/math/constants.hpp"

#include "framework/graphic/ResourceManager.hpp"

// #include "framework/graphic/ShaderProgramBuilder.hpp"
#include "framework/graphic/GeometryBuilder.hpp"

#include "demo/logic/Data.hpp"

void StackRenderer::initialise()
{
    _shader = ResourceManager::get().getShader(asValue(Shaders::stackRenderer));

    {

        GeometryBuilder geometryBuilder;

        geometryBuilder
            .reset()
            .setShader(*_shader)
            .setPrimitiveType(Geometry::PrimitiveType::lines)
            .addVbo()
            .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
            .addVboAttribute("a_color", Geometry::AttrType::Vec4f, 3);

        geometryBuilder.build(_geometries.lines);

        geometryBuilder
            .setPrimitiveType(Geometry::PrimitiveType::triangles);

        geometryBuilder.build(_geometries.triangles);

        _geometries.lines.setPrimitiveCount(0);
        _geometries.triangles.setPrimitiveCount(0);

    }

    constexpr std::size_t preAllocatedSize = 1024 * 8;
    _lineVertices.reserve(preAllocatedSize);
    _triangleVertices.reserve(preAllocatedSize);
}

void StackRenderer::setMatricesData(const Camera::MatricesData& matricesData)
{
    _matricesData = matricesData;
}

//

// void StackRenderer::push(const Vertex& vertex)
// {
//     if (_lineVertices.size() == _lineVertices.capacity())
//         flush();

//     _lineVertices.emplace_back(vertex);
// }

void StackRenderer::pushLine(
    const glm::vec3& posA,
    const glm::vec3& posB,
    const glm::vec4& color)
{
    if (_lineVertices.size() + 2 >= _lineVertices.capacity())
        flush();

    _lineVertices.emplace_back(posA, color);
    _lineVertices.emplace_back(posB, color);
}

void StackRenderer::pushLine(
    const glm::vec3& posA,
    const glm::vec3& posB,
    const glm::vec3& color)
{
    pushLine(posA, posB, glm::vec4(color, 1.0f));
}

void StackRenderer::pushCross(
    const glm::vec3& pos,
    const glm::vec3& color,
    float halfExtent)
{
    if (halfExtent <= 0)
        return;

    std::array<glm::vec3[2], 3> vertices
    {{
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

void StackRenderer::pushTriangle(
    const glm::vec3& posA,
    const glm::vec3& posB,
    const glm::vec3& posC,
    const glm::vec4& color)
{
    pushTriangle(
        posA,
        posB,
        posC,
        color,
        color,
        color
    );
}

void StackRenderer::pushTriangle(
    const glm::vec3& posA,
    const glm::vec3& posB,
    const glm::vec3& posC,
    const glm::vec4& colorA,
    const glm::vec4& colorB,
    const glm::vec4& colorC)
{
    if (_triangleVertices.size() + 3 >= _triangleVertices.capacity())
        flush();

    _triangleVertices.emplace_back(posA, colorA);
    _triangleVertices.emplace_back(posB, colorB);
    _triangleVertices.emplace_back(posC, colorC);
}

void StackRenderer::pushQuad(const glm::vec2& center, const glm::vec2& size, const glm::vec4& color)
{
    pushQuad(glm::vec3(center, 0.0f), size, color);
}

void StackRenderer::pushQuad(const glm::vec3& center, const glm::vec2& size, const glm::vec4& color)
{
    const glm::vec2 hsize = size * 0.5f;

    const std::array<glm::vec3, 4> vertices =
    {{
        { center.x + hsize.x, center.y + hsize.y, center.z },
        { center.x - hsize.x, center.y + hsize.y, center.z },
        { center.x + hsize.x, center.y - hsize.y, center.z },
        { center.x - hsize.x, center.y - hsize.y, center.z },
    }};
    std::array<int, 6> indices = {{ 0,1,2, 2,1,3, }};

    pushTriangle(vertices[indices[0]], vertices[indices[1]], vertices[indices[2]], color);
    pushTriangle(vertices[indices[3]], vertices[indices[4]], vertices[indices[5]], color);
}

void StackRenderer::pushCircle(const glm::vec2& center, float radius, const glm::vec4& color)
{
    pushCircle(glm::vec3(center, 0.0f), radius, color);
}

void StackRenderer::pushCircle(const glm::vec3& center, float radius, const glm::vec4& color)
{
    constexpr int quality = 16;

    std::array<glm::vec2, quality> cachedValues;

    for (int ii = 0; ii < quality; ++ii)
    {
        const float coef = float(ii) / quality;

        cachedValues[ii].x = std::cos(coef * math::pi2);
        cachedValues[ii].y = std::sin(coef * math::pi2);
    }

    for (int ii = 0; ii < quality; ++ii)
    {
        const int indexA = ii;
        const int indexB = (ii + 1) % quality;

        glm::vec3 vertexA = {
            center.x + radius * cachedValues[indexA].x,
            center.y + radius * cachedValues[indexA].y,
            0.0f
        };

        glm::vec3 vertexB = {
            center.x + radius * cachedValues[indexB].x,
            center.y + radius * cachedValues[indexB].y,
            0.0f
        };

        pushTriangle(center, vertexA, vertexB, color);
    }
}

void StackRenderer::pushThickTriangleLine(const glm::vec2& posA, const glm::vec2& posB, float thickness, const glm::vec4& color)
{
    const float angle = std::atan2(posB.y - posA.y, posB.x - posA.x) + math::hpi;

    const float stepX = std::cos(angle) * thickness * 0.5f;
    const float stepY = std::sin(angle) * thickness * 0.5f;

    std::array<glm::vec3, 4> vertices
    {{
        { posA.x - stepX, posA.y - stepY, 0.0f },
        { posA.x + stepX, posA.y + stepY, 0.0f },
        { posB.x - stepX, posB.y - stepY, 0.0f },
        { posB.x + stepX, posB.y + stepY, 0.0f },
    }};

    pushTriangle(vertices[0], vertices[3], vertices[2], color);
    pushTriangle(vertices[0], vertices[1], vertices[3], color);
}

// void StackRenderer::pushThickTriangleLine(const glm::vec3& posA, const glm::vec3& posB, float thickness, const glm::vec4& color)
// {
//     const float angle = std::atan2(posB.y - posA.y, posB.x - posA.x) + math::hpi;

//     const float stepX = std::cos(angle) * thickness * 0.5f;
//     const float stepY = std::sin(angle) * thickness * 0.5f;

//     std::array<glm::vec3, 4> vertices{{
//         { posA.x - stepX, posA.y - stepY, posA.z },
//         { posA.x + stepX, posA.y + stepY, posA.z },
//         { posB.x - stepX, posB.y - stepY, posB.z },
//         { posB.x + stepX, posB.y + stepY, posB.z },
//     }};

//     pushTriangle(vertices[0], vertices[3], vertices[2], color);
//     pushTriangle(vertices[0], vertices[1], vertices[3], color);
// }

void StackRenderer::pushThickTriangleLine(
    const glm::vec2& posA,
    const glm::vec2& posB,
    float thicknessA,
    float thicknessB,
    const glm::vec4& color)
{
    const float angle = std::atan2(posB.y - posA.y, posB.x - posA.x) + math::hpi;

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

void StackRenderer::pushThickTriangleLine(
    const glm::vec2& posA,
    const glm::vec2& posB,
    float thicknessA,
    float thicknessB,
    const glm::vec4& color,
    float z)
{
    const float angle = std::atan2(posB.y - posA.y, posB.x - posA.x) + math::hpi;

    const float stepAX = std::cos(angle) * thicknessA * 0.5f;
    const float stepAY = std::sin(angle) * thicknessA * 0.5f;
    const float stepBX = std::cos(angle) * thicknessB * 0.5f;
    const float stepBY = std::sin(angle) * thicknessB * 0.5f;

    std::array<glm::vec3, 4> vertices{{
        { posA.x - stepAX, posA.y - stepAY, z },
        { posA.x + stepAX, posA.y + stepAY, z },
        { posB.x - stepBX, posB.y - stepBY, z },
        { posB.x + stepBX, posB.y + stepBY, z },
    }};

    pushTriangle(vertices[0], vertices[3], vertices[2], color);
    pushTriangle(vertices[0], vertices[1], vertices[3], color);
}

void StackRenderer::pushThickTriangle3DLine(const glm::vec3& posA, const glm::vec3& posB,
                                            float thicknessA, float thicknessB,
                                            const glm::vec4& colorA, const glm::vec4& colorB)
{
    const glm::vec3 diff = posB - posA;
    const float horizontalAngle = std::atan2(diff.y, diff.x);
    const float verticalAngle = std::atan2(diff.z, glm::length(glm::vec2(diff)));

    const float verticalUpAngleRad = verticalAngle - math::hpi;
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
        glm::vec4 color;
    };

    std::array<std::array<Vertex, 4>, 3> allQuads
    {{
        {{
            { posA_bottomLeft, colorA },
            { posA_bottomRight, colorA },
            { posB_bottomLeft, colorB },
            { posB_bottomRight, colorB }
        }},
        {{
            { posA_bottomLeft, colorA },
            { posA_topCenter, colorA },
            { posB_bottomLeft, colorB },
            { posB_topCenter, colorB }
        }},
        {{
            { posA_bottomRight, colorA },
            { posA_topCenter, colorA },
            { posB_bottomRight, colorB },
            { posB_topCenter, colorB }
        }},
    }};

    for (auto& quad : allQuads)
    {
        pushTriangle(quad[0].position, quad[3].position, quad[2].position, quad[0].color, quad[3].color, quad[2].color);
        pushTriangle(quad[0].position, quad[1].position, quad[3].position, quad[0].color, quad[3].color, quad[2].color);
    }
}

void StackRenderer::pushLine(const glm::vec2& posA,
                             const glm::vec2& posB,
                             const glm::vec4& color)
{
    StackRenderer::pushLine({ posA.x, posA.y, 0 }, { posB.x, posB.y, 0 }, color);
}

void StackRenderer::pushLine(
    const glm::vec2& posA,
    const glm::vec2& posB,
    const glm::vec3& color)
{
    StackRenderer::pushLine({ posA.x, posA.y, 0 }, { posB.x, posB.y, 0 }, glm::vec4(color, 1.0f));
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
    if (_lineVertices.empty())
        return;

    if (!_shader)
        D_THROW(std::runtime_error, "shader not setup");

    _shader->bind();
    _shader->setUniform("u_composedMatrix", _matricesData.composed);

    _geometries.lines.updateBuffer(0, _lineVertices, true);
    _geometries.lines.setPrimitiveCount(uint32_t(_lineVertices.size()));
    _geometries.lines.render();

    _lineVertices.clear();
}

void StackRenderer::flushTriangles()
{
    if (_triangleVertices.empty())
        return;

    if (!_shader)
        D_THROW(std::runtime_error, "shader not setup");

    _shader->bind();
    _shader->setUniform("u_composedMatrix", _matricesData.composed);

    _geometries.triangles.updateBuffer(0, _triangleVertices, true);
    _geometries.triangles.setPrimitiveCount(uint32_t(_triangleVertices.size()));
    _geometries.triangles.render();

    _triangleVertices.clear();
}

std::size_t StackRenderer::getLinesCount() const
{
    return _lineVertices.size();
}

std::size_t StackRenderer::getTrianglesCount() const
{
    return _triangleVertices.size();
}
