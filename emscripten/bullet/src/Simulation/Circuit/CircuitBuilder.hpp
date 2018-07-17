
#pragma once


#include <string>
#include <vector>
#include <functional>


class CircuitBuilder
{
public:
	typedef std::function<void(const std::vector<float>& buffer, const std::vector<int>& indices)> t_callback;

private:
	std::vector<float> m_points_left, m_points_right;

public:
	bool load(const std::string& filename);

public:
	bool generateSkeleton(t_callback skeleton);
	bool generate(t_callback ground, t_callback wall);

};
