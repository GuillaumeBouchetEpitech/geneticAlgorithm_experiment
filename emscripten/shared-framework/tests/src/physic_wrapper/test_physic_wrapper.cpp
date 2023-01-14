
#include "headers.hpp"

// #include "generic_array_container_commons/common.hpp"

// namespace {

// struct generic_array_container_constructor
//   : public common::threadsafe_fixture
// {};

// }

TEST(physic_wrapper, dynamic_object_is_affected_by_gravity) {
  PhysicWorld world;

  const float frameTime = 1.0f / 60.0f;

  PhysicBodyDef bodyDef;
  bodyDef.shape.type = PhysicShapeDef::Type::sphere;
  bodyDef.shape.data.sphere.radius = 0.5f;
  bodyDef.mass = 1.0f;
  auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
  bodyRef->setPosition({0, 0, 10});
  world.getPhysicBodyManager().addBody(bodyRef, -1, -1);

  for (int ii = 0; ii < 100; ++ii)
    world.step(frameTime, 1, frameTime);

  EXPECT_LT(bodyRef->getPosition().z, 0);
}

TEST(physic_wrapper, static_object_is_not_affected_by_gravity) {
  PhysicWorld world;

  const float frameTime = 1.0f / 60.0f;

  PhysicBodyDef bodyDef;
  bodyDef.shape.type = PhysicShapeDef::Type::sphere;
  bodyDef.shape.data.sphere.radius = 0.5f;
  bodyDef.mass = 0.0f;
  auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
  bodyRef->setPosition({0, 0, 10});
  world.getPhysicBodyManager().addBody(bodyRef, -1, -1);

  for (int ii = 0; ii < 100; ++ii)
    world.step(frameTime, 1, frameTime);

  EXPECT_EQ(bodyRef->getPosition().z, 10);
}

TEST(physic_wrapper, dynamic_object_can_collide_with_static_object) {
  PhysicWorld world;

  const float frameTime = 1.0f / 60.0f;

  PhysicBodyDef bodyDef_1;
  bodyDef_1.shape.type = PhysicShapeDef::Type::sphere;
  bodyDef_1.shape.data.sphere.radius = 0.5f;
  bodyDef_1.mass = 1.0f;
  auto bodyRef_1 = world.getPhysicBodyManager().createBody(bodyDef_1);
  bodyRef_1->setPosition({0, 0, 10});
  world.getPhysicBodyManager().addBody(bodyRef_1, -1, -1);

  PhysicBodyDef bodyDef_2;
  bodyDef_2.shape.type = PhysicShapeDef::Type::box;
  bodyDef_2.shape.data.box.size = {20, 20, 0.5f};
  bodyDef_2.mass = 0.0f;
  auto bodyRef_2 = world.getPhysicBodyManager().createBody(bodyDef_2);
  bodyRef_2->setPosition({0, 0, 0});
  world.getPhysicBodyManager().addBody(bodyRef_2, -1, -1);

  for (int ii = 0; ii < 100; ++ii)
    world.step(frameTime, 1, frameTime);

  EXPECT_LT(bodyRef_1->getPosition().z, 10);
  EXPECT_GT(bodyRef_1->getPosition().z, 0);
}

