
#include "easingFunctions.hpp"

#include "framework/system/math/constants.hpp"

namespace easing {

float easeInSine(float t) { return std::sin(1.5707963f * t); }

float easeOutSine(float t) { return 1.0f + std::sin(1.5707963f * (t - 1.0f)); }

float easeInOutSine(float t) {
  return 0.5f * (1.0f + std::sin(3.1415926f * (t - 0.5f)));
}

float easeInQuad(float t) { return t * t; }

float easeOutQuad(float t) { return t * (2.0f - t); }

float easeInOutQuad(float t) {
  return t < 0.5f ? 2.0f * t * t : t * (4 - 2.0f * t) - 1.0f;
}

float easeInCubic(float t) { return t * t * t; }

float easeOutCubic(float t) {
  float t2 = t - 1;
  return 1.0f + t2 * t2 * t2;
}

float easeInOutCubic(float t) {
  float t2 = t - 1.0f;
  float t3 = t2 - 1.0f;
  return t < 0.5f ? 4.0f * t * t * t : 1.0f + t2 * (2.0f * t3) * (2.0f * t3);
}

float easeInQuart(float t) {
  t *= t;
  return t * t;
}

float easeOutQuart(float t) {
  float t2 = t - 1.0f;
  t = t2 * t2;
  return 1.0f - t * t;
}

float easeInOutQuart(float t) {
  if (t < 0.5f) {
    t *= t;
    return 8.0f * t * t;
  }
  float t2 = t - 1;
  t = t2 * t2;
  return 1.0f - 8.0f * t * t;
}

float easeInQuint(float t) {
  float t2 = t * t;
  return t * t2 * t2;
}

float easeOutQuint(float t) {
  float tx = t - 1;
  float t2 = tx * tx;
  return 1.0f + tx * t2 * t2;
}

float easeInOutQuint(float t) {
  float t2;
  if (t < 0.5f) {
    t2 = t * t;
    return 16.0f * t * t2 * t2;
  }

  float tx = t - 1.0f;
  t2 = tx * tx;
  return 1.0f + 16.0f * tx * t2 * t2;
}

float easeInExpo(float t) { return (std::pow(2.0f, 8.0f * t) - 1) / 255.0f; }

float easeOutExpo(float t) { return 1.0f - std::pow(2.0f, -8.0f * t); }

float easeInOutExpo(float t) {
  if (t < 0.5f)
    return (std::pow(2.0f, 16.0f * t) - 1) / 510.0f;
  return 1.0f - 0.5f * std::pow(2.0f, -16.0f * (t - 0.5f));
}

float easeInCirc(float t) { return 1.0f - std::sqrt(1.0f - t); }

float easeOutCirc(float t) { return std::sqrt(t); }

float easeInOutCirc(float t) {
  if (t < 0.5f)
    return (1.0f - std::sqrt(1.0f - 2.0f * t)) * 0.5f;
  return (1.0f + std::sqrt(2.0f * t - 1.0f)) * 0.5f;
}

float easeInBack(float t) { return t * t * (2.70158f * t - 1.70158f); }

float easeOutBack(float t) {
  float tx = t - 1;
  return 1.0f + tx * tx * (2.70158f * tx + 1.70158f);
}

float easeInOutBack(float t) {
  if (t < 0.5f)
    return t * t * (7.0f * t - 2.5f) * 2.0f;
  float tx = t - 1.0f;
  return 1.0f + tx * tx * 2.0f * (7.0f * tx + 2.5f);
}

float easeInElastic(float t) {
  float t2 = t * t;
  return t2 * t2 * std::sin(t * math::pi * 4.5f);
}

float easeOutElastic(float t) {
  float t2 = (t - 1.0f) * (t - 1.0f);
  return 1.0f - t2 * t2 * std::cos(t * math::pi * 4.5f);
}

float easeInOutElastic(float t) {
  float t2;
  if (t < 0.45f) {
    t2 = t * t;
    return 8.0f * t2 * t2 * std::sin(t * math::pi * 9.0f);
  }
  if (t < 0.55f)
    return 0.5f + 0.75f * std::sin(t * math::pi * 4.0f);

  t2 = (t - 1.0f) * (t - 1.0f);
  return 1.0f - 8.0f * t2 * t2 * std::sin(t * math::pi * 9.0f);
}

float easeInBounce(float t) {
  return std::pow(2.0f, 6.0f * (t - 1.0f)) *
         std::abs(std::sin(t * math::pi * 3.5f));
}

float easeOutBounce(float t) {
  return 1.0f -
         std::pow(2.0f, -6.0f * t) * std::abs(std::cos(t * math::pi * 3.5f));
}

float easeInOutBounce(float t) {
  if (t < 0.5f)
    return 8.0f * std::pow(2.0f, 8.0f * (t - 1.0f)) *
           std::abs(std::sin(t * math::pi * 7.0f));
  return 1.0f - 8.0f * std::pow(2.0f, -8.0f * t) *
                  std::abs(std::sin(t * math::pi * 7.0f));
}

} // namespace easing
