
#include "Data.hpp"

#include "graphic/wrappers/Shader.hpp"

#include "demo/defines.hpp"


#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "thirdparty/tinyobjloader/tiny_obj_loader.h"


namespace /*anonymous*/
{

struct t_modelVertex
{
    glm::vec3	position;
    glm::vec3	color;
};

void loadModel(const std::string& filename, std::vector<t_modelVertex>& vertices);

void loadCarModel(std::vector<t_modelVertex>& vertices)
{
    loadModel("CarNoWheels.obj", vertices);
}

void loadWheelModel(std::vector<t_modelVertex>& vertices)
{
    loadModel("CarWheel.obj", vertices);
}

void loadModel(const std::string& filename, std::vector<t_modelVertex>& vertices)
{
    const std::string objFile = "./assets/model/" + filename;
    const std::string mtlDir = "./assets/model/";
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, objFile.c_str(), mtlDir.c_str());

    if (!err.empty())
        D_MYLOG(err);

    if (!ret)
    {
        D_MYLOG("LoadObj exit");
        exit(1);
    }

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
            std::size_t faceVerex = faceVertices[face];

            // per-face material
            int materialId = materialIds[face];

            const auto& diffuse = materials[materialId].diffuse;
            glm::vec3 color = { diffuse[0], diffuse[1], diffuse[2] };

            // Loop over vertices in the face.
            for (std::size_t vertex = 0; vertex < faceVerex; ++vertex)
            {
                // access to vertex
                tinyobj::index_t index = indices[indexOffset + vertex];
                tinyobj::real_t vx = attrib.vertices[3 * index.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3 * index.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3 * index.vertex_index + 2];
                // tinyobj::real_t nx = attrib.normals[3*index.normal_index+0];
                // tinyobj::real_t ny = attrib.normals[3*index.normal_index+1];
                // tinyobj::real_t nz = attrib.normals[3*index.normal_index+2];
                // tinyobj::real_t tx = attrib.texcoords[2*index.texcoord_index+0];
                // tinyobj::real_t ty = attrib.texcoords[2*index.texcoord_index+1];

                vertices.push_back({ { vx, vy, vz }, color });
            }
            indexOffset += faceVerex;
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

    static const std::vector<glm::vec3> positions = {
        {-X, N, Z}, { X, N, Z}, {-X, N,-Z}, { X, N,-Z},
        { N, Z, X}, { N, Z,-X}, { N,-Z, X}, { N,-Z,-X},
        { Z, X, N}, {-Z, X, N}, { Z,-X, N}, {-Z,-X, N}
    };

    static const std::vector<glm::ivec3> indices = {
        { 0, 4, 1}, { 0, 9, 4}, { 9, 5, 4}, { 4, 5, 8}, { 4, 8, 1},
        { 8,10, 1}, { 8, 3,10}, { 5, 3, 8}, { 5, 2, 3}, { 2, 7, 3},
        { 7,10, 3}, { 7, 6,10}, { 7,11, 6}, {11, 0, 6}, { 0, 1, 6},
        { 6, 1,10}, { 9, 0,11}, { 9,11, 2}, { 9, 2, 5}, { 7, 2,11}
    };

    vertices.reserve(indices.size() * 3);
    for (const glm::ivec3& index : indices) {
        vertices.push_back(positions[index.x]);
        vertices.push_back(positions[index.y]);
        vertices.push_back(positions[index.z]);
    }
}

};

