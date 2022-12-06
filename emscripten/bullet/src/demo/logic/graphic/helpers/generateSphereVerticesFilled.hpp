
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <vector>

void generateSphereVerticesFilled(float radius, unsigned int quality,
                                  std::vector<glm::vec3>& vertices);
