
#include "headers.hpp"

namespace {

struct TestData {
  glm::vec3 eye;
  glm::vec3 center;
  glm::vec3 up;
};

std::array<TestData, 6> k_allTests = {{
  {{0, 0, 0}, {+1, 0, 0}, {0, 0, 1}},
  {{0, 0, 0}, {0, +1, 0}, {0, 0, 1}},
  {{0, 0, 0}, {0, 0, +1}, {0, 1, 0}},
  {{0, 0, 0}, {-1, 0, 0}, {0, 0, 1}},
  {{0, 0, 0}, {0, -1, 0}, {0, 0, 1}},
  {{0, 0, 0}, {0, 0, -1}, {0, 1, 0}},
}};

std::array<glm::vec3, 6> k_longDirections{{
  glm::vec3(+200, 0, 0),
  glm::vec3(0, +200, 0),
  glm::vec3(0, 0, +200),
  glm::vec3(-200, 0, 0),
  glm::vec3(0, -200, 0),
  glm::vec3(0, 0, -200),
}};

std::array<glm::vec3, 6> k_shortDirections{{
  glm::vec3(+40, 0, 0),
  glm::vec3(0, +40, 0),
  glm::vec3(0, 0, +40),
  glm::vec3(-40, 0, 0),
  glm::vec3(0, -40, 0),
  glm::vec3(0, 0, -40),
}};

constexpr int k_range = 3;
constexpr float k_step = 10000.0f;

} // namespace

TEST(frustum_culling,
     confirm_under_multiple_angles_the_point_in_frustum_feature) {
  for (int zz = -k_range; zz <= k_range; ++zz)
    for (int yy = -k_range; yy <= k_range; ++yy)
      for (int xx = -k_range; xx <= k_range; ++xx) {
        glm::vec3 currOffset = glm::vec3(xx, yy, zz) * k_step;
        for (const TestData& baseData : k_allTests) {
          TestData currData;
          currData.eye = baseData.eye + currOffset;
          currData.center = baseData.center + currOffset;
          currData.up = baseData.up;

          const glm::vec3 forward =
            glm::normalize(baseData.center - baseData.eye);

          const glm::mat4 proj =
            glm::perspective(glm::radians(70.0f), 1.0f, 0.1f, 100.0f);
          const glm::mat4 view =
            glm::lookAt(currData.eye, currData.center, currData.up);

          FrustumCulling frustumCulling;
          frustumCulling.calculateFrustum(proj, view);

          //

          for (glm::vec3 dir : k_longDirections)
            EXPECT_EQ(frustumCulling.pointInFrustum(currOffset + dir), false);
          EXPECT_EQ(frustumCulling.pointInFrustum(currOffset + forward * 10.0f),
                    true);
          EXPECT_EQ(frustumCulling.pointInFrustum(currOffset + forward * 90.0f),
                    true);
        }
      }
}

TEST(frustum_culling,
     confirm_under_multiple_angles_the_sphere_in_frustum_feature) {
  for (int zz = -k_range; zz <= k_range; ++zz)
    for (int yy = -k_range; yy <= k_range; ++yy)
      for (int xx = -k_range; xx <= k_range; ++xx) {
        glm::vec3 currOffset = glm::vec3(xx, yy, zz) * k_step;
        for (const TestData& baseData : k_allTests) {
          TestData currData;
          currData.eye = baseData.eye + currOffset;
          currData.center = baseData.center + currOffset;
          currData.up = baseData.up;

          const glm::vec3 forward =
            glm::normalize(baseData.center - baseData.eye);

          const glm::mat4 proj =
            glm::perspective(glm::radians(70.0f), 1.0f, 0.1f, 100.0f);
          const glm::mat4 view =
            glm::lookAt(currData.eye, currData.center, currData.up);

          FrustumCulling frustumCulling;
          frustumCulling.calculateFrustum(proj, view);

          //

          for (glm::vec3 dir : k_longDirections)
            EXPECT_EQ(frustumCulling.sphereInFrustum(currOffset + dir, 50.0f),
                      false);
          for (glm::vec3 dir : k_shortDirections)
            EXPECT_EQ(frustumCulling.sphereInFrustum(currOffset + dir, 50.0f),
                      true);
          EXPECT_EQ(
            frustumCulling.sphereInFrustum(currOffset + forward * 10.0f, 20.0f),
            true);
          EXPECT_EQ(
            frustumCulling.sphereInFrustum(currOffset + forward * 90.0f, 20.0f),
            true);
        }
      }
}

TEST(frustum_culling,
     confirm_under_multiple_angles_the_cube_in_frustum_feature) {
  for (int zz = -k_range; zz <= k_range; ++zz)
    for (int yy = -k_range; yy <= k_range; ++yy)
      for (int xx = -k_range; xx <= k_range; ++xx) {
        glm::vec3 currOffset = glm::vec3(xx, yy, zz) * k_step;
        for (const TestData& baseData : k_allTests) {
          TestData currData;
          currData.eye = baseData.eye + currOffset;
          currData.center = baseData.center + currOffset;
          currData.up = baseData.up;

          const glm::vec3 forward =
            glm::normalize(baseData.center - baseData.eye);

          const glm::mat4 proj =
            glm::perspective(glm::radians(70.0f), 1.0f, 0.1f, 100.0f);
          const glm::mat4 view =
            glm::lookAt(currData.eye, currData.center, currData.up);

          FrustumCulling frustumCulling;
          frustumCulling.calculateFrustum(proj, view);

          //

          for (glm::vec3 dir : k_longDirections)
            EXPECT_EQ(
              frustumCulling.cubeInFrustum(currOffset + dir, glm::vec3(20.0f)),
              false);
          for (glm::vec3 dir : k_shortDirections)
            EXPECT_EQ(
              frustumCulling.cubeInFrustum(currOffset + dir, glm::vec3(100.0f)),
              true);
          EXPECT_EQ(frustumCulling.cubeInFrustum(currOffset + forward * 10.0f,
                                                 glm::vec3(20.0f)),
                    true);
          EXPECT_EQ(frustumCulling.cubeInFrustum(currOffset + forward * 90.0f,
                                                 glm::vec3(20.0f)),
                    true);
        }
      }
}