void	Data::initialiseGeometries()
{
    // { // instanced geometries

    //     auto& shader = *graphic.shaders.instanced;

    //     Geometry::t_def::t_vbo vboGeometry;
    //     vboGeometry.attrs = {
    //         { "a_position", Geometry::e_attrType::eVec3f }
    //     };

    //     Geometry::t_def::t_vbo vboInstance;
    //     vboInstance.attrs = {
    //         { "a_transform", Geometry::e_attrType::eMat4f, 0 },
    //         { "a_color", Geometry::e_attrType::eVec4f, 16 }
    //     };
    //     vboInstance.instanced = true;

    //     Geometry::t_def geomDef = { { vboGeometry, vboInstance }, GL_LINES };
    //     // Geometry::t_def geomDef = { { vboGeometry, vboInstance }, GL_TRIANGLES };

    //     auto& instanced = graphic.geometries.instanced;

    //     { // chassis geometry (instanced)

    //         std::vector<glm::vec3> chassisVertices;

    //         glm::vec3 chassisSize = { 2.0f, 4.0f, 1.0f };
    //         generateCubeVerticesWireframe(chassisSize, chassisVertices);

    //         instanced.chassis.initialise(shader, geomDef);
    //         instanced.chassis.updateBuffer(0, chassisVertices);
    //         instanced.chassis.setPrimitiveCount(chassisVertices.size());
    //     }

    //     { // wheel geometry (instanced)

    //         const int	wheelQuality = 8;
    //         const float	wheelRadius = 0.5f;
    //         const float	wheelWidth = 0.2f;
    //         const float	wheelHWidth = wheelWidth * 0.5f;

    //         std::vector<glm::vec3> vertices;
    //         vertices.reserve(wheelQuality * 2 + 2);

    //         for (int ii = 0; ii < wheelQuality; ++ii)
    //         {
    //             float coef = float(ii) / wheelQuality;
    //             vertices.push_back({
    //                 -wheelHWidth,
    //                 wheelRadius * cosf(M_PI * 2.0f * coef),
    //                 wheelRadius * sinf(M_PI * 2.0f * coef)
    //             });
    //         }

    //         for (int ii = 0; ii < wheelQuality; ++ii)
    //         {
    //             float coef = float(ii) / wheelQuality;
    //             vertices.push_back({
    //                 +wheelHWidth,
    //                 wheelRadius * cosf(M_PI * 2.0f * coef),
    //                 wheelRadius * sinf(M_PI * 2.0f * coef)
    //             });
    //         }

    //         vertices.push_back(vertices[0]);
    //         vertices.push_back(vertices[wheelQuality]);

    //         vertices.push_back({ -wheelHWidth, 0, 0 });
    //         vertices.push_back({ +wheelHWidth, 0, 0 });

    //         //

    //         std::vector<int> indices;
    //         indices.reserve(wheelQuality * 2 + 2);

    //         // wheel side 1
    //         for (int ii = 0; ii < wheelQuality; ++ii)
    //         {
    //             indices.push_back(ii);
    //             indices.push_back((ii + 1) % wheelQuality);
    //         }

    //         // wheel side 2
    //         for (int ii = 0; ii < wheelQuality; ++ii)
    //         {
    //             indices.push_back(wheelQuality + ii);
    //             indices.push_back(wheelQuality + (ii + 1) % wheelQuality);
    //         }

    //         // wheel bridge
    //         indices.push_back(0);
    //         indices.push_back(wheelQuality);

    //         indices.push_back(0);
    //         indices.push_back(vertices.size() - 2);
    //         indices.push_back(wheelQuality);
    //         indices.push_back(vertices.size() - 1);

    //         //

    //         std::vector<glm::vec3> wheelVertices;
    //         wheelVertices.reserve(indices.size());

    //         for (int index : indices)
    //             wheelVertices.push_back(vertices[index]);

    //         instanced.wheels.initialise(shader, geomDef);
    //         instanced.wheels.updateBuffer(0, wheelVertices);
    //         instanced.wheels.setPrimitiveCount(wheelVertices.size());
    //     }

    // } // instanced geometries

    { // particles geometries

        auto& shader = *graphic.shaders.particles;

        Geometry::t_def::t_vbo vboGeometry;
        vboGeometry.attrs = {
            { "a_position", Geometry::e_attrType::eVec3f }
        };

        Geometry::t_def::t_vbo vboInstance;
        vboInstance.attrs = {
            { "a_offsetPosition", Geometry::e_attrType::eVec3f, 0 },
            { "a_offsetScale", Geometry::e_attrType::eFloat, 3 },
            { "a_offsetColor", Geometry::e_attrType::eVec4f, 4 }
        };
        vboInstance.instanced = true;

        Geometry::t_def geomDef = { { vboGeometry, vboInstance }, GL_TRIANGLES };

        auto& firework = graphic.geometries.particles.firework;

        { // chassis geometry (instanced)

            std::vector<glm::vec3> particlesVertices;

            // glm::vec3 particleSize = { 1.0f, 1.0f, 1.0f };
            // generateCubeVerticesFilled(particleSize, particlesVertices);
            generateSphereVerticesFilled(0.5f, particlesVertices);

            firework.initialise(shader, geomDef);
            firework.updateBuffer(0, particlesVertices);
            firework.setPrimitiveCount(particlesVertices.size());
        }

    } // particles geometries

    { // stack renderer geometry

        auto& shader = *graphic.shaders.stackRenderer;
        auto& geometry = graphic.geometries.stackRenderer.lines;

        Geometry::t_def::t_vbo vboGeometry;
        vboGeometry.attrs = {
            { "a_position", Geometry::e_attrType::eVec3f, 0 },
            { "a_color", Geometry::e_attrType::eVec3f, 3 },
        };

        Geometry::t_def geomDef = { { vboGeometry }, GL_LINES };

        geometry.initialise(shader, geomDef);
        geometry.setPrimitiveCount(0);

    } // stack renderer geometry

    { // hud geometry

        auto& shader = *graphic.shaders.hudText;
        auto& geometry = graphic.geometries.hudText.letters;

        Geometry::t_def::t_vbo vboGeometry;
        vboGeometry.attrs = {
            { "a_position", Geometry::e_attrType::eVec2f, 0 },
            { "a_texCoord", Geometry::e_attrType::eVec2f, 2 }
        };

        Geometry::t_def::t_vbo vboInstance;
        vboInstance.attrs = {
            { "a_offsetPosition", Geometry::e_attrType::eVec2f, 0 },
            { "a_offsetTexCoord", Geometry::e_attrType::eVec2f, 2 },
            { "a_offsetScale", Geometry::e_attrType::eFloat, 4 }
        };
        vboInstance.instanced = true;

        Geometry::t_def geomDef = { { vboGeometry, vboInstance }, GL_TRIANGLES };

        struct t_vertex
        {
            glm::vec2	position;
            glm::vec2	texCoord;
        };

        const auto&	hudText = graphic.hudText;
        const glm::vec2 letterSize = hudText.textureSize / hudText.gridSize;
        const glm::vec2 texCoord = letterSize / hudText.textureSize;

        std::array<t_vertex, 4> vertices{{
            { { +letterSize.x,             0 }, { texCoord.x, texCoord.y } },
            { {             0,             0 }, {          0, texCoord.y } },
            { { +letterSize.x, +letterSize.y }, { texCoord.x,          0 } },
            { {             0, +letterSize.y }, {          0,          0 } }
        }};

        std::array<int, 6> indices{{ 1,0,2,  1,3,2 }};

        std::vector<t_vertex> letterVertices;
        letterVertices.reserve(indices.size());

        for (int index : indices)
            letterVertices.push_back(vertices[index]);

        geometry.initialise(shader, geomDef);
        geometry.updateBuffer(0, letterVertices);
        geometry.setPrimitiveCount(letterVertices.size());

    } // hud geometry

    { // model geometry

        auto& shader = *graphic.shaders.model;

        Geometry::t_def::t_vbo vboGeometry;
        vboGeometry.attrs = {
            { "a_position", Geometry::e_attrType::eVec3f, 0 },
            { "a_color", Geometry::e_attrType::eVec3f, 3 },
        };

        Geometry::t_def::t_vbo vboInstance;
        vboInstance.attrs = {
            { "a_offsetTransform", Geometry::e_attrType::eMat4f, 0 },
            { "a_offsetColor", Geometry::e_attrType::eVec3f, 16 },
        };
        vboInstance.instanced = true;

        Geometry::t_def geomDef = { { vboGeometry, vboInstance }, GL_TRIANGLES };

        { // chassis geometry (instanced)

            auto& geometry = graphic.geometries.model.car;

            std::vector<t_modelVertex> modelVertices;

            loadCarModel(modelVertices);

            geometry.initialise(shader, geomDef);
            geometry.updateBuffer(0, modelVertices);
            geometry.setPrimitiveCount(modelVertices.size());
        }

        { // wheel geometry (instanced)

            auto& geometry = graphic.geometries.model.wheel;

            std::vector<t_modelVertex> modelVertices;

            loadWheelModel(modelVertices);

            geometry.initialise(shader, geomDef);
            geometry.updateBuffer(0, modelVertices);
            geometry.setPrimitiveCount(modelVertices.size());
        }

    } // model geometry

}
