
#include "FrustumCulling.hpp"

#include <cmath>

void FrustumCulling::_normalizePlane(FrustumSide side)
{
    auto& sideArray = _frustum[toUnderlying(side)];

    float magnitude = std::sqrt(
        sideArray[toUnderlying(PlaneData::A)] * sideArray[toUnderlying(PlaneData::A)] +
        sideArray[toUnderlying(PlaneData::B)] * sideArray[toUnderlying(PlaneData::B)] +
        sideArray[toUnderlying(PlaneData::C)] * sideArray[toUnderlying(PlaneData::C)]
    );

    sideArray[toUnderlying(PlaneData::A)] /= magnitude;
    sideArray[toUnderlying(PlaneData::B)] /= magnitude;
    sideArray[toUnderlying(PlaneData::C)] /= magnitude;
    sideArray[toUnderlying(PlaneData::D)] /= magnitude;
}

void FrustumCulling::calculateFrustum(const glm::mat4& proj, const glm::mat4& modl)
{
    glm::mat4 clip;

#define D_SET_CLIP(X) \
    clip[X][0] = modl[X][0]*proj[0][0] + modl[X][1]*proj[1][0] + modl[X][2]*proj[2][0] + modl[X][3]*proj[3][0]; \
    clip[X][1] = modl[X][0]*proj[0][1] + modl[X][1]*proj[1][1] + modl[X][2]*proj[2][1] + modl[X][3]*proj[3][1]; \
    clip[X][2] = modl[X][0]*proj[0][2] + modl[X][1]*proj[1][2] + modl[X][2]*proj[2][2] + modl[X][3]*proj[3][2]; \
    clip[X][3] = modl[X][0]*proj[0][3] + modl[X][1]*proj[1][3] + modl[X][2]*proj[2][3] + modl[X][3]*proj[3][3];

    D_SET_CLIP(0);
    D_SET_CLIP(1);
    D_SET_CLIP(2);
    D_SET_CLIP(3);

#undef D_SET_CLIP

    _frustum[toUnderlying(FrustumSide::right)][toUnderlying(PlaneData::A)] = clip[0][3] - clip[0][0];
    _frustum[toUnderlying(FrustumSide::right)][toUnderlying(PlaneData::B)] = clip[1][3] - clip[1][0];
    _frustum[toUnderlying(FrustumSide::right)][toUnderlying(PlaneData::C)] = clip[2][3] - clip[2][0];
    _frustum[toUnderlying(FrustumSide::right)][toUnderlying(PlaneData::D)] = clip[3][3] - clip[3][0];
    _normalizePlane(FrustumSide::right);

    _frustum[toUnderlying(FrustumSide::left)][toUnderlying(PlaneData::A)] = clip[0][3] - clip[0][0];
    _frustum[toUnderlying(FrustumSide::left)][toUnderlying(PlaneData::B)] = clip[1][3] - clip[1][0];
    _frustum[toUnderlying(FrustumSide::left)][toUnderlying(PlaneData::C)] = clip[2][3] - clip[2][0];
    _frustum[toUnderlying(FrustumSide::left)][toUnderlying(PlaneData::D)] = clip[3][3] - clip[3][0];
    _normalizePlane(FrustumSide::left);


    _frustum[toUnderlying(FrustumSide::bottom)][toUnderlying(PlaneData::A)] = clip[0][3] - clip[0][1];
    _frustum[toUnderlying(FrustumSide::bottom)][toUnderlying(PlaneData::B)] = clip[1][3] - clip[1][1];
    _frustum[toUnderlying(FrustumSide::bottom)][toUnderlying(PlaneData::C)] = clip[2][3] - clip[2][1];
    _frustum[toUnderlying(FrustumSide::bottom)][toUnderlying(PlaneData::D)] = clip[3][3] - clip[3][1];
    _normalizePlane(FrustumSide::bottom);

    _frustum[toUnderlying(FrustumSide::top)][toUnderlying(PlaneData::A)] = clip[0][3] - clip[0][1];
    _frustum[toUnderlying(FrustumSide::top)][toUnderlying(PlaneData::B)] = clip[1][3] - clip[1][1];
    _frustum[toUnderlying(FrustumSide::top)][toUnderlying(PlaneData::C)] = clip[2][3] - clip[2][1];
    _frustum[toUnderlying(FrustumSide::top)][toUnderlying(PlaneData::D)] = clip[3][3] - clip[3][1];
    _normalizePlane(FrustumSide::top);


    _frustum[toUnderlying(FrustumSide::back)][toUnderlying(PlaneData::A)] = clip[0][3] - clip[0][2];
    _frustum[toUnderlying(FrustumSide::back)][toUnderlying(PlaneData::B)] = clip[1][3] - clip[1][2];
    _frustum[toUnderlying(FrustumSide::back)][toUnderlying(PlaneData::C)] = clip[2][3] - clip[2][2];
    _frustum[toUnderlying(FrustumSide::back)][toUnderlying(PlaneData::D)] = clip[3][3] - clip[3][2];
    _normalizePlane(FrustumSide::back);

    _frustum[toUnderlying(FrustumSide::front)][toUnderlying(PlaneData::A)] = clip[0][3] - clip[0][2];
    _frustum[toUnderlying(FrustumSide::front)][toUnderlying(PlaneData::B)] = clip[1][3] - clip[1][2];
    _frustum[toUnderlying(FrustumSide::front)][toUnderlying(PlaneData::C)] = clip[2][3] - clip[2][2];
    _frustum[toUnderlying(FrustumSide::front)][toUnderlying(PlaneData::D)] = clip[3][3] - clip[3][2];
    _normalizePlane(FrustumSide::front);
}

