


#include "Car.hpp"


#include "Logic/Physic/Trimesh.hpp"
#include "Logic/Physic/Vehicle.hpp"

#include "constants.hpp"

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



// IPhysicWrapper*	Car::m_pPhysicWrapper = nullptr;

// void	Car::initialise(IPhysicWrapper* pPhysicWrapper)
// {
// 	m_pPhysicWrapper = pPhysicWrapper;
// }

Physic::World*	Car::m_pPhysicWorld = nullptr;

void	Car::initialise(Physic::World* pPhysicWorld)
{
	m_pPhysicWorld = pPhysicWorld;
}



// Car::Car(unsigned int index)
// 	:	m_index(index)
Car::Car()
{
	// m_trail.reserve(2048*4);

	m_pVehicle = m_pPhysicWorld->createVehicle();

	reset();
}

void	Car::update(const NeuralNetwork& in_NN)
{
	if (!m_alive)
		return;

	this->updateSensors();
	this->collideSensors();
	this->collideCheckpoints();

	// check again as the collideCheckpoints() method can change that
	if (!m_alive)
		return;

	//
	//

	// TODO: that might be slow, to profile and make as a static attribut?
	std::vector<float>	input;
	input.reserve(m_eyeSensors.size());
	for (auto&& eyeSensor : m_eyeSensors)
		input.push_back(eyeSensor.value);

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





	float steer = leftTheta * D_CAR_STEER_MAX_VALUE;
	leftTheta = std::min(D_CAR_STEER_MAX_VALUE, std::max(-D_CAR_STEER_MAX_VALUE, leftTheta));

	float speed = rightTheta * D_CAR_SPEED_MAX_VALUE;
	speed = std::min(D_CAR_SPEED_MAX_VALUE, std::max(-D_CAR_SPEED_MAX_VALUE, speed));



	// m_pPhysicWrapper->vehicle_applyEngineForce(m_index, speed);
	// m_pPhysicWrapper->vehicle_setSteeringValue(m_index, steer);

	m_pVehicle->applyEngineForce(speed);
	m_pVehicle->setSteeringValue(steer);


	//
	// trail

	// float	mat4x4[16];

	// // m_pPhysicWrapper->vehicle_getOpenGLMatrix(m_index, mat4x4);

	// pVehicle->getOpenGLMatrix(mat4x4);

	// glm::mat4	tmp_mat = glm::make_mat4((float*)mat4x4);
	// tmp_mat = glm::translate(tmp_mat, glm::vec3(0.0f, 0.0f, 1.0f));
	// glm::vec4	tmp_pos = tmp_mat * glm::vec4(0, 0, 0, 1);

	// m_trail.push_back(tmp_pos.x);
	// m_trail.push_back(tmp_pos.y);
	// m_trail.push_back(tmp_pos.z);

	// trail
	//

	++m_totalUpdateNumber;
}

