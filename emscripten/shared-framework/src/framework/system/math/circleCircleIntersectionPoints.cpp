
#include "circleCircleIntersectionPoints.hpp"

#include "framework/system/math/constants.hpp"

namespace {
// Let EPS (epsilon) be a small value
const float EPS = 0.0000001f;

// Due to double rounding precision the value passed into the Math.acos
// function may be outside its domain of [-1, +1] which would return
// the value NaN which we do not want.
float acossafe(float x) {
  if (x >= +1.0)
    return 0.0f;
  if (x <= -1.0)
    return math::pi;
  return std::acos(x);
}

// Rotates a point about a fixed point at some angle 'a'
glm::vec2 rotatePoint(const glm::vec2& fp, const glm::vec2& pt, float a) {
  float x = pt.x - fp.x;
  float y = pt.y - fp.y;
  float xRot = x * std::cos(a) + y * std::sin(a);
  float yRot = y * std::cos(a) - x * std::sin(a);
  return glm::vec2(fp.x + xRot, fp.y + yRot);
}

} // namespace

// Given two circles this method finds the intersection
// point(s) of the two circles (if any exists)
bool circleCircleIntersectionPoints(const Circle& c1, const Circle& c2,
                                    glm::vec2& res1, glm::vec2& res2) {

  float r, R, d, dx, dy, cx, cy, Cx, Cy;

  if (c1.radius < c2.radius) {
    r = c1.radius;
    R = c2.radius;
    cx = c1.center.x;
    cy = c1.center.y;
    Cx = c2.center.x;
    Cy = c2.center.y;
  } else {
    r = c2.radius;
    R = c1.radius;
    Cx = c1.center.x;
    Cy = c1.center.y;
    cx = c2.center.x;
    cy = c2.center.y;
  }

  // Compute the vector <dx, dy>
  dx = cx - Cx;
  dy = cy - Cy;

  // Find the distance between two points.
  d = std::sqrt(dx * dx + dy * dy);

  // There are an infinite number of solutions
  // Seems appropriate to also return null
  if (d < EPS && std::abs(R - r) < EPS)
    return false;

  // No intersection (circles centered at the
  // same place with different size)
  if (d < EPS)
    return false;

  float x = (dx / d) * R + Cx;
  float y = (dy / d) * R + Cy;
  glm::vec2 P = {x, y};

  // Single intersection (kissing circles)
  if (std::abs((R + r) - d) < EPS || std::abs(R - (r + d)) < EPS) {
    res1 = P;
    res2 = P;
    return true;
  }

  // No intersection. Either the small circle contained within
  // big circle or circles are simply disjoint.
  if ((d + r) < R || (R + r < d))
    return false;

  glm::vec2 C = {Cx, Cy};
  float angle = acossafe((r * r - d * d - R * R) / (-2.0f * d * R));
  glm::vec2 pt1 = rotatePoint(C, P, +angle);
  glm::vec2 pt2 = rotatePoint(C, P, -angle);

  res1 = pt1;
  res2 = pt2;

  return true;
}
