
#include "FrustumCulling.hpp"

#include "framework/system/asValue.hpp"

void FrustumCulling::_normalizePlane(FrustumSide side) {
  const float magnitude =
    std::sqrt(_frustum[asValue(side)].a * _frustum[asValue(side)].a +
              _frustum[asValue(side)].b * _frustum[asValue(side)].b +
              _frustum[asValue(side)].c * _frustum[asValue(side)].c);

  _frustum[asValue(side)].a /= magnitude;
  _frustum[asValue(side)].b /= magnitude;
  _frustum[asValue(side)].c /= magnitude;
  _frustum[asValue(side)].d /= magnitude;
}

void FrustumCulling::calculateFrustum(const glm::mat4& inProj,
                                      const glm::mat4& inModl) {
  std::array<float, 16> clip;

  const float* proj = glm::value_ptr(inProj);
  const float* modl = glm::value_ptr(inModl);

  clip[0] = modl[0] * proj[0] + modl[1] * proj[4] + modl[2] * proj[8] +
            modl[3] * proj[12];
  clip[1] = modl[0] * proj[1] + modl[1] * proj[5] + modl[2] * proj[9] +
            modl[3] * proj[13];
  clip[2] = modl[0] * proj[2] + modl[1] * proj[6] + modl[2] * proj[10] +
            modl[3] * proj[14];
  clip[3] = modl[0] * proj[3] + modl[1] * proj[7] + modl[2] * proj[11] +
            modl[3] * proj[15];

  clip[4] = modl[4] * proj[0] + modl[5] * proj[4] + modl[6] * proj[8] +
            modl[7] * proj[12];
  clip[5] = modl[4] * proj[1] + modl[5] * proj[5] + modl[6] * proj[9] +
            modl[7] * proj[13];
  clip[6] = modl[4] * proj[2] + modl[5] * proj[6] + modl[6] * proj[10] +
            modl[7] * proj[14];
  clip[7] = modl[4] * proj[3] + modl[5] * proj[7] + modl[6] * proj[11] +
            modl[7] * proj[15];

  clip[8] = modl[8] * proj[0] + modl[9] * proj[4] + modl[10] * proj[8] +
            modl[11] * proj[12];
  clip[9] = modl[8] * proj[1] + modl[9] * proj[5] + modl[10] * proj[9] +
            modl[11] * proj[13];
  clip[10] = modl[8] * proj[2] + modl[9] * proj[6] + modl[10] * proj[10] +
             modl[11] * proj[14];
  clip[11] = modl[8] * proj[3] + modl[9] * proj[7] + modl[10] * proj[11] +
             modl[11] * proj[15];

  clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8] +
             modl[15] * proj[12];
  clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9] +
             modl[15] * proj[13];
  clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] +
             modl[15] * proj[14];
  clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] +
             modl[15] * proj[15];

  //

  _frustum[asValue(FrustumSide::eRight)] = {
    clip[3] - clip[0],
    clip[7] - clip[4],
    clip[11] - clip[8],
    clip[15] - clip[12],
  };
  _normalizePlane(FrustumSide::eRight);

  _frustum[asValue(FrustumSide::eLeft)] = {
    clip[3] + clip[0],
    clip[7] + clip[4],
    clip[11] + clip[8],
    clip[15] + clip[12],
  };
  _normalizePlane(FrustumSide::eLeft);

  //

  _frustum[asValue(FrustumSide::eBottom)] = {
    clip[3] + clip[1],
    clip[7] + clip[5],
    clip[11] + clip[9],
    clip[15] + clip[13],
  };
  _normalizePlane(FrustumSide::eBottom);

  _frustum[asValue(FrustumSide::eTop)] = {
    clip[3] - clip[1],
    clip[7] - clip[5],
    clip[11] - clip[9],
    clip[15] - clip[13],
  };
  _normalizePlane(FrustumSide::eTop);

  //

  _frustum[asValue(FrustumSide::eBack)] = {
    clip[3] - clip[2],
    clip[7] - clip[6],
    clip[11] - clip[10],
    clip[15] - clip[14],
  };
  _normalizePlane(FrustumSide::eBack);

  _frustum[asValue(FrustumSide::eFront)] = {
    clip[3] + clip[2],
    clip[7] + clip[6],
    clip[11] + clip[10],
    clip[15] + clip[14],
  };
  _normalizePlane(FrustumSide::eFront);
}

bool FrustumCulling::pointInFrustum(const glm::vec3& v) const {
  return sphereInFrustum(v, 0.0f);
}

bool FrustumCulling::sphereInFrustum(const glm::vec3& center,
                                     float radius) const {
  for (const auto& currSide : _frustum)
    if (currSide.a * center.x + currSide.b * center.y + currSide.c * center.z +
          currSide.d <=
        -radius)
      return false;

  return true;
}

bool FrustumCulling::cubeInFrustum(const glm::vec3& center,
                                   const glm::vec3& cubeSize) const {
  const glm::vec3 cubeHSize = cubeSize * 0.5f;

  for (const auto& currSide : _frustum) {
    if (currSide.a * (center.x - cubeHSize.x) +
          currSide.b * (center.y - cubeHSize.y) +
          currSide.c * (center.z - cubeHSize.z) + currSide.d >
        0)
      continue;

    if (currSide.a * (center.x + cubeHSize.x) +
          currSide.b * (center.y - cubeHSize.y) +
          currSide.c * (center.z - cubeHSize.z) + currSide.d >
        0)
      continue;

    if (currSide.a * (center.x - cubeHSize.x) +
          currSide.b * (center.y + cubeHSize.y) +
          currSide.c * (center.z - cubeHSize.z) + currSide.d >
        0)
      continue;

    if (currSide.a * (center.x + cubeHSize.x) +
          currSide.b * (center.y + cubeHSize.y) +
          currSide.c * (center.z - cubeHSize.z) + currSide.d >
        0)
      continue;

    if (currSide.a * (center.x - cubeHSize.x) +
          currSide.b * (center.y - cubeHSize.y) +
          currSide.c * (center.z + cubeHSize.z) + currSide.d >
        0)
      continue;

    if (currSide.a * (center.x + cubeHSize.x) +
          currSide.b * (center.y - cubeHSize.y) +
          currSide.c * (center.z + cubeHSize.z) + currSide.d >
        0)
      continue;

    if (currSide.a * (center.x - cubeHSize.x) +
          currSide.b * (center.y + cubeHSize.y) +
          currSide.c * (center.z + cubeHSize.z) + currSide.d >
        0)
      continue;

    if (currSide.a * (center.x + cubeHSize.x) +
          currSide.b * (center.y + cubeHSize.y) +
          currSide.c * (center.z + cubeHSize.z) + currSide.d >
        0)
      continue;

    return false;
  }

  return true;
}