TEST(
  physic_wrapper,
  dynamic_object_cannot_collide_with_static_object_if_set_with_non_matching_collision_filter) {
  PhysicWorld world;

  const float frameTime = 1.0f / 60.0f;

  PhysicBodyDef bodyDef_1;
  bodyDef_1.shape.type = PhysicShapeDef::Type::sphere;
  bodyDef_1.shape.data.sphere.radius = 0.5f;
  bodyDef_1.mass = 1.0f;
  bodyDef_1.mask = 1;
  bodyDef_1.group = 1;
  auto bodyRef_1 = world.getPhysicBodyManager().createBody(bodyDef_1);
  bodyRef_1->setPosition({0, 0, 10});
  bodyRef_1->setUserValue(1111);
  world.getPhysicBodyManager().addBody(bodyRef_1, 1, 1);

  PhysicBodyDef bodyDef_2;
  bodyDef_2.shape.type = PhysicShapeDef::Type::box;
  bodyDef_2.shape.data.box.size = {20, 20, 0.5f};
  bodyDef_2.mass = 0.0f;
  bodyDef_2.mask = 8;
  bodyDef_2.group = 8;
  auto bodyRef_2 = world.getPhysicBodyManager().createBody(bodyDef_2);
  bodyRef_2->setPosition({0, 0, 0});
  bodyRef_2->setUserValue(2222);
  world.getPhysicBodyManager().addBody(bodyRef_2, 8, 8);

  for (int ii = 0; ii < 100; ++ii)
    world.step(frameTime, 1, frameTime);

  EXPECT_LT(bodyRef_1->getPosition().z, 0);
}

