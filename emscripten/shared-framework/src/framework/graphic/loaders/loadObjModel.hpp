
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <string>
#include <vector>

namespace loader {

struct ModelVertex {
  glm::vec3 position;
  glm::vec3 color;
  glm::vec3 normal;
};

using ModelVertices = std::vector<ModelVertex>;

void loadObjModel(const std::string& filename, const std::string& mtlDir,
                  ModelVertices& vertices);

} // namespace loader
