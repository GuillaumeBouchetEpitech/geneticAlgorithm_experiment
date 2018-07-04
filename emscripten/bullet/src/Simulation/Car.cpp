


#include "Car.hpp"


#include "Car.hpp"

// #include "Circuit.hpp"

// #include "../utils/utils.hpp"
// #include "../utils/collision.hpp"


#include <cmath> // sqrtf
#include <iostream>



#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm:value_ptr



IPhysicWrapper*	Car::m_pPhysicWrapper = nullptr;

void	Car::initialise(IPhysicWrapper* pPhysicWrapper)
{
	m_pPhysicWrapper = pPhysicWrapper;
}



Car::Car(unsigned int index)
	:	m_index(index)
{
	m_trail.reserve(2048*4);

	reset();
}

void	Car::update(const NeuralNetwork& in_NN)
{
	if (!m_alive)
		return;

	this->updateSensors();
	this->collideSensors();
	this->collideCheckpoints();

	//
	//

	// TODO: that might be slow, to profile and make as a static attribut?
	std::vector<float>	input;
	input.reserve(m_sensors.size());
	for (t_sensor& sensor : m_sensors)
		input.push_back( sensor.value );

	std::vector<float>	output;
	output.reserve(2);

	// if (input.size() != 5)
	//     throw std::invalid_argument( "about to use an invalid number of inputs" );

	in_NN.process(input, output);

	// if (output.size() != 2)
	//     throw std::invalid_argument( "received invalid number of outputs" );

	float leftTheta		= output[0];
	float rightTheta	= output[1];

	if (std::isnan(leftTheta))	leftTheta = 0.0f;
	if (std::isnan(rightTheta))	rightTheta = 0.0f;





	float steer_range = M_PI/8.0f;
	float steer = leftTheta * steer_range;
	leftTheta = std::min(steer_range, std::max(-steer_range, leftTheta));

	float speed_range = 250.0f;
	float speed = rightTheta * speed_range;
	speed = std::min(speed_range, std::max(-speed_range, speed));



	m_pPhysicWrapper->vehicle_applyEngineForce(m_index, speed);
	m_pPhysicWrapper->vehicle_setSteeringValue(m_index, steer);



	//
	// trail

	float	mat4x4[16];

	m_pPhysicWrapper->vehicle_getOpenGLMatrix(m_index, mat4x4);

	// m_trail.push_back(mat4x4[12]);
	// m_trail.push_back(mat4x4[13]);
	// m_trail.push_back(mat4x4[14]);

	glm::mat4	tmp_mat = glm::make_mat4((float*)mat4x4);
	tmp_mat = glm::translate(tmp_mat, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::vec4	tmp_pos = tmp_mat * glm::vec4(0, 0, 0, 1);

	m_trail.push_back(tmp_pos.x);
	m_trail.push_back(tmp_pos.y);
	m_trail.push_back(tmp_pos.z);

	// trail
	//

	++m_total_updates;
}

void	Car::updateSensors()
{
	float	mat4x4[16];
	m_pPhysicWrapper->vehicle_getOpenGLMatrix(m_index, mat4x4);

	glm::mat4	tmp_mat4x4 = glm::make_mat4(mat4x4);

	glm::vec4	tmp_from(0,0,1,1);
	tmp_from = tmp_mat4x4 * tmp_from;

	for (t_sensor& sensor : m_sensors)
	{
		sensor.near[0] = tmp_from.x;
		sensor.near[1] = tmp_from.y;
		sensor.near[2] = tmp_from.z;
	}

	// float arr_elevations[] = { -3, 0, +3 };
	float arr_elevations[] = { -6, 0, +6 };

	float arr_angles[] = {
		// -M_PI/8.0f,
		// -M_PI/16.0f,
		// 0.0f,
		// +M_PI/16.0f,
		// +M_PI/8.0f
		-M_PI/4.0f,
		-M_PI/8.0f,
		0.0f,
		+M_PI/8.0f,
		+M_PI/4.0f
	};

	for (unsigned int i = 0; i < 3; ++i)
	{
		float elevation = arr_elevations[i];

		for (unsigned int j = 0; j < 5; ++j)
		{
			float angle = arr_angles[j];

			glm::vec4	tmp_to(50.0f * sinf(angle), 50.0f * cosf(angle), 1.0f + elevation, 1.0f);
			tmp_to = tmp_mat4x4 * tmp_to;

			auto& sensor = m_sensors[ i * 5 + j ];

			sensor.far[0] = tmp_to.x;
			sensor.far[1] = tmp_to.y;
			sensor.far[2] = tmp_to.z;
		}
	}
}


void	Car::collideSensors()
{
	float	pCoord_hit[3];
	for (auto& sensor : m_sensors)
	{
		sensor.value = 1.0f;

		//

		bool success = m_pPhysicWrapper->raycast(m_index, sensor.near, sensor.far, pCoord_hit, nullptr);

		if (success)
		{
            sensor.far[0] = pCoord_hit[0];
            sensor.far[1] = pCoord_hit[1];
            sensor.far[2] = pCoord_hit[2];

			float	diff[3];
			diff[0] = sensor.far[0] - sensor.near[0];
			diff[1] = sensor.far[1] - sensor.near[1];
			diff[2] = sensor.far[2] - sensor.near[2];

            float	length = sqrtf(diff[0]*diff[0] + diff[1]*diff[1] + diff[2]*diff[2]);

            sensor.value = length / 50.0f;
		}
	}
}

void	Car::collideCheckpoints()
{
    int tmp_checkpoint_id = -1;

    { // raycast to get the checkpoints validation

		float	mat4x4[16];
		m_pPhysicWrapper->vehicle_getOpenGLMatrix(m_index, mat4x4);

		glm::mat4	tmp_mat = glm::make_mat4(mat4x4);

		glm::vec4	tmp_from(0,0,1,1);
		tmp_from = tmp_mat * tmp_from;

		glm::vec4	tmp_to(0,0,-9,1);
		tmp_to = tmp_mat * tmp_to;

		m_sensor_ground.near[0] = tmp_from.x;
		m_sensor_ground.near[1] = tmp_from.y;
		m_sensor_ground.near[2] = tmp_from.z;

		m_sensor_ground.far[0] = tmp_to.x;
		m_sensor_ground.far[1] = tmp_to.y;
		m_sensor_ground.far[2] = tmp_to.z;

		float	pCoord_hit[3];

		bool success = m_pPhysicWrapper->raycast(m_index, m_sensor_ground.near, m_sensor_ground.far, pCoord_hit, &tmp_checkpoint_id);

		if (success)
		{
            m_sensor_ground.far[0] = pCoord_hit[0];
            m_sensor_ground.far[1] = pCoord_hit[1];
            m_sensor_ground.far[2] = pCoord_hit[2];
		}
    }

    {
        if (m_current_checkpoint < tmp_checkpoint_id)
        {
            m_min_updates = 200;
            m_current_checkpoint = tmp_checkpoint_id;
            m_fitness++;
        }
        else
        {
            --m_min_updates;
            if (tmp_checkpoint_id == -1)
                --m_min_updates;

            if (m_min_updates <= 0)
                m_alive = false;
        }
    }
}

void	Car::reset() const
{
	Car*	self = const_cast<Car*>(this);

	self->m_alive = true;
	self->m_fitness = 0;
	self->m_total_updates = 0;
	self->m_trail.clear();
	self->m_min_updates = 200;
	self->m_current_checkpoint = 0;

	float	pos[] = {0,0,1.1f};
	float	rot[] = {0,0,-1,1};
	self->m_pPhysicWrapper->vehicle_reset(m_index, pos, rot);

	self->updateSensors();
}
