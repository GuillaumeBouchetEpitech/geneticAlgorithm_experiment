
#pragma once

#include "thirdparty/GLMath.hpp"

#include "demo/utilities/types.hpp"

#include <array>

class FrustumCulling
{
public:
    void calculateFrustum(const glm::mat4& proj, const glm::mat4& modl);

public:
    bool pointInFrustum(const glm::vec3& v) const;
    bool sphereInFrustum(const glm::vec3& v, float radius) const;
    bool cubeInFrustum(const glm::vec3& v, const glm::vec3& s) const;

private:
    enum class FrustumSide: unsigned int
    {
        right = 0,
        left,
        bottom,
        top,
        back,
        front,

        count,
    };

    enum class PlaneData: unsigned int
    {
        A = 0,
        B,
        C,
        D,

        count,
    };

private:
    // float _frustum[toUnderlying(FrustumSide::count)][toUnderlying(PlaneData::count)];
    // std::array<std::array<float, toUnderlying(PlaneData::count)>, toUnderlying(FrustumSide::count)> _frustum;
    std::array<float[toUnderlying(PlaneData::count)], toUnderlying(FrustumSide::count)> _frustum;

private:
    void _normalizePlane(FrustumSide side);

};