bool FrustumCulling::pointInFrustum(const glm::vec3& center) const
{
    for (const auto& sideArray : _frustum)
        if (sideArray[toUnderlying(PlaneData::A)] * center.x +
            sideArray[toUnderlying(PlaneData::B)] * center.y +
            sideArray[toUnderlying(PlaneData::C)] * center.z +
            sideArray[toUnderlying(PlaneData::D)] <= 0)
            return false;

    return true;
}

bool FrustumCulling::sphereInFrustum(const glm::vec3& center, float radius) const
{
    for (const auto& sideArray : _frustum)
        if (sideArray[toUnderlying(PlaneData::A)] * center.x +
            sideArray[toUnderlying(PlaneData::B)] * center.y +
            sideArray[toUnderlying(PlaneData::C)] * center.z +
            sideArray[toUnderlying(PlaneData::D)] <= -radius)
            return false;

    return true;
}

bool FrustumCulling::cubeInFrustum(const glm::vec3& center, const glm::vec3& size) const
{
    glm::vec3 halfSize = size * 0.5f;

    for (const auto& sideArray : _frustum)
    {
        if (sideArray[toUnderlying(PlaneData::A)] * (center.x - halfSize.x) +
            sideArray[toUnderlying(PlaneData::B)] * (center.y - halfSize.y) +
            sideArray[toUnderlying(PlaneData::C)] * (center.z - halfSize.z) +
            sideArray[toUnderlying(PlaneData::D)] > 0)
            continue;

        if (sideArray[toUnderlying(PlaneData::A)] * (center.x + halfSize.x) +
            sideArray[toUnderlying(PlaneData::B)] * (center.y - halfSize.y) +
            sideArray[toUnderlying(PlaneData::C)] * (center.z - halfSize.z) +
            sideArray[toUnderlying(PlaneData::D)] > 0)
            continue;

        if (sideArray[toUnderlying(PlaneData::A)] * (center.x - halfSize.x) +
            sideArray[toUnderlying(PlaneData::B)] * (center.y + halfSize.y) +
            sideArray[toUnderlying(PlaneData::C)] * (center.z - halfSize.z) +
            sideArray[toUnderlying(PlaneData::D)] > 0)
            continue;

        if (sideArray[toUnderlying(PlaneData::A)] * (center.x + halfSize.x) +
            sideArray[toUnderlying(PlaneData::B)] * (center.y + halfSize.y) +
            sideArray[toUnderlying(PlaneData::C)] * (center.z - halfSize.z) +
            sideArray[toUnderlying(PlaneData::D)] > 0)
            continue;

        if (sideArray[toUnderlying(PlaneData::A)] * (center.x - halfSize.x) +
            sideArray[toUnderlying(PlaneData::B)] * (center.y - halfSize.y) +
            sideArray[toUnderlying(PlaneData::C)] * (center.z + halfSize.z) +
            sideArray[toUnderlying(PlaneData::D)] > 0)
            continue;

        if (sideArray[toUnderlying(PlaneData::A)] * (center.x + halfSize.x) +
            sideArray[toUnderlying(PlaneData::B)] * (center.y - halfSize.y) +
            sideArray[toUnderlying(PlaneData::C)] * (center.z + halfSize.z) +
            sideArray[toUnderlying(PlaneData::D)] > 0)
            continue;

        if (sideArray[toUnderlying(PlaneData::A)] * (center.x - halfSize.x) +
            sideArray[toUnderlying(PlaneData::B)] * (center.y + halfSize.y) +
            sideArray[toUnderlying(PlaneData::C)] * (center.z + halfSize.z) +
            sideArray[toUnderlying(PlaneData::D)] > 0)
            continue;

        if (sideArray[toUnderlying(PlaneData::A)] * (center.x + halfSize.x) +
            sideArray[toUnderlying(PlaneData::B)] * (center.y + halfSize.y) +
            sideArray[toUnderlying(PlaneData::C)] * (center.z + halfSize.z) +
            sideArray[toUnderlying(PlaneData::D)] > 0)
            continue;

        return false;
    }

    return true;
}

