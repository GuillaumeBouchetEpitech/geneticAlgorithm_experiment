
#include "BSpline.hpp"

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

//

BSpline::BSpline(const std::vector<float>& points, unsigned int dimension, unsigned int degree)
    : _points(points)
    , _dimension(dimension)
    , _degree(degree)
{
    if (degree <= 2)
    {
        _baseFunc = &basisDeg2;
        _baseFuncRangeInterval = 2;
    }
    else if (degree == 3)
    {
        _baseFunc = &basisDeg3;
        _baseFuncRangeInterval = 2;
    }
    else if (degree == 4)
    {
        _baseFunc = &basisDeg4;
        _baseFuncRangeInterval = 3;
    }
    else if (degree > 4)
    {
        _baseFunc = &basisDeg5;
        _baseFuncRangeInterval = 3;
    } 
}

//

std::function<float(float)> BSpline::seqAt(unsigned int dim)
{
    return [this, dim](float n) -> float {

        float margin = _degree + 1;

        if (n < margin)
            return _points[dim];

        if (_points.size() / _dimension + margin <= n)
            return _points[(_points.size() / _dimension - 1) * _dimension + dim];

        return _points[(n - margin) * _dimension + dim];
    };
}

float   BSpline::getInterpol(std::function<float(float)> seq, float tt)
{
    unsigned int rangeInterval = _baseFuncRangeInterval;
    unsigned int tInterval = std::floor(tt);

    float result = 0.0f;
    for (unsigned int ii = tInterval - rangeInterval; ii <= tInterval + rangeInterval; ++ii)
        result += seq(ii) * _baseFunc(tt - ii);

    return result;
}

void    BSpline::calcAt(float tt, float* out_arr)
{
    // t must be in [0,1]
    tt = tt * ((_degree + 1) * 2 + _points.size() / _dimension);

    for (unsigned int ii = 0; ii < _dimension; ++ii)
        out_arr[ii] = getInterpol(seqAt(ii), tt);
}
