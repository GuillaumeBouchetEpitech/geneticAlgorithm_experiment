
#include "Data.hpp"

#include "graphic/wrappers/Shader.hpp"
#include "graphic/wrappers/GeometryBuilder.hpp"

#include "demo/defines.hpp"


#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "thirdparty/tinyobjloader/tiny_obj_loader.h"


namespace /*anonymous*/
{

struct t_modelVertex
{
    glm::vec3 position;
    glm::vec3 color;
};

void loadModel(const std::string& filename, std::vector<t_modelVertex>& vertices); // declaration

void loadCarModel(std::vector<t_modelVertex>& vertices)
{
    loadModel("CarNoWheels.obj", vertices);
}

void loadWheelModel(std::vector<t_modelVertex>& vertices)
{
    loadModel("CarWheel.obj", vertices);
}

void loadModel(const std::string& filename, std::vector<t_modelVertex>& vertices) // definition
{
    const std::string objFile = "./assets/model/" + filename;
    const std::string mtlDir = "./assets/model/";

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

    vertices.reserve(indices.size() * 3); // pre-allocate
    for (const glm::ivec3& index : indices) {
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

    } // particles geometries

    { // stack renderer geometry

        { // lines

            geometryBuilder
                .reset()
                .setShader(*graphic.shaders.stackRenderer)
                .setPrimitiveType(GL_LINES)
                .addVbo()
                .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
                .addVboAttribute("a_color", Geometry::AttrType::Vec3f, 3);

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
                .addVboAttribute("a_color", Geometry::AttrType::Vec3f, 3);

            geometryBuilder.build(graphic.geometries.stackRenderer.triangles);

            graphic.geometries.stackRenderer.triangles.setPrimitiveCount(0);

        } // triangles

    } // stack renderer geometry

    { // hud geometry

        geometryBuilder
            .reset()
            .setShader(*graphic.shaders.hudText)
            .setPrimitiveType(GL_TRIANGLES)
            .addVbo()
            .addVboAttribute("a_position", Geometry::AttrType::Vec2f, 0)
            .addVboAttribute("a_texCoord", Geometry::AttrType::Vec2f, 2)
            .addVbo()
            .setVboAsInstanced()
            .addVboAttribute("a_offsetPosition", Geometry::AttrType::Vec2f, 0)
            .addVboAttribute("a_offsetTexCoord", Geometry::AttrType::Vec2f, 2)
            .addVboAttribute("a_offsetScale", Geometry::AttrType::Float, 4);

        geometryBuilder.build(graphic.geometries.hudText.letters);

        struct t_vertex
        {
            glm::vec2 position;
            glm::vec2 texCoord;
        };

        const auto& hudText = graphic.hudText;
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
        letterVertices.reserve(indices.size()); // pre-allocate

        for (int index : indices)
            letterVertices.push_back(vertices[index]);

        graphic.geometries.hudText.letters.updateBuffer(0, letterVertices);
        graphic.geometries.hudText.letters.setPrimitiveCount(letterVertices.size());

    } // hud geometry

    { // model geometry

        geometryBuilder
            .reset()
            .setShader(*graphic.shaders.model)
            .setPrimitiveType(GL_TRIANGLES)
            .addVbo()
            .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
            .addVboAttribute("a_color", Geometry::AttrType::Vec3f, 3)
            .addVbo()
            .setVboAsInstanced()
            .addVboAttribute("a_offsetTransform", Geometry::AttrType::Mat4f, 0)
            .addVboAttribute("a_offsetColor", Geometry::AttrType::Vec3f, 16);

        geometryBuilder.build(graphic.geometries.model.car);
        geometryBuilder.build(graphic.geometries.model.wheel);

        { // chassis geometry (instanced)

            std::vector<t_modelVertex> modelVertices;
            loadCarModel(modelVertices);

            graphic.geometries.model.car.updateBuffer(0, modelVertices);
            graphic.geometries.model.car.setPrimitiveCount(modelVertices.size());
        }

        { // wheel geometry (instanced)

            std::vector<t_modelVertex> modelVertices;
            loadWheelModel(modelVertices);

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
            for (unsigned int ii = 0; ii < wheelsTrail.wheels.size(); ++ii)
                geometryBuilder.build(wheelsTrail.wheels[ii]);

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
