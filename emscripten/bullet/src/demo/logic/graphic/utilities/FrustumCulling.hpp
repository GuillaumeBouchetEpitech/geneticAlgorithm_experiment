
#pragma once

#include "thirdparty/GLMath.hpp"

#include <cmath>

class FrustumCulling
{
public:
    void calculateFrustum(const glm::mat4& proj, const glm::mat4& modl);

public:
    bool pointInFrustum(const glm::vec3& v) const;
    bool sphereInFrustum(const glm::vec3& v, float radius) const;
    bool cubeInFrustum(const glm::vec3& v, const glm::vec3& s) const;

private:
    enum e_FrustumSide { eRight, eLeft, eBottom, eTop, eBack, eFront };
    enum e_PlaneData { eA, eB, eC, eD };

private:
    float _frustum[6][4];

private:
    void normalizePlane(e_FrustumSide side);

};
