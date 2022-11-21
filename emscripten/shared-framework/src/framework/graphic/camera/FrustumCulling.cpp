
#include "FrustumCulling.hpp"

void FrustumCulling::_normalizePlane(e_FrustumSide side)
{
	float magnitude = std::sqrt(
    _frustum[side][eA] * _frustum[side][eA] +
    _frustum[side][eB] * _frustum[side][eB] +
    _frustum[side][eC] * _frustum[side][eC]
  );

	_frustum[side][eA] /= magnitude;
	_frustum[side][eB] /= magnitude;
	_frustum[side][eC] /= magnitude;
	_frustum[side][eD] /= magnitude;
}

void FrustumCulling::calculateFrustum(const glm::mat4& proj2, const glm::mat4& modl2)
{
	float clip[16];

	const float* proj = glm::value_ptr(proj2);
	const float* modl = glm::value_ptr(modl2);

	clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
	clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
	clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
	clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

	clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
	clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
	clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
	clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

	clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
	clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
	clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

  //

	_frustum[eRight][eA] = clip[ 3] - clip[ 0];
	_frustum[eRight][eB] = clip[ 7] - clip[ 4];
	_frustum[eRight][eC] = clip[11] - clip[ 8];
	_frustum[eRight][eD] = clip[15] - clip[12];
	_normalizePlane(eRight);

	_frustum[eLeft][eA] = clip[ 3] + clip[ 0];
	_frustum[eLeft][eB] = clip[ 7] + clip[ 4];
	_frustum[eLeft][eC] = clip[11] + clip[ 8];
	_frustum[eLeft][eD] = clip[15] + clip[12];
	_normalizePlane(eLeft);

  //

	_frustum[eBottom][eA] = clip[ 3] + clip[ 1];
	_frustum[eBottom][eB] = clip[ 7] + clip[ 5];
	_frustum[eBottom][eC] = clip[11] + clip[ 9];
	_frustum[eBottom][eD] = clip[15] + clip[13];
	_normalizePlane(eBottom);

	_frustum[eTop][eA] = clip[ 3] - clip[ 1];
	_frustum[eTop][eB] = clip[ 7] - clip[ 5];
	_frustum[eTop][eC] = clip[11] - clip[ 9];
	_frustum[eTop][eD] = clip[15] - clip[13];
	_normalizePlane(eTop);

  //

	_frustum[eBack][eA] = clip[ 3] - clip[ 2];
	_frustum[eBack][eB] = clip[ 7] - clip[ 6];
	_frustum[eBack][eC] = clip[11] - clip[10];
	_frustum[eBack][eD] = clip[15] - clip[14];
	_normalizePlane(eBack);

	_frustum[eFront][eA] = clip[ 3] + clip[ 2];
	_frustum[eFront][eB] = clip[ 7] + clip[ 6];
	_frustum[eFront][eC] = clip[11] + clip[10];
	_frustum[eFront][eD] = clip[15] + clip[14];
	_normalizePlane(eFront);
}

bool FrustumCulling::pointInFrustum(const glm::vec3& v) const
{
	return sphereInFrustum(v, 0.0f);
}

bool FrustumCulling::sphereInFrustum(const glm::vec3& center, float radius) const
{
	for (int ii = 0; ii < 6; ++ii)
		if (_frustum[ii][eA] * center.x +
        _frustum[ii][eB] * center.y +
        _frustum[ii][eC] * center.z +
        _frustum[ii][eD] <= -radius)
			return false;

	return true;
}

bool FrustumCulling::cubeInFrustum(const glm::vec3& center, const glm::vec3& cubeSize) const
{
	const glm::vec3 cubeHSize = cubeSize * 0.5f;

	for (int ii = 0; ii < 6; ++ii)
	{
		if (_frustum[ii][eA] * (center.x - cubeHSize.x) +
        _frustum[ii][eB] * (center.y - cubeHSize.y) +
        _frustum[ii][eC] * (center.z - cubeHSize.z) +
        _frustum[ii][eD] > 0)
			continue;

		if (_frustum[ii][eA] * (center.x + cubeHSize.x) +
        _frustum[ii][eB] * (center.y - cubeHSize.y) +
        _frustum[ii][eC] * (center.z - cubeHSize.z) +
        _frustum[ii][eD] > 0)
			continue;

		if (_frustum[ii][eA] * (center.x - cubeHSize.x) +
        _frustum[ii][eB] * (center.y + cubeHSize.y) +
        _frustum[ii][eC] * (center.z - cubeHSize.z) +
        _frustum[ii][eD] > 0)
			continue;

		if (_frustum[ii][eA] * (center.x + cubeHSize.x) +
        _frustum[ii][eB] * (center.y + cubeHSize.y) +
        _frustum[ii][eC] * (center.z - cubeHSize.z) +
        _frustum[ii][eD] > 0)
			continue;

		if (_frustum[ii][eA] * (center.x - cubeHSize.x) +
        _frustum[ii][eB] * (center.y - cubeHSize.y) +
        _frustum[ii][eC] * (center.z + cubeHSize.z) +
        _frustum[ii][eD] > 0)
			continue;

		if (_frustum[ii][eA] * (center.x + cubeHSize.x) +
        _frustum[ii][eB] * (center.y - cubeHSize.y) +
        _frustum[ii][eC] * (center.z + cubeHSize.z) +
        _frustum[ii][eD] > 0)
			continue;

		if (_frustum[ii][eA] * (center.x - cubeHSize.x) +
        _frustum[ii][eB] * (center.y + cubeHSize.y) +
        _frustum[ii][eC] * (center.z + cubeHSize.z) +
        _frustum[ii][eD] > 0)
			continue;

		if (_frustum[ii][eA] * (center.x + cubeHSize.x) +
        _frustum[ii][eB] * (center.y + cubeHSize.y) +
        _frustum[ii][eC] * (center.z + cubeHSize.z) +
        _frustum[ii][eD] > 0)
			continue;

		return false;
	}

	return true;
}