TEST(physic_wrapper, test_all_shapes) {
  std::array<glm::vec3, 4> k_rawVertices = {{
    glm::vec3(0, 0, 0),
    glm::vec3(1, 0, 0),
    glm::vec3(1, 1, 0),
    glm::vec3(0, 1, 0),
  }};
  std::array<int32_t, 6> k_rawIndices = {{1, 2, 3, 1, 0, 3}};

  {

    PhysicWorld world;

    const float frameTime = 1.0f / 60.0f;

    {
      PhysicBodyDef bodyDef;
      bodyDef.shape.type = PhysicShapeDef::Type::sphere;
      bodyDef.shape.data.sphere.radius = 0.5f;
      bodyDef.mass = 0.0f;
      bodyDef.mask = 1;
      bodyDef.group = 1;
      auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
      bodyRef->setPosition({0, 0, 10});
      // bodyRef->setUserValue(1111);
      world.getPhysicBodyManager().addBody(bodyRef, bodyDef.group,
                                           bodyDef.mask);
    }

    {
      PhysicBodyDef bodyDef;
      bodyDef.shape.type = PhysicShapeDef::Type::box;
      bodyDef.shape.data.box.size = {0.5f, 0.5f, 0.5f};
      bodyDef.mass = 0.0f;
      bodyDef.mask = 1;
      bodyDef.group = 1;
      auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
      bodyRef->setPosition({0, 0, 10});
      // bodyRef->setUserValue(1111);
      world.getPhysicBodyManager().addBody(bodyRef, bodyDef.group,
                                           bodyDef.mask);
    }

    {
      PhysicBodyDef bodyDef;
      bodyDef.shape.type = PhysicShapeDef::Type::capsule;
      bodyDef.shape.data.capsule.radius = 0.5f;
      bodyDef.shape.data.capsule.height = 1.0f;
      bodyDef.mass = 0.0f;
      bodyDef.mask = 1;
      bodyDef.group = 1;
      auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
      bodyRef->setPosition({0, 0, 10});
      // bodyRef->setUserValue(1111);
      world.getPhysicBodyManager().addBody(bodyRef, bodyDef.group,
                                           bodyDef.mask);
    }

    {
      PhysicBodyDef bodyDef;

      bodyDef.shape.type = PhysicShapeDef::Type::staticMesh;
      bodyDef.shape.data.staticMesh.verticesData = k_rawVertices.data();
      bodyDef.shape.data.staticMesh.verticesLength = k_rawVertices.size();
      bodyDef.shape.data.staticMesh.indicesData = k_rawIndices.data();
      bodyDef.shape.data.staticMesh.indicesLength = k_rawIndices.size();

      bodyDef.mass = 0.0f;
      bodyDef.mask = 1;
      bodyDef.group = 1;
      auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
      bodyRef->setPosition({0, 0, 10});
      // bodyRef->setUserValue(1111);
      world.getPhysicBodyManager().addBody(bodyRef, bodyDef.group,
                                           bodyDef.mask);
    }

    {
      PhysicBodyDef bodyDef;
      bodyDef.shape.type = PhysicShapeDef::Type::compound;

      {
        auto shapeDef = std::make_shared<PhysicShapeDef>();
        shapeDef->type = PhysicShapeDef::Type::sphere;
        shapeDef->data.sphere.radius = 0.5f;
        bodyDef.shape.data.compound.childShapes.push_back(
          {glm::identity<glm::mat4>(), shapeDef});
      }
      {
        auto shapeDef = std::make_shared<PhysicShapeDef>();
        shapeDef->type = PhysicShapeDef::Type::box;
        shapeDef->data.box.size = {0.5f, 0.5f, 0.5f};
        bodyDef.shape.data.compound.childShapes.push_back(
          {glm::identity<glm::mat4>(), shapeDef});
      }
      {
        auto shapeDef = std::make_shared<PhysicShapeDef>();
        shapeDef->type = PhysicShapeDef::Type::capsule;
        shapeDef->data.capsule.radius = 0.5f;
        shapeDef->data.capsule.height = 1.0f;
        bodyDef.shape.data.compound.childShapes.push_back(
          {glm::identity<glm::mat4>(), shapeDef});
      }
      {
        auto shapeDef = std::make_shared<PhysicShapeDef>();
        shapeDef->type = PhysicShapeDef::Type::capsule;
        shapeDef->data.capsule.radius = 0.5f;
        shapeDef->data.capsule.height = 1.0f;
        bodyDef.shape.data.compound.childShapes.push_back(
          {glm::identity<glm::mat4>(), shapeDef});
      }
      {
        auto shapeDef = std::make_shared<PhysicShapeDef>();
        shapeDef->type = PhysicShapeDef::Type::staticMesh;
        shapeDef->data.staticMesh.verticesData = k_rawVertices.data();
        shapeDef->data.staticMesh.verticesLength = k_rawVertices.size();
        shapeDef->data.staticMesh.indicesData = k_rawIndices.data();
        shapeDef->data.staticMesh.indicesLength = k_rawIndices.size();
        bodyDef.shape.data.compound.childShapes.push_back(
          {glm::identity<glm::mat4>(), shapeDef});
      }
      {
        auto shapeDef = std::make_shared<PhysicShapeDef>();
        shapeDef->type = PhysicShapeDef::Type::compound;
        {
          auto subShapeDef = std::make_shared<PhysicShapeDef>();
          subShapeDef->type = PhysicShapeDef::Type::sphere;
          subShapeDef->data.sphere.radius = 0.5f;
          shapeDef->data.compound.childShapes.push_back(
            {glm::identity<glm::mat4>(), subShapeDef});
        }
        bodyDef.shape.data.compound.childShapes.push_back(
          {glm::identity<glm::mat4>(), shapeDef});
      }

      bodyDef.mass = 0.0f;
      bodyDef.mask = 1;
      bodyDef.group = 1;
      auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
      bodyRef->setPosition({0, 0, 10});
      // bodyRef->setUserValue(1111);
      world.getPhysicBodyManager().addBody(bodyRef, bodyDef.group,
                                           bodyDef.mask);
    }

    PhysicBodyDef bodyDef_2;
    bodyDef_2.shape.type = PhysicShapeDef::Type::box;
    bodyDef_2.shape.data.box.size = {20, 20, 0.5f};
    bodyDef_2.mass = 0.0f;
    bodyDef_2.mask = 8;
    bodyDef_2.group = 8;
    auto bodyRef_2 = world.getPhysicBodyManager().createBody(bodyDef_2);
    bodyRef_2->setPosition({0, 0, 0});
    bodyRef_2->setUserValue(2222);
    world.getPhysicBodyManager().addBody(bodyRef_2, 8, 8);

    for (int ii = 0; ii < 100; ++ii)
      world.step(frameTime, 1, frameTime);
  }
}
