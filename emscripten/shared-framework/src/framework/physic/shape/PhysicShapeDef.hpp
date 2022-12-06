
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <cstdint>
#include <memory>
#include <vector>

struct PhysicShapeDef {
  PhysicShapeDef() = default;

  enum class Type {
    capsule = 0,
    box,
    sphere,
    staticMesh,
    compound,
  } type;

  struct Data {
    struct Box {
      glm::vec3 size = {0, 0, 0};
    } box;

    struct Sphere {
      float radius = 0.0f;
    } sphere;

    struct CapsuleShape {
      float radius = 0.0f;
      float height = 0.0f;
    } capsule;

    struct StaticMesh {
      glm::vec3* verticesData = nullptr; // will be allocated and copied
      std::size_t verticesLength = 0;

      int32_t* indicesData = nullptr; // will be allocated and copied
      std::size_t indicesLength = 0;
    } staticMesh;

    struct Compound {
      struct ChildShape {
        glm::mat4 transform;
        std::shared_ptr<PhysicShapeDef> shape;
      };

      std::vector<ChildShape> childShapes;
    } compound;
  } data;
};
