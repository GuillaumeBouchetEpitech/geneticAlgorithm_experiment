
#include "Data.hpp"

#include "demo/logic/graphicIds.hpp"

#include "framework/asValue.hpp"

#include "framework/graphic/ResourceManager.hpp"
#include "framework/graphic/ShaderProgram.hpp"
#include "framework/graphic/GeometryBuilder.hpp"

#include "demo/defines.hpp"

void Data::initialiseGeometries()
{
    GeometryBuilder geometryBuilder;

    { // hud geometry perpective

        auto shader = ResourceManager::get().getShader(asValue(Shaders::simpleTexture));

        geometryBuilder
            .reset()
            .setShader(*shader)
            .setPrimitiveType(Geometry::PrimitiveType::triangles)
            .addVbo()
            .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
            .addVboAttribute("a_texCoord", Geometry::AttrType::Vec2f, 3);

        geometryBuilder.build(graphic.geometries.hudPerspective.geometry);

        struct Vertex
        {
            glm::vec3 position;
            glm::vec2 texCoord;
        };

        //
        //
        //

        // std::array<int, 6> refIndices{{ 1,0,2,  1,3,2 }};

        // // const int divider = 8;
        // const int divider = 1; // disable perspective hud for now

        // const auto& vSize = graphic.camera.viewportSize;

        // // Vertex diff{ { 800 / divider, 600 / divider, 0 }, { 1.0f / divider, 1.0f / divider } };
        // Vertex diff{ { vSize.x / divider, vSize.y / divider, 0 }, { 1.0f / divider, 1.0f / divider } };

        // std::vector<Vertex> quadVertices;
        // std::vector<int> indices;
        // quadVertices.reserve(128);
        // indices.reserve(128);

        // int tmp_index = 0;

        // for (int yy = 0; yy < divider; ++yy)
        // for (int xx = 0; xx < divider; ++xx)
        // {
        //     auto getDepth = [](int x)
        //     {
        //         return -40.0f * std::sin(M_PI * float(x) / divider);
        //     };

        //     quadVertices.push_back({
        //         { diff.position.x * (xx + 1), diff.position.y * (yy + 0), getDepth(xx + 1) },
        //         { diff.texCoord.x * (xx + 1), diff.texCoord.y * (yy + 0) },
        //     });
        //     quadVertices.push_back({
        //         { diff.position.x * (xx + 0), diff.position.y * (yy + 0), getDepth(xx + 0) },
        //         { diff.texCoord.x * (xx + 0), diff.texCoord.y * (yy + 0) },
        //     });
        //     quadVertices.push_back({
        //         { diff.position.x * (xx + 1), diff.position.y * (yy + 1), getDepth(xx + 1) },
        //         { diff.texCoord.x * (xx + 1), diff.texCoord.y * (yy + 1) },
        //     });
        //     quadVertices.push_back({
        //         { diff.position.x * (xx + 0), diff.position.y * (yy + 1), getDepth(xx + 0) },
        //         { diff.texCoord.x * (xx + 0), diff.texCoord.y * (yy + 1) },
        //     });

        //     for (int idx : refIndices)
        //         indices.push_back(tmp_index * 4 + idx);

        //     ++tmp_index;
        // }

        // std::vector<Vertex> vertices;
        // vertices.reserve(indices.size()); // pre-allocate
        // for (int index : indices)
        //     vertices.push_back(quadVertices[index]);

        //
        //
        //

        const auto& vSize = graphic.camera.viewportSize;

        std::array<Vertex, 4> quadVertices
        {{
            { { vSize.x * 1.0f, vSize.y * 0.0f, 0.0f }, { 1.0f, 0.0f } },
            { { vSize.x * 0.0f, vSize.y * 0.0f, 0.0f }, { 0.0f, 0.0f } },
            { { vSize.x * 1.0f, vSize.y * 1.0f, 0.0f }, { 1.0f, 1.0f } },
            { { vSize.x * 0.0f, vSize.y * 1.0f, 0.0f }, { 0.0f, 1.0f } }
        }};

        std::array<int, 6> indices{{ 1,0,2,  1,3,2 }};

        std::vector<Vertex> vertices;
        vertices.reserve(indices.size()); // pre-allocate
        for (int index : indices)
            vertices.push_back(quadVertices[index]);

        //
        //
        //

        graphic.geometries.hudPerspective.geometry.updateBuffer(0, vertices);
        graphic.geometries.hudPerspective.geometry.setPrimitiveCount(vertices.size());

    } // hud geometry perpective
}
