
#include "headers.hpp"

TEST(circleCircleIntersectionPoints, find_90_degrees_positions) {
  Circle c1;
  c1.center.x = 0;
  c1.center.y = 0;
  c1.radius = 4;

  Circle c2;
  c2.center.x = -4;
  c2.center.y = -4;
  c2.radius = 4;

  glm::vec2 res1;
  glm::vec2 res2;

  const bool result = circleCircleIntersectionPoints(c1, c2, res1, res2);

  EXPECT_EQ(result, true);

  EXPECT_NEAR(res1.x, -4.0f, 0.01f);
  EXPECT_NEAR(res1.y, 0.0f, 0.01f);
  const glm::vec2 res1ParentDiff = res1 - c1.center;
  const float res1ParentAngle = std::atan2(res1ParentDiff.y, res1ParentDiff.x);
  EXPECT_NEAR(glm::degrees(res1ParentAngle), 180.0f, 0.01f);

  EXPECT_NEAR(res2.x, 0.0f, 0.01f);
  EXPECT_NEAR(res2.y, -4.0f, 0.01f);
  const glm::vec2 res2ParentDiff = res2 - c1.center;
  const float res2ParentAngle = std::atan2(res2ParentDiff.y, res2ParentDiff.x);
  EXPECT_NEAR(glm::degrees(res2ParentAngle), -90.0f, 0.01f);
}

TEST(circleCircleIntersectionPoints, find_straigth_positions) {
  Circle c1;
  c1.center.x = 0;
  c1.center.y = 0;
  c1.radius = 4;

  Circle c2;
  c2.center.x = 8;
  c2.center.y = 0;
  c2.radius = 4;

  // c2.center.x = -8;
  // c2.center.y = 0;

  // c2.center.x = 4;
  // c2.center.y = 4;

  // c2.center.x = -4;
  // c2.center.y = -4;

  glm::vec2 res1;
  glm::vec2 res2;

  const bool result = circleCircleIntersectionPoints(c1, c2, res1, res2);

  EXPECT_EQ(result, true);

  EXPECT_NEAR(res1.x, 4.0f, 0.01f);
  EXPECT_NEAR(res1.y, 0.0f, 0.01f);
  const glm::vec2 res1ParentDiff = res1 - c1.center;
  const float res1ParentAngle = std::atan2(res1ParentDiff.y, res1ParentDiff.x);
  EXPECT_NEAR(glm::degrees(res1ParentAngle), 0.0f, 0.01f);

  EXPECT_NEAR(res2.x, 4.0f, 0.01f);
  EXPECT_NEAR(res2.y, 0.0f, 0.01f);
  const glm::vec2 res2ParentDiff = res2 - c1.center;
  const float res2ParentAngle = std::atan2(res2ParentDiff.y, res2ParentDiff.x);
  EXPECT_NEAR(glm::degrees(res2ParentAngle), 0.0f, 0.01f);
}

TEST(circleCircleIntersectionPoints,
     find_90_degrees_positions_with_different_radius) {
  Circle c1;
  c1.center.x = 0;
  c1.center.y = 0;
  c1.radius = 5;

  Circle c2;
  c2.center.x = 5;
  c2.center.y = 3;
  c2.radius = 3;

  glm::vec2 res1;
  glm::vec2 res2;

  const bool result = circleCircleIntersectionPoints(c1, c2, res1, res2);

  EXPECT_EQ(result, true);

  EXPECT_NEAR(res1.x, 5.0f, 0.01f);
  EXPECT_NEAR(res1.y, 0.0f, 0.01f);
  const glm::vec2 res1ParentDiff = res1 - c1.center;
  const float res1ParentAngle = std::atan2(res1ParentDiff.y, res1ParentDiff.x);
  EXPECT_NEAR(glm::degrees(res1ParentAngle), 0.0f, 0.01f);

  EXPECT_NEAR(res2.x, 2.35f, 0.01f);
  EXPECT_NEAR(res2.y, 4.41f, 0.01f);
  const glm::vec2 res2ParentDiff = res2 - c1.center;
  const float res2ParentAngle = std::atan2(res2ParentDiff.y, res2ParentDiff.x);
  EXPECT_NEAR(glm::degrees(res2ParentAngle), 61.92f, 0.01f);
}
