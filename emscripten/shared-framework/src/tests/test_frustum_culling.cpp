
#include "framework/graphic/camera/FrustumCulling.hpp"

#include "framework/TraceLogger.hpp"

#include "framework/helpers/GLMath.hpp"

#include <cassert>
#include <array>

void test_frustum_culling()
{
  D_MYLOG("test_frustum_culling()");

  struct TestData
  {
    glm::vec3 eye;
    glm::vec3 center;
    glm::vec3 up;
  };

  std::array<TestData, 6> allTests =
  {{
    { { 0, 0, 0 }, {+1, 0, 0 }, { 0, 0, 1 } },
    { { 0, 0, 0 }, { 0,+1, 0 }, { 0, 0, 1 } },
    { { 0, 0, 0 }, { 0, 0,+1 }, { 0, 1, 0 } },
    { { 0, 0, 0 }, {-1, 0, 0 }, { 0, 0, 1 } },
    { { 0, 0, 0 }, { 0,-1, 0 }, { 0, 0, 1 } },
    { { 0, 0, 0 }, { 0, 0,-1 }, { 0, 1, 0 } },
  }};

  std::vector<glm::vec3> allOffsets;
  allOffsets.reserve(1024);
  constexpr int k_range = 3;
  constexpr float k_step = 10000.0f;
  for (int zz = -k_range; zz <= k_range; ++zz)
  for (int yy = -k_range; yy <= k_range; ++yy)
  for (int xx = -k_range; xx <= k_range; ++xx)
    allOffsets.push_back(glm::vec3(xx, yy, zz) * k_step);

  for (const glm::vec3& currOffset : allOffsets)
  for (const TestData& baseData : allTests)
  {
    TestData data;
    data.eye = baseData.eye + currOffset;
    data.center = baseData.center + currOffset;
    data.up = baseData.up;

    const glm::vec3 forward = glm::normalize(baseData.center - baseData.eye);

    FrustumCulling frustumCulling;

    const glm::mat4 proj = glm::perspective(glm::radians(70.0f), 1.0f, 0.1f, 100.0f);
    const glm::mat4 view = glm::lookAt(data.eye, data.center, data.up);

    frustumCulling.calculateFrustum(proj, view);

    assert(frustumCulling.pointInFrustum(currOffset + glm::vec3(+200, 0, 0)) == false);
    assert(frustumCulling.pointInFrustum(currOffset + glm::vec3(0, +200, 0)) == false);
    assert(frustumCulling.pointInFrustum(currOffset + glm::vec3(0, 0, +200)) == false);
    assert(frustumCulling.pointInFrustum(currOffset + glm::vec3(-200, 0, 0)) == false);
    assert(frustumCulling.pointInFrustum(currOffset + glm::vec3(0, -200, 0)) == false);
    assert(frustumCulling.pointInFrustum(currOffset + glm::vec3(0, 0, -200)) == false);

    assert(frustumCulling.pointInFrustum(currOffset + forward * 10.0f) == true);
    assert(frustumCulling.pointInFrustum(currOffset + forward * 90.0f) == true);

    //

    assert(frustumCulling.sphereInFrustum(currOffset + glm::vec3(+200, 0, 0), 20.0f) == false);
    assert(frustumCulling.sphereInFrustum(currOffset + glm::vec3(0, +200, 0), 20.0f) == false);
    assert(frustumCulling.sphereInFrustum(currOffset + glm::vec3(0, 0, +200), 20.0f) == false);
    assert(frustumCulling.sphereInFrustum(currOffset + glm::vec3(-200, 0, 0), 20.0f) == false);
    assert(frustumCulling.sphereInFrustum(currOffset + glm::vec3(0, -200, 0), 20.0f) == false);
    assert(frustumCulling.sphereInFrustum(currOffset + glm::vec3(0, 0, -200), 20.0f) == false);

    assert(frustumCulling.sphereInFrustum(currOffset + glm::vec3(+40, 0, 0), 50.0f) == true);
    assert(frustumCulling.sphereInFrustum(currOffset + glm::vec3(0, +40, 0), 50.0f) == true);
    assert(frustumCulling.sphereInFrustum(currOffset + glm::vec3(0, 0, +40), 50.0f) == true);
    assert(frustumCulling.sphereInFrustum(currOffset + glm::vec3(-40, 0, 0), 50.0f) == true);
    assert(frustumCulling.sphereInFrustum(currOffset + glm::vec3(0, -40, 0), 50.0f) == true);
    assert(frustumCulling.sphereInFrustum(currOffset + glm::vec3(0, 0, -40), 50.0f) == true);
    assert(frustumCulling.sphereInFrustum(currOffset + forward * 10.0f, 20.0f) == true);
    assert(frustumCulling.sphereInFrustum(currOffset + forward * 10.0f, 20.0f) == true);

    //

    assert(frustumCulling.cubeInFrustum(currOffset + glm::vec3(+200, 0, 0), glm::vec3(20.0f)) == false);
    assert(frustumCulling.cubeInFrustum(currOffset + glm::vec3(0, +200, 0), glm::vec3(20.0f)) == false);
    assert(frustumCulling.cubeInFrustum(currOffset + glm::vec3(0, 0, +200), glm::vec3(20.0f)) == false);
    assert(frustumCulling.cubeInFrustum(currOffset + glm::vec3(-200, 0, 0), glm::vec3(20.0f)) == false);
    assert(frustumCulling.cubeInFrustum(currOffset + glm::vec3(0, -200, 0), glm::vec3(20.0f)) == false);
    assert(frustumCulling.cubeInFrustum(currOffset + glm::vec3(0, 0, -200), glm::vec3(20.0f)) == false);

    assert(frustumCulling.cubeInFrustum(currOffset + glm::vec3(+40, 0, 0), glm::vec3(100.0f)) == true);
    assert(frustumCulling.cubeInFrustum(currOffset + glm::vec3(0, +40, 0), glm::vec3(100.0f)) == true);
    assert(frustumCulling.cubeInFrustum(currOffset + glm::vec3(0, 0, +40), glm::vec3(100.0f)) == true);
    assert(frustumCulling.cubeInFrustum(currOffset + glm::vec3(-40, 0, 0), glm::vec3(100.0f)) == true);
    assert(frustumCulling.cubeInFrustum(currOffset + glm::vec3(0, -40, 0), glm::vec3(100.0f)) == true);
    assert(frustumCulling.cubeInFrustum(currOffset + glm::vec3(0, 0, -40), glm::vec3(100.0f)) == true);
    assert(frustumCulling.cubeInFrustum(currOffset + forward * 10.0f, glm::vec3(20.0f)) == true);
    assert(frustumCulling.cubeInFrustum(currOffset + forward * 90.0f, glm::vec3(20.0f)) == true);

  }


  D_MYLOG(" => DONE");
}

