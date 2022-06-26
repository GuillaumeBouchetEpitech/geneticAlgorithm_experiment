
#include "Data.hpp"

#include "graphic/wrappers/ShaderProgram.hpp"
#include "graphic/wrappers/GeometryBuilder.hpp"

#include "demo/defines.hpp"

#include "helpers/TinyObjLoader.hpp"

namespace /*anonymous*/
{

struct ModelVertex
{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
};

void loadModel(const std::string& filename, std::vector<ModelVertex>& vertices); // declaration

void loadCarModel(std::vector<ModelVertex>& vertices)
{
    loadModel("CarNoWheels.obj", vertices);
}

void loadWheelModel(std::vector<ModelVertex>& vertices)
{
    loadModel("CarWheel.obj", vertices);
}

void loadModel(const std::string& filename, std::vector<ModelVertex>& vertices) // definition
{
    const std::string objFile = "./assets/model/" + filename;
    const std::string mtlDir = "./assets/model/";

    vertices.reserve(8 * 1024);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string errMsg;

    bool loadingSuccess = tinyobj::LoadObj(&attrib, &shapes, &materials, &errMsg, objFile.c_str(), mtlDir.c_str());

    if (!loadingSuccess)
        D_THROW(std::runtime_error, "tinyobj::LoadObj failed to open");

    if (!errMsg.empty())
        D_THROW(std::runtime_error, "tinyobj::LoadObj error, message: " << errMsg);

    // Loop over shapes
    for (std::size_t shape = 0; shape < shapes.size(); ++shape)
    {
        const auto& mesh = shapes[shape].mesh;
        const auto& indices = mesh.indices;
        const auto& faceVertices = mesh.num_face_vertices;
        const auto& materialIds = mesh.material_ids;

        // Loop over faces(polygon)
        std::size_t indexOffset = 0;
        for (std::size_t face = 0; face < faceVertices.size(); ++face)
        {
            std::size_t faceVertex = faceVertices[face];

            // per-face material
            int materialId = materialIds[face];

            const auto& diffuse = materials[materialId].diffuse;
            glm::vec3 color = { diffuse[0], diffuse[1], diffuse[2] };
            glm::vec3 position;
            glm::vec3 normal;

            // std::size_t prevIndex = vertices.size();

            // Loop over vertices in the face.
            for (std::size_t vertex = 0; vertex < faceVertex; ++vertex)
            {
                // access to vertex
                tinyobj::index_t index = indices[indexOffset + vertex];
                position.x = attrib.vertices[3 * index.vertex_index + 0];
                position.y = attrib.vertices[3 * index.vertex_index + 1];
                position.z = attrib.vertices[3 * index.vertex_index + 2];
                normal.x = attrib.normals[3 * index.normal_index + 0];
                normal.y = attrib.normals[3 * index.normal_index + 1];
                normal.z = attrib.normals[3 * index.normal_index + 2];

                // D_MYLOG("normal=" << normal.x << "/" << normal.y << "/" << normal.z);

                // tinyobj::real_t tx = attrib.texcoords[2*index.texcoord_index+0];
                // tinyobj::real_t ty = attrib.texcoords[2*index.texcoord_index+1];

                vertices.push_back({ position, color, normal });
            }

            // if (vertices.size() - prevIndex >= 3)
            // {
            //     glm::vec3 vertex1 = vertices[prevIndex + 0].position;
            //     glm::vec3 vertex2 = vertices[prevIndex + 1].position;
            //     glm::vec3 vertex3 = vertices[prevIndex + 2].position;

            //     glm::vec3 newNormal = glm::normalize(glm::cross(vertex2 - vertex3, vertex3 - vertex1));
            //     // glm::vec3 newNormal = glm::normalize(glm::cross(vertex2 - vertex1, vertex3 - vertex1));

            //     for (std::size_t ii = prevIndex; ii < vertices.size(); ++ii)
            //         vertices[ii].normal = newNormal;
            // }

            indexOffset += faceVertex;
        }
    }
}

};


