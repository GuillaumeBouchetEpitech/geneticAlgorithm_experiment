
#pragma once

#include "helpers/GLMath.hpp"

#include <cmath>

class IFrustumCulling
{
public:
  virtual bool pointInFrustum(const glm::vec3& v) const = 0;
  virtual bool sphereInFrustum(const glm::vec3& v, float radius) const = 0;
  virtual bool cubeInFrustum(const glm::vec3& v, const glm::vec3& s) const = 0;
};

//
//
//

class FrustumCulling
  : public IFrustumCulling
{
public:
  void calculateFrustum(const glm::mat4& proj, const glm::mat4& modl);

public:
  virtual bool pointInFrustum(const glm::vec3& v) const override;
  virtual bool sphereInFrustum(const glm::vec3& v, float radius) const override;
  virtual bool cubeInFrustum(const glm::vec3& v, const glm::vec3& s) const override;

private:
  enum e_FrustumSide { eRight, eLeft, eBottom, eTop, eBack, eFront };
  enum e_PlaneData { eA, eB, eC, eD };

private:
  float _frustum[6][4];

private:
  void _normalizePlane(e_FrustumSide side);

};
