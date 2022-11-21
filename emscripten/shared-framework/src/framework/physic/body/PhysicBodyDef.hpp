
#pragma once

#include "framework/physic/shape/PhysicShapeDef.hpp"

struct PhysicBodyDef
{
  PhysicShapeDef shape;

  float mass = 0.0f;

  short group = -1;
  short mask = -1;

  // float continuous_radius = 0.0f;
};

// class PhysicBody;

// class btRigidBody;

// enum class ContactEvent
// {
//   BEGIN = 0,
//   UPDATE,
//   END,
// };

// struct PhysicContactData
// {
//   unsigned short id = 0;

//   btRigidBody* pBodyA = nullptr;
//   btRigidBody* pBodyB = nullptr;

//   glm::vec3 posB;
//   glm::vec3 normalB;
// };

// struct ContactPoint
// {
//   unsigned short id = 0;

//   glm::vec3 position;
//   glm::vec3 normal;
//   PhysicBody* other;

//   // ContactPoint(const glm::vec3& pos, const glm::vec3& n, PhysicBody* o)
//   //     : position(pos)
//   //     , normal(n)
//   //     , other(o)
//   // {}
// };
