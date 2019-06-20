
#pragma once

#include <vector>
#include <functional>

class BSpline
{
private:

	typedef	float (*t_baseFunc)(float);

	const std::vector<float>&	_points;
	unsigned int				_dimension;
	unsigned int				_degree;
	t_baseFunc					_baseFunc = nullptr;
	unsigned int				_baseFuncRangeInterval;

public:
	BSpline(const std::vector<float>& points, unsigned int dimension, unsigned int degree);

private:
	std::function<float(float)>		seqAt(unsigned int dim);
	float							getInterpol(std::function<float(float)> seq, float t);

public:
	void	calcAt(float t, float* out_arr);

};