namespace /*anonymous*/
{

// void generateCubeVerticesWireframe(const glm::vec3& size,
//                                    std::vector<glm::vec3>& vertices)
// {
//     const glm::vec3 hsize = size * 0.5f;
//     std::array<glm::vec3, 8> cubeVertices{{
//         // 0 (top)
//         { +hsize.x, +hsize.y, +hsize.z },
//         { -hsize.x, +hsize.y, +hsize.z },
//         { +hsize.x, -hsize.y, +hsize.z },
//         { -hsize.x, -hsize.y, +hsize.z },
//         // 4 (bottom)
//         { +hsize.x, +hsize.y, -hsize.z },
//         { -hsize.x, +hsize.y, -hsize.z },
//         { +hsize.x, -hsize.y, -hsize.z },
//         { -hsize.x, -hsize.y, -hsize.z }
//     }};

//     std::array<int, 24> indices{{
//         0,1,  1,3,  3,2,  2,0,
//         4,5,  5,7,  7,6,  6,4,
//         0,4,  1,5,  3,7,  2,6,
//     }};

//     //

//     vertices.reserve(indices.size());
//     for (int index : indices)
//         vertices.push_back(cubeVertices[index]);
// }

// void generateCubeVerticesFilled(const glm::vec3& size,
//                                 std::vector<glm::vec3>& vertices)
// {
//     const glm::vec3 hsize = size * 0.5f;
//     std::array<glm::vec3, 8> cubeVertices{{
//         // 0 (top)
//         { +hsize.x, +hsize.y, +hsize.z },
//         { -hsize.x, +hsize.y, +hsize.z },
//         { +hsize.x, -hsize.y, +hsize.z },
//         { -hsize.x, -hsize.y, +hsize.z },
//         // 4 (bottom)
//         { +hsize.x, +hsize.y, -hsize.z },
//         { -hsize.x, +hsize.y, -hsize.z },
//         { +hsize.x, -hsize.y, -hsize.z },
//         { -hsize.x, -hsize.y, -hsize.z }
//     }};

//     std::array<int, 36> indices{{
//         0,6,4,  0,2,6,
//         0,3,2,  0,1,3,
//         2,7,6,  2,3,7,
//         4,6,7,  4,7,5,
//         0,4,5,  0,5,1,
//         1,5,7,  1,7,3,
//     }};

//     //

//     vertices.reserve(indices.size());
//     for (int index : indices)
//         vertices.push_back(cubeVertices[index]);
// }

void generateSphereVerticesFilled(float radius,
                                  std::vector<glm::vec3>& vertices)
{
    const float X = 0.525731112119133606f * radius;
    const float Z = 0.850650808352039932f * radius;
    const float N = 0.0f;

    static const std::array<glm::vec3, 12> positions
    {{
        { -X,+N,+Z }, { +X,+N,+Z }, { -X,+N,-Z }, { +X,+N,-Z },
        { +N,+Z,+X }, { +N,+Z,-X }, { +N,-Z,+X }, { +N,-Z,-X },
        { +Z,+X,+N }, { -Z,+X,+N }, { +Z,-X,+N }, { -Z,-X,+N },
    }};

    static const std::array<glm::ivec3, 20> indices
    {{
        { 0, 4, 1}, { 0, 9, 4}, { 9, 5, 4}, { 4, 5, 8}, { 4, 8, 1},
        { 8,10, 1}, { 8, 3,10}, { 5, 3, 8}, { 5, 2, 3}, { 2, 7, 3},
        { 7,10, 3}, { 7, 6,10}, { 7,11, 6}, {11, 0, 6}, { 0, 1, 6},
        { 6, 1,10}, { 9, 0,11}, { 9,11, 2}, { 9, 2, 5}, { 7, 2,11}
    }};

    vertices.clear();
    vertices.reserve(indices.size() * 3); // pre-allocate
    for (const glm::ivec3& index : indices)
    {
        vertices.push_back(positions[index.x]);
        vertices.push_back(positions[index.y]);
        vertices.push_back(positions[index.z]);
    }
}

};

