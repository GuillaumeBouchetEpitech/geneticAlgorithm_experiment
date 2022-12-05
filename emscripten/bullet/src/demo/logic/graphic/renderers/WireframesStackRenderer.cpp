
#include "WireframesStackRenderer.hpp"

#include "demo/logic/graphicIds.hpp"

#include "framework/asValue.hpp"

#include "framework/math/constants.hpp"

#include "framework/graphic/ResourceManager.hpp"

#include "framework/graphic/GeometryBuilder.hpp"

#include "demo/logic/Context.hpp"

void WireframesStackRenderer::initialise()
{
    _shader = Context::get().graphic.resourceManager.getShader(asValue(Shaders::stackRenderer));

    {

        GeometryBuilder geometryBuilder;

        geometryBuilder
            .reset()
            .setShader(*_shader)
            .setPrimitiveType(Geometry::PrimitiveType::lines)
            .addVbo()
            .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
            .addVboAttribute("a_color", Geometry::AttrType::Vec4f, 3);

        geometryBuilder.build(_geometry);
        _geometry.setPrimitiveCount(0);

    }

    constexpr std::size_t preAllocatedSize = 1024 * 32; // 32Ko
    _vertices.reserve(preAllocatedSize);
}

void WireframesStackRenderer::setMatricesData(const Camera::MatricesData& matricesData)
{
    _matricesData = matricesData;
}

//

void WireframesStackRenderer::pushLine(
    const glm::vec3& posA,
    const glm::vec3& posB,
    const glm::vec4& color)
{
    if (_vertices.size() + 2 >= _vertices.capacity())
        flush();

    _vertices.emplace_back(posA, color);
    _vertices.emplace_back(posB, color);
}

void WireframesStackRenderer::pushLine(
    const glm::vec3& posA,
    const glm::vec3& posB,
    const glm::vec3& color)
{
    pushLine(posA, posB, glm::vec4(color, 1.0f));
}

void WireframesStackRenderer::pushCross(
    const glm::vec3& pos,
    const glm::vec3& color,
    float halfExtent)
{
    if (halfExtent <= 0)
        return;

    std::array<std::array<glm::vec3, 2>, 3> vertices
    {{
        {{
            { pos.x - halfExtent, pos.y, pos.z },
            { pos.x + halfExtent, pos.y, pos.z }
        }},
        {{
            { pos.x, pos.y - halfExtent, pos.z },
            { pos.x, pos.y + halfExtent, pos.z }
        }},
        {{
            { pos.x, pos.y, pos.z - halfExtent },
            { pos.x, pos.y, pos.z + halfExtent }
        }}
    }};

    for (const auto& elem : vertices)
        pushLine(elem.at(0), elem.at(1), color);
}

void WireframesStackRenderer::pushLine(const glm::vec2& posA,
                             const glm::vec2& posB,
                             const glm::vec4& color)
{
    WireframesStackRenderer::pushLine({ posA.x, posA.y, 0 }, { posB.x, posB.y, 0 }, color);
}

void WireframesStackRenderer::pushLine(
    const glm::vec2& posA,
    const glm::vec2& posB,
    const glm::vec3& color)
{
    WireframesStackRenderer::pushLine({ posA.x, posA.y, 0 }, { posB.x, posB.y, 0 }, glm::vec4(color, 1.0f));
}


void WireframesStackRenderer::pushRectangle(const glm::vec2& pos,
                                  const glm::vec2& size,
                                  const glm::vec3& color)
{
    const glm::vec2 farPos = pos + size;

    WireframesStackRenderer::pushLine(pos, {farPos.x, pos.y}, color);
    WireframesStackRenderer::pushLine({farPos.x, pos.y}, farPos, color);
    WireframesStackRenderer::pushLine(farPos, {pos.x, farPos.y}, color);
    WireframesStackRenderer::pushLine({pos.x, farPos.y}, pos, color);
}

void WireframesStackRenderer::flush()
{
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

std::size_t WireframesStackRenderer::getLinesCount() const
{
    return _vertices.size();
}