void	Car::updateSensors()
{
	float	mat4x4[16];

	// m_pPhysicWrapper->vehicle_getOpenGLMatrix(m_index, mat4x4);

	m_pVehicle->getOpenGLMatrix(mat4x4);


	glm::mat4	vehicleTransform = glm::make_mat4(mat4x4);

	glm::vec4	fromVector(0, 0, 1, 1);
	fromVector = vehicleTransform * fromVector;

	for (auto& eyeSensor : m_eyeSensors)
	{
		eyeSensor.near[0] = fromVector.x;
		eyeSensor.near[1] = fromVector.y;
		eyeSensor.near[2] = fromVector.z;
	}

	// float arr_elevations[] = { -3, 0, +3 };
	float elevations[] = { -6, 0, +6 };

	float angles[] = {
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

	for (unsigned int ii = 0; ii < 3; ++ii)
	{
		float elevation = elevations[ii];

		for (unsigned int jj = 0; jj < 5; ++jj)
		{
			float angle = angles[jj];

			glm::vec4	toVector(50.0f * sinf(angle), 50.0f * cosf(angle), 1.0f + elevation, 1.0f);
			toVector = vehicleTransform * toVector;

			auto& eyeSensor = m_eyeSensors[ii * 5 + jj];

			eyeSensor.far[0] = toVector.x;
			eyeSensor.far[1] = toVector.y;
			eyeSensor.far[2] = toVector.z;
		}
	}
}


void	Car::collideSensors()
{
	float	pHitVector[3];
	for (auto& eyeSensor : m_eyeSensors)
	{
		eyeSensor.value = 1.0f;

		//

		// bool hasHit = m_pPhysicWrapper->raycast(m_index, eyeSensor.near, eyeSensor.far, pHitVector, nullptr);

		bool hasHit = m_pPhysicWorld->raycast(eyeSensor.near, eyeSensor.far, nullptr, pHitVector);

		if (!hasHit)
			continue;

        eyeSensor.far[0] = pHitVector[0];
        eyeSensor.far[1] = pHitVector[1];
        eyeSensor.far[2] = pHitVector[2];

		float	diff[3];
		diff[0] = eyeSensor.far[0] - eyeSensor.near[0];
		diff[1] = eyeSensor.far[1] - eyeSensor.near[1];
		diff[2] = eyeSensor.far[2] - eyeSensor.near[2];

        float	length = sqrtf(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);

        eyeSensor.value = length / 50.0f;
	}
}

void	Car::collideCheckpoints()
{
    // raycast to get the checkpoints validation

	float	mat4x4[16];

	// m_pPhysicWrapper->vehicle_getOpenGLMatrix(m_index, mat4x4);

	m_pVehicle->getOpenGLMatrix(mat4x4);

	glm::mat4	tmp_mat = glm::make_mat4(mat4x4);

	glm::vec4	fromVector(0, 0, 1, 1);
	glm::vec4	toVector(0, 0, -9, 1);

	fromVector = tmp_mat * fromVector;
	toVector = tmp_mat * toVector;

	m_groundSensor.near[0] = fromVector.x;
	m_groundSensor.near[1] = fromVector.y;
	m_groundSensor.near[2] = fromVector.z;

	m_groundSensor.far[0] = toVector.x;
	m_groundSensor.far[1] = toVector.y;
	m_groundSensor.far[2] = toVector.z;

	float	pHitVector[3];

	// bool hasHitGround = m_pPhysicWrapper->raycast(m_index, m_groundSensor.near, m_groundSensor.far, pHitVector, &tmp_checkpoint_id);

	Physic::Trimesh*	pTrimesh = nullptr;
	bool hasHitGround = m_pPhysicWorld->raycast(m_groundSensor.near, m_groundSensor.far, &pTrimesh, pHitVector);

    int	hasHitGroundCheckpointId = -1;

	if (hasHitGround)
	{
		m_groundSensor.far[0] = pHitVector[0];
		m_groundSensor.far[1] = pHitVector[1];
		m_groundSensor.far[2] = pHitVector[2];

		if (pTrimesh)
			hasHitGroundCheckpointId = pTrimesh->getID();
	}

	// reduce the health over time
    --m_currentHealth;

	if (!hasHitGround)
	{
    	// reduce the health again
		// => the car does not touch the ground
		// => faster discard of a most probably weak genome 
        --m_currentHealth;
	}
	else if (m_latestCheckpointId < hasHitGroundCheckpointId)
	{
		m_latestCheckpointId = hasHitGroundCheckpointId;

		// restore health to full
		m_currentHealth = D_CAR_HEALTH_MAX_VALUE;

		// reward the genome
		++m_fitness;
	}

    if (m_currentHealth <= 0)
        m_alive = false;
}

void	Car::reset() const
{
	Car*	pSelf = const_cast<Car*>(this);

	pSelf->m_alive = true;
	pSelf->m_fitness = 0;
	pSelf->m_totalUpdateNumber = 0;
	// pSelf->m_trail.clear();
	pSelf->m_currentHealth = D_CAR_HEALTH_MAX_VALUE;
	pSelf->m_latestCheckpointId = 0;

	float	pos[] = { 0, 0, 1.1f };
	float	rot[] = { 0, 0, -1, 1 };

	// pSelf->m_pPhysicWrapper->vehicle_reset(m_index, pos, rot);

	m_pVehicle->fullBrake();
	m_pVehicle->setPosition(pos);
	m_pVehicle->setRotation(rot);

	pSelf->updateSensors();
}
