
#pragma once

#include "thirdparty/GLMath.hpp"

#include "demo/utilities/types.hpp"

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
    enum class e_FrustumSide: unsigned int
    {
        eRight = 0,
        eLeft,
        eBottom,
        eTop,
        eBack,
        eFront,

        eCount,
    };

    enum class e_PlaneData: unsigned int
    {
        eA = 0,
        eB,
        eC,
        eD,

        eCount,
    };

private:
    float _frustum[toUnderlying(e_FrustumSide::eCount)][toUnderlying(e_PlaneData::eCount)];

private:
    void _normalizePlane(e_FrustumSide side);

};