void Data::initialiseGeometries()
{
    GeometryBuilder geometryBuilder;

    { // particles geometries

        geometryBuilder
            .reset()
            .setShader(*graphic.shaders.particles)
            .setPrimitiveType(GL_TRIANGLES)
            .addVbo()
            .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
            .addVbo()
            .setVboAsInstanced()
            .addVboAttribute("a_offsetPosition", Geometry::AttrType::Vec3f, 0)
            .addVboAttribute("a_offsetScale", Geometry::AttrType::Float, 3)
            .addVboAttribute("a_offsetColor", Geometry::AttrType::Vec3f, 4);

        geometryBuilder.build(graphic.geometries.particles.firework);

        std::vector<glm::vec3> particlesVertices;

        generateSphereVerticesFilled(0.5f, particlesVertices);

        graphic.geometries.particles.firework.updateBuffer(0, particlesVertices);
        graphic.geometries.particles.firework.setPrimitiveCount(particlesVertices.size());

        geometryBuilder.build(graphic.geometries.particles.boids);
        graphic.geometries.particles.boids.updateBuffer(0, particlesVertices);
        graphic.geometries.particles.boids.setPrimitiveCount(particlesVertices.size());

    } // particles geometries

    { // stack renderer geometry

        { // lines

            geometryBuilder
                .reset()
                .setShader(*graphic.shaders.stackRenderer)
                .setPrimitiveType(GL_LINES)
                .addVbo()
                .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
                .addVboAttribute("a_color", Geometry::AttrType::Vec4f, 3);

            geometryBuilder.build(graphic.geometries.stackRenderer.lines);
            graphic.geometries.stackRenderer.lines.setPrimitiveCount(0);

        } // lines

        { // triangles

            geometryBuilder
                .reset()
                .setShader(*graphic.shaders.stackRenderer)
                .setPrimitiveType(GL_TRIANGLES)
                .addVbo()
                .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
                .addVboAttribute("a_color", Geometry::AttrType::Vec4f, 3);

            geometryBuilder.build(graphic.geometries.stackRenderer.triangles);

            graphic.geometries.stackRenderer.triangles.setPrimitiveCount(0);

        } // triangles

    } // stack renderer geometry

    { // hud geometry ortho

        geometryBuilder
            .reset()
            .setShader(*graphic.shaders.hudText)
            .setPrimitiveType(GL_TRIANGLES)
            .addVbo()
            .addVboAttribute("a_position", Geometry::AttrType::Vec2f, 0)
            .addVboAttribute("a_texCoord", Geometry::AttrType::Vec2f, 2)
            .addVbo()
            .setVboAsInstanced()
            .addVboAttribute("a_offsetPosition", Geometry::AttrType::Vec3f, 0)
            .addVboAttribute("a_offsetTexCoord", Geometry::AttrType::Vec2f, 3)
            .addVboAttribute("a_offsetColor", Geometry::AttrType::Vec3f, 5)
            .addVboAttribute("a_offsetScale", Geometry::AttrType::Float, 8);

        geometryBuilder.build(graphic.geometries.hudText.letters);

        struct Vertex
        {
            glm::vec2 position;
            glm::vec2 texCoord;
        };

        const auto& hudText = graphic.hudText;
        const glm::vec2 letterSize = hudText.textureSize / hudText.gridSize;
        const glm::vec2 texCoord = letterSize / hudText.textureSize;

        std::array<Vertex, 4> vertices{{
            { { +letterSize.x,             0 }, { texCoord.x, texCoord.y } },
            { {             0,             0 }, {          0, texCoord.y } },
            { { +letterSize.x, +letterSize.y }, { texCoord.x,          0 } },
            { {             0, +letterSize.y }, {          0,          0 } }
        }};

        std::array<int, 6> indices{{ 1,0,2,  1,3,2 }};

        std::vector<Vertex> letterVertices;
        letterVertices.reserve(indices.size()); // pre-allocate

        for (int index : indices)
            letterVertices.push_back(vertices[index]);

        graphic.geometries.hudText.letters.updateBuffer(0, letterVertices);
        graphic.geometries.hudText.letters.setPrimitiveCount(letterVertices.size());

    } // hud geometry ortho

    { // hud geometry perpective

        geometryBuilder
            .reset()
            .setShader(*graphic.shaders.simpleTexture)
            .setPrimitiveType(GL_TRIANGLES)
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

        std::array<Vertex, 4> quadVertices{{
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

    { // chessboard ground

        geometryBuilder
            .reset()
            .setShader(*graphic.shaders.simpleTexture)
            .setPrimitiveType(GL_TRIANGLES)
            .addVbo()
            .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
            .addVboAttribute("a_texCoord", Geometry::AttrType::Vec2f, 3);

        geometryBuilder.build(graphic.geometries.ground.chessboard);

        for (auto& cylinder : graphic.geometries.ground.cylinders)
            geometryBuilder.build(cylinder);

    } // chessboard ground

    { // model geometry

        geometryBuilder
            .reset()
            .setShader(*graphic.shaders.model)
            .setPrimitiveType(GL_TRIANGLES)
            .addVbo()
            .setVboStride(9 * 4)
            .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
            .addVboAttribute("a_color", Geometry::AttrType::Vec3f, 3)
            // .addVboAttribute("a_normal", Geometry::AttrType::Vec3f, 6)
            .addVbo()
            .setVboAsInstanced()
            .addVboAttribute("a_offsetTransform", Geometry::AttrType::Mat4f, 0)
            .addVboAttribute("a_offsetColor", Geometry::AttrType::Vec3f, 16);

        geometryBuilder.build(graphic.geometries.model.car);
        geometryBuilder.build(graphic.geometries.model.wheel);

        { // chassis geometry (instanced)

            std::vector<ModelVertex> modelVertices;
            loadCarModel(modelVertices);

            // {
            //     modelVertices.reserve(8 * 1024);

            //     auto addRectangle = [&modelVertices](
            //         const glm::vec3& center,
            //         const glm::vec3& size,
            //         const glm::vec3& color)
            //     {
            //         const glm::vec3 hsize = size * 0.5f;

            //         std::array<glm::vec3, 8> tmpVertices
            //         {{
            //             { center.x - hsize.x, center.y - hsize.y, center.z - hsize.z },
            //             { center.x + hsize.x, center.y - hsize.y, center.z - hsize.z },
            //             { center.x - hsize.x, center.y + hsize.y, center.z - hsize.z },
            //             { center.x + hsize.x, center.y + hsize.y, center.z - hsize.z },

            //             { center.x - hsize.x, center.y - hsize.y, center.z + hsize.z },
            //             { center.x + hsize.x, center.y - hsize.y, center.z + hsize.z },
            //             { center.x - hsize.x, center.y + hsize.y, center.z + hsize.z },
            //             { center.x + hsize.x, center.y + hsize.y, center.z + hsize.z },
            //         }};

            //         std::array<int, 36> indices
            //         {{
            //             0,1,2,  1,2,3, // bottom
            //             4,5,6,  5,6,7, // top

            //             1,3,5,  3,5,7, // right
            //             0,2,4,  2,4,6, // left

            //             0,1,4, 1,4,5, // back
            //             2,3,6, 3,6,7, // front
            //         }};

            //         for (int index : indices)
            //             modelVertices.push_back({ tmpVertices[index], color, glm::vec3(1) });
            //     };

            //     addRectangle({ 0, 0, 0.2f }, { 0.75f, 4, 0.5f }, glm::vec3(1));
            //     addRectangle({ 0, 0, 0.2f }, { 1.50f, 2, 0.4f }, glm::vec3(1));

            //     // addRectangle({ 0, -0.5f, 0.4f }, { 1, 1, 0.25 }, glm::vec3(1,0,0));

            //     addRectangle({ 0, -0.5f, 0.6f }, { 1, 1, 0.2 }, glm::vec3(0)); // cockpit
            // }

            graphic.geometries.model.car.updateBuffer(0, modelVertices);
            graphic.geometries.model.car.setPrimitiveCount(modelVertices.size());
        }

        { // wheel geometry (instanced)

            std::vector<ModelVertex> modelVertices;
            loadWheelModel(modelVertices);

            // {
            //     modelVertices.reserve(8 * 1024);
            //     const int quality = 16;
            //     for (int ii = 1; ii <= quality; ii += 2)
            //     {
            //         const float prevAngle = (float(ii - 1) / quality) * M_PI * 2;
            //         const float currAngle = (float(ii % quality) / quality) * M_PI * 2;

            //         constexpr float radius = 0.5f;

            //         std::array<glm::vec3, 4> tmpVertices
            //         {{
            //             { -0.25f, std::cos(prevAngle) * radius, std::sin(prevAngle) * radius },
            //             { +0.25f, std::cos(prevAngle) * radius, std::sin(prevAngle) * radius },
            //             { -0.25f, std::cos(currAngle) * radius, std::sin(currAngle) * radius },
            //             { +0.25f, std::cos(currAngle) * radius, std::sin(currAngle) * radius },
            //         }};

            //         std::array<int, 6> indices{{ 0,1,2,  1,2,3 }};
            //         for (int index : indices)
            //             modelVertices.push_back({ tmpVertices[index], glm::vec3(1), glm::vec3(1) });
            //     }
            // }

            graphic.geometries.model.wheel.updateBuffer(0, modelVertices);
            graphic.geometries.model.wheel.setPrimitiveCount(modelVertices.size());
        }

    } // model geometry

    { // wireframe

        geometryBuilder
            .reset()
            .setShader(*graphic.shaders.wireframes)
            .setPrimitiveType(GL_LINES)
            .addVbo()
            .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0);

        geometryBuilder.build(graphic.geometries.wireframes.circuitSkelton);

        //

        geometryBuilder
            .setPrimitiveType(GL_LINE_STRIP); // <= reused instead of reset

        for (auto& wheelsTrail : graphic.geometries.wireframes.bestNewCarsTrails)
            for (auto& wheel : wheelsTrail.wheels)
                geometryBuilder.build(wheel);

        geometryBuilder.build(graphic.geometries.wireframes.leaderCarTrail);

    } // wireframe

    { // animatedCircuit geometry

        geometryBuilder
            .reset()
            .setShader(*graphic.shaders.animatedCircuit)
            .setPrimitiveType(GL_TRIANGLES)
            .addVbo()
            .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
            .addVboAttribute("a_color", Geometry::AttrType::Vec3f, 3)
            .addVboAttribute("a_normal", Geometry::AttrType::Vec3f, 6)
            .addVboAttribute("a_index", Geometry::AttrType::Float, 9);

        geometryBuilder.build(graphic.geometries.animatedCircuit.ground);
        geometryBuilder.build(graphic.geometries.animatedCircuit.walls);

    } // animatedCircuit geometry
}
