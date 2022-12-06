
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <array>
#include <cmath>

class IFrustumCulling {
public:
  virtual bool pointInFrustum(const glm::vec3& v) const = 0;
  virtual bool sphereInFrustum(const glm::vec3& v, float radius) const = 0;
  virtual bool cubeInFrustum(const glm::vec3& v, const glm::vec3& s) const = 0;
};

//
//
//

class FrustumCulling : public IFrustumCulling {
public:
  void calculateFrustum(const glm::mat4& proj, const glm::mat4& modl);

public:
  virtual bool pointInFrustum(const glm::vec3& v) const override;
  virtual bool sphereInFrustum(const glm::vec3& v, float radius) const override;
  virtual bool cubeInFrustum(const glm::vec3& v,
                             const glm::vec3& s) const override;

private:
  enum class FrustumSide : std::size_t {
    eRight = 0,
    eLeft,
    eBottom,
    eTop,
    eBack,
    eFront
  };
  enum class PlaneData : std::size_t { eA = 0, eB, eC, eD };

private:
  struct Plane {
    float a;
    float b;
    float c;
    float d;
  };
  std::array<Plane, 6> _frustum;

private:
  void _normalizePlane(FrustumSide side);
};
