
#include "loadObjModel.hpp"

#include "framework/ErrorHandler.hpp"

#include "framework/helpers/TinyObjLoader.hpp"

namespace loader
{

  void loadObjModel(
    const std::string& filename,
    const std::string& mtlDir,
    ModelVertices& vertices)
  {
    vertices.reserve(8 * 1024);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string errMsg;

    const bool loadingSuccess = tinyobj::LoadObj(
      &attrib,
      &shapes,
      &materials,
      &errMsg,
      filename.c_str(),
      mtlDir.c_str());

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
        std::size_t materialId = std::size_t(materialIds[face]);

        const auto& diffuse = materials[materialId].diffuse;
        const glm::vec3 color = { diffuse[0], diffuse[1], diffuse[2] };
        glm::vec3 position = { 0, 0, 0 };
        glm::vec3 normal = { 0, 0, 0 };

        // std::size_t prevIndex = vertices.size();

        // Loop over vertices in the face.
        for (std::size_t vertex = 0; vertex < faceVertex; ++vertex)
        {
          // access to vertex
          tinyobj::index_t index = indices[indexOffset + vertex];

          std::size_t vertexIndex = 3 * std::size_t(index.vertex_index);
          std::size_t normalIndex = 3 * std::size_t(index.normal_index);

          position.x = attrib.vertices[vertexIndex + 0];
          position.y = attrib.vertices[vertexIndex + 1];
          position.z = attrib.vertices[vertexIndex + 2];

          normal.x = attrib.normals[normalIndex + 0];
          normal.y = attrib.normals[normalIndex + 1];
          normal.z = attrib.normals[normalIndex + 2];

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

}
