

#pragma once


#include <vector>
#include <functional>


class BSpline
{
private:
	const std::vector<float>&	m_points;
	unsigned int				m_dimension;
	unsigned int				m_degree;

	typedef	float (*t_baseFunc)(float);
	t_baseFunc					m_baseFunc;

	unsigned int				m_baseFuncRangeInt;

public:
	BSpline(const std::vector<float>& points, unsigned int dimension, unsigned int degree);

private:
	std::function<float(float)>		seqAt(unsigned int dim);
	float							getInterpol(std::function<float(float)> seq, float t);

public:
	void	calcAt(float t, float* out_arr);

};
