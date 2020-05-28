
#include "BSpline.hpp"

#include "../ErrorHandler.hpp"

#include <cmath>

namespace /*anonymous*/
{

float basisDeg2(float x)
{
    if (-0.5f <= x && x < 0.5f)
        return 0.75f - x * x;
    if (0.5f <= x && x <= 1.5f)
        return 1.125f + (-1.5f + x / 2.0f) * x;
    if (-1.5f <= x && x < -0.5f)
        return 1.125f + (1.5f + x / 2.0f) * x;

    return 0;
};

float basisDeg3(float x)
{
    if (-1 <= x && x < 0)
        return 2.0f / 3.0f + (-1.0f - x / 2.0f) * x * x;
    if (1 <= x && x <= 2)
        return 4.0f / 3.0f + x * (-2.0f + (1.0f - x / 6.0f) * x);
    if (-2 <= x && x < -1)
        return 4.0f / 3.0f + x * (2.0f + (1.0f + x / 6.0f) * x);
    if (0 <= x && x < 1)
        return 2.0f / 3.0f + (-1.0f + x / 2.0f) * x * x;

    return 0;
};

float basisDeg4(float x)
{
    if (-1.5f <= x && x < -0.5f)
        return 55.0f / 96.0f + x * (-(5.0f / 24.0f) + x * (-(5.0f / 4.0f) + (-(5.0f / 6.0f) - x / 6.0f) * x));
    if (0.5f <= x && x < 1.5f)
        return 55.0f / 96.0f + x * (5.0f / 24.0f + x * (-(5.0f / 4.0f) + (5.0f / 6.0f - x / 6.0f) * x));
    if (1.5f <= x && x <= 2.5f)
        return 625.0f / 384.0f + x * (-(125.0f / 48.0f) + x * (25.0f / 16.0f + (-(5.0f / 12.0f) + x / 24.0f) * x));
    if (-2.5f <= x && x <= -1.5f)
        return 625.0f / 384.0f + x * (125.0f / 48.0f + x * (25.0f / 16.0f + (5.0f / 12.0f + x / 24.0f) * x));
    if (-1.5f <= x && x < 1.5f)
        return 115.0f / 192.0f + x * x * (-(5.0f / 8.0f) + x * x / 4.0f);

    return 0;
}

float basisDeg5(float x)
{
    if (-2 <= x && x < -1)
        return 17.0f / 40.0f + x * (-(5.0f / 8.0f) + x * (-(7.0f / 4.0f) + x * (-(5.0f / 4.0f) + (-(3.0f / 8.0f) - x / 24.0f) * x)));
    if (0 <= x && x < 1)
        return 11.0f / 20.0f + x * x * (-(1.0f / 2.0f) + (1.0f / 4.0f - x / 12.0f) * x * x);
    if (2 <= x && x <= 3)
        return 81.0f / 40.0f + x * (-(27.0f / 8.0f) + x * (9.0f / 4.0f + x * (-(3.0f / 4.0f) + (1.0f / 8.0f - x / 120.0f) * x)));
    if (-3 <= x && x < -2)
        return 81.0f / 40.0f + x * (27.0f / 8.0f + x * (9.0f / 4.0f + x * (3.0f / 4.0f + (1.0f / 8.0f + x / 120.0f) * x)));
    if (1 <= x && x < 2)
        return 17.0f / 40.0f + x * (5.0f / 8.0f + x * (-(7.0f / 4.0f) + x * (5.0f / 4.0f + (-(3.0f / 8.0f) + x / 24.0f) * x)));
    if (-1 <= x && x < 0)
        return 11.0f / 20.0f + x * x * (-(1.0f / 2.0f) + (1.0f / 4.0f + x / 12.0f) * x * x);

    return 0;
};

};

void BSpline::initialise(const BSpline::t_def& def)
{
    _def = def;

    if (_def.knotsData == nullptr)
        D_THROW(std::invalid_argument, "BSpline need knots, nullptr");
    if (_def.knotsLength == 0)
        D_THROW(std::invalid_argument, "BSpline need knots, emtpy length");
    if (_def.dimension == 0)
        D_THROW(std::invalid_argument, "BSpline need >0 dimension");

    if (_def.degree <= 2)
    {
        _baseFunc = &basisDeg2;
        _rangeInterval = 2;
    }
    else if (_def.degree == 3)
    {
        _baseFunc = &basisDeg3;
        _rangeInterval = 2;
    }
    else if (_def.degree == 4)
    {
        _baseFunc = &basisDeg4;
        _rangeInterval = 3;
    }
    else if (_def.degree > 4)
    {
        _baseFunc = &basisDeg5;
        _rangeInterval = 3;
    }
}

float BSpline::calcAt(float coef, unsigned int dimension) const
{
    if (_baseFunc == nullptr)
        D_THROW(std::invalid_argument, "not initialised");
    if (dimension >= _def.dimension)
        D_THROW(std::invalid_argument, "dimension out of bound");
    if (coef < 0.0f || coef > 1.0f)
        D_THROW(std::invalid_argument, "coef must be in [0,1], input=" << coef);

    float margin = _def.degree + 1;

    coef = coef * (margin * 2 + float(_def.knotsLength) / _def.dimension);
    unsigned int coefInterval = std::floor(coef);

    float result = 0.0f;
    for (unsigned int ii = coefInterval - _rangeInterval; ii <= coefInterval + _rangeInterval; ++ii)
    {
        float value = 0.0f;

        if (ii < margin)
            value = _def.knotsData[dimension];
        else if (ii >= _def.knotsLength / _def.dimension + margin)
            value = _def.knotsData[(_def.knotsLength / _def.dimension - 1) * _def.dimension + dimension];
        else
            value = _def.knotsData[int(ii - margin) * _def.dimension + dimension];

        result += value * _baseFunc(coef - ii);
    }

    return result;
}
