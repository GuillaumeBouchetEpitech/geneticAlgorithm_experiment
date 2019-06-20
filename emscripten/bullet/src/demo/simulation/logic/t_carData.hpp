
#pragma once

#include "demo/constants.hpp"

#include "thirdparty/GLMath.hpp"

#include <array>
#include <vector>

struct t_carData
{
	bool		isAlive;
	float		fitness;
	int			groundIndex;

	glm::mat4	transform;
	std::array<glm::mat4, 4>	wheelsTransform;


	struct t_sensor
	{
		glm::vec3	near, far;
		float		value;
	};

	std::array<t_sensor, 15>	eyeSensors;
	t_sensor					groundSensor;

	struct t_neuralNetworkOutput
	{
		float steer	= 0.0f;
		float speed	= 0.0f;
	}
	neuralNetworkOutput;

};

typedef std::vector<t_carData>	t_carsData;