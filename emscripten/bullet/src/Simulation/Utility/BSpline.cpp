

#include "BSpline.hpp"

#include <cmath>

//


float basisDeg2(float x)
{
    if(-0.5 <= x && x < 0.5){
        return 0.75 - x*x;
    }else if(0.5 <= x && x <= 1.5){
        return 1.125 + (-1.5 + x/2.0)*x;
    }else if(-1.5 <= x && x < -0.5){
        return 1.125 + (1.5 + x/2.0)*x;
    }else{
        return 0;
    }
};

float basisDeg3(float x)
{
    if(-1 <= x && x < 0){
        return 2.0/3.0 + (-1.0 - x/2.0)*x*x;
    }else if(1 <= x && x <= 2){
        return 4.0/3.0 + x*(-2.0 + (1.0 - x/6.0)*x);
    }else if(-2 <= x && x < -1){
        return 4.0/3.0 + x*(2.0 + (1.0 + x/6.0)*x);
    }else if(0 <= x && x < 1){
        return 2.0/3.0 + (-1.0 + x/2.0)*x*x;
    }else{
        return 0;
    }
};

float basisDeg4(float x)
{
    if(-1.5 <= x && x < -0.5){
        return 55.0/96.0 + x*(-(5.0/24.0) + x*(-(5.0/4.0) + (-(5.0/6.0) - x/6.0)*x));
    }else if(0.5 <= x && x < 1.5){
        return 55.0/96.0 + x*(5.0/24.0 + x*(-(5.0/4.0) + (5.0/6.0 - x/6.0)*x));
    }else if(1.5 <= x && x <= 2.5){
        return 625.0/384.0 + x*(-(125.0/48.0) + x*(25.0/16.0 + (-(5.0/12.0) + x/24.0)*x));
    }else if(-2.5 <= x && x <= -1.5){
        return 625.0/384.0 + x*(125.0/48.0 + x*(25.0/16.0 + (5.0/12.0 + x/24.0)*x));
    }else if(-1.5 <= x && x < 1.5){
        return 115.0/192.0 + x*x*(-(5.0/8.0) + x*x/4.0);
    }else{
        return 0;
    }
};

float basisDeg5(float x)
{
    if(-2 <= x && x < -1){
        return 17.0/40.0 + x*(-(5.0/8.0) + x*(-(7.0/4.0) + x*(-(5.0/4.0) + (-(3.0/8.0) - x/24.0)*x)));
    }else if(0 <= x && x < 1){
        return 11.0/20.0 + x*x*(-(1.0/2.0) + (1.0/4.0 - x/12.0)*x*x);
    }else if(2 <= x && x <= 3){
        return 81.0/40.0 + x*(-(27.0/8.0) + x*(9.0/4.0 + x*(-(3.0/4.0) + (1.0/8.0 - x/120.0)*x)));
    }else if(-3 <= x && x < -2){
        return 81.0/40.0 + x*(27.0/8.0 + x*(9.0/4.0 + x*(3.0/4.0 + (1.0/8.0 + x/120.0)*x)));
    }else if(1 <= x && x < 2){
        return 17.0/40.0 + x*(5.0/8.0 + x*(-(7.0/4.0) + x*(5.0/4.0 + (-(3.0/8.0) + x/24.0)*x)));
    }else if(-1 <= x && x < 0){
        return 11.0/20.0 + x*x*(-(1.0/2.0) + (1.0/4.0 + x/12.0)*x*x);
    }else{
        return 0;
    }
};


//


BSpline::BSpline(const std::vector<float>& points, unsigned int dimension, unsigned int degree)
	: m_points(points)
	, m_dimension(dimension)
	, m_degree(degree)
{
    if (degree <= 2)
    {
        m_baseFunc = &basisDeg2;
        m_baseFuncRangeInt = 2;
    }
    else if(degree == 3)
    {
        m_baseFunc = &basisDeg3;
        m_baseFuncRangeInt = 2;
    }
    else if(degree == 4)
    {
        m_baseFunc = &basisDeg4;
        m_baseFuncRangeInt = 3;
    }
    else if(degree > 4)
    {
        m_baseFunc = &basisDeg5;
        m_baseFuncRangeInt = 3;
    } 
}


//


std::function<float(float)>	BSpline::seqAt(unsigned int dim)
{
    return [this, dim](float n) -> float
    {
    	float margin = m_degree + 1;
        if (n < margin)
        {
            return m_points[dim];
        }
        else if (m_points.size() / m_dimension + margin <= n)
        {
            return m_points[(m_points.size() / m_dimension - 1) * m_dimension + dim];
        }
        else
        {
            return m_points[(n - margin) * m_dimension + dim];
        }
    };
}



float	BSpline::getInterpol(std::function<float(float)> seq, float t)
{
	unsigned int	rangeInt = m_baseFuncRangeInt;
	unsigned int	tInt = floor(t);

	float	result = 0.0f;
	for (unsigned int i = tInt - rangeInt; i <= tInt + rangeInt; ++i)
	{
		result += seq(i) * m_baseFunc(t - i);
	}
	return result;
};




void	BSpline::calcAt(float t, float* out_arr)
{
	t = t * ((m_degree + 1) * 2 + m_points.size() / m_dimension); // t must be in [0,1]

	for (unsigned int i = 0; i < m_dimension; ++i)
		out_arr[i] = getInterpol(seqAt(i), t);
};