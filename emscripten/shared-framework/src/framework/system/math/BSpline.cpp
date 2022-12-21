
#include "BSpline.hpp"

#include "framework/system/ErrorHandler.hpp"

#include <cmath>
#include <cstdint>

namespace BaseFuncs {

float basisDeg2(float x) {
  if (-0.5f <= x && x < 0.5f)
    return 0.75f - x * x;
  if (0.5f <= x && x <= 1.5f)
    return 1.125f + (-1.5f + x / 2.0f) * x;
  if (-1.5f <= x && x < -0.5f)
    return 1.125f + (1.5f + x / 2.0f) * x;

  return 0;
}

float basisDeg3(float x) {
  if (-1 <= x && x < 0)
    return 2.0f / 3.0f + (-1.0f - x / 2.0f) * x * x;
  if (1 <= x && x <= 2)
    return 4.0f / 3.0f + x * (-2.0f + (1.0f - x / 6.0f) * x);
  if (-2 <= x && x < -1)
    return 4.0f / 3.0f + x * (2.0f + (1.0f + x / 6.0f) * x);
  if (0 <= x && x < 1)
    return 2.0f / 3.0f + (-1.0f + x / 2.0f) * x * x;

  return 0;
}

float basisDeg4(float x) {
  if (-1.5f <= x && x < -0.5f)
    return 55.0f / 96.0f +
           x * (-(5.0f / 24.0f) +
                x * (-(5.0f / 4.0f) + (-(5.0f / 6.0f) - x / 6.0f) * x));
  if (0.5f <= x && x < 1.5f)
    return 55.0f / 96.0f +
           x * (5.0f / 24.0f +
                x * (-(5.0f / 4.0f) + (5.0f / 6.0f - x / 6.0f) * x));
  if (1.5f <= x && x <= 2.5f)
    return 625.0f / 384.0f +
           x * (-(125.0f / 48.0f) +
                x * (25.0f / 16.0f + (-(5.0f / 12.0f) + x / 24.0f) * x));
  if (-2.5f <= x && x <= -1.5f)
    return 625.0f / 384.0f +
           x * (125.0f / 48.0f +
                x * (25.0f / 16.0f + (5.0f / 12.0f + x / 24.0f) * x));
  if (-1.5f <= x && x < 1.5f)
    return 115.0f / 192.0f + x * x * (-(5.0f / 8.0f) + x * x / 4.0f);

  return 0;
}

float basisDeg5(float x) {
  if (-2 <= x && x < -1)
    return 17.0f / 40.0f +
           x * (-(5.0f / 8.0f) +
                x * (-(7.0f / 4.0f) +
                     x * (-(5.0f / 4.0f) + (-(3.0f / 8.0f) - x / 24.0f) * x)));
  if (0 <= x && x < 1)
    return 11.0f / 20.0f +
           x * x * (-(1.0f / 2.0f) + (1.0f / 4.0f - x / 12.0f) * x * x);
  if (2 <= x && x <= 3)
    return 81.0f / 40.0f +
           x * (-(27.0f / 8.0f) +
                x * (9.0f / 4.0f +
                     x * (-(3.0f / 4.0f) + (1.0f / 8.0f - x / 120.0f) * x)));
  if (-3 <= x && x < -2)
    return 81.0f / 40.0f +
           x * (27.0f / 8.0f +
                x * (9.0f / 4.0f +
                     x * (3.0f / 4.0f + (1.0f / 8.0f + x / 120.0f) * x)));
  if (1 <= x && x < 2)
    return 17.0f / 40.0f +
           x * (5.0f / 8.0f +
                x * (-(7.0f / 4.0f) +
                     x * (5.0f / 4.0f + (-(3.0f / 8.0f) + x / 24.0f) * x)));
  if (-1 <= x && x < 0)
    return 11.0f / 20.0f +
           x * x * (-(1.0f / 2.0f) + (1.0f / 4.0f + x / 12.0f) * x * x);

  return 0;
}

} // namespace BaseFuncs

void BSpline::initialise(const Definition& def) {
  _def = def;

  if (_def.knotsData == nullptr)
    D_THROW(std::invalid_argument, "BSpline need knots, nullptr");
  if (_def.knotsLength == 0)
    D_THROW(std::invalid_argument, "BSpline need knots, emtpy length");
  if (_def.dimensions == 0)
    D_THROW(std::invalid_argument, "BSpline need >0 dimensions");

  if (_def.degree <= 2) {
    _baseFunc = &BaseFuncs::basisDeg2;
    _rangeInterval = 2;
  } else if (_def.degree == 3) {
    _baseFunc = &BaseFuncs::basisDeg3;
    _rangeInterval = 2;
  } else if (_def.degree == 4) {
    _baseFunc = &BaseFuncs::basisDeg4;
    _rangeInterval = 3;
  } else if (_def.degree > 4) {
    _baseFunc = &BaseFuncs::basisDeg5;
    _rangeInterval = 3;
  }
}

float BSpline::calcAt(float coef, uint32_t currDimension) const {
  if (_baseFunc == nullptr)
    D_THROW(std::invalid_argument, "not initialised");
  if (currDimension >= _def.dimensions)
    D_THROW(std::invalid_argument, "dimension out of bound");
  if (coef < 0.0f || coef > 1.0f)
    D_THROW(std::invalid_argument, "coef must be in [0..1], input=" << coef);

  const float margin = float(_def.degree) + 1.0f;

  coef =
    coef * (margin * 2.0f + float(_def.knotsLength) / float(_def.dimensions));
  const int32_t coefInterval = int32_t(std::floor(coef));

  const int32_t iknotsLength = int32_t(_def.knotsLength);
  const int32_t idimensions = int32_t(_def.dimensions);
  const int32_t icurrDimension = int32_t(currDimension);

  float result = 0.0f;
  for (int32_t ii = coefInterval - _rangeInterval;
       ii <= coefInterval + _rangeInterval; ++ii) {
    float value = 0.0f;

    if (float(ii) < margin)
      value = _def.knotsData[currDimension];
    else if (float(ii) >= float(iknotsLength / idimensions) + margin)
      value =
        _def
          .knotsData[int32_t(float(iknotsLength) / float(idimensions) - 1.0f) *
                       idimensions +
                     icurrDimension];
    else
      value = _def.knotsData[int32_t(float(ii) - margin) * idimensions +
                             icurrDimension];

    result += value * _baseFunc(coef - float(ii));
  }

  return result;
}
