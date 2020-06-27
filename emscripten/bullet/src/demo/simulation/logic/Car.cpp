
#include "Car.hpp"

#include "./physic/PhysicTrimesh.hpp"

#include <cmath> // <= M_PI
#include <iostream>

namespace /*anonymous*/
{
    const float k_steeringMaxValue = M_PI / 8.0f;
    const float k_speedMaxValue = 10.0f;
    const int   k_healthMaxValue = 100;

    const float k_eyeMaxRange = 50.0f;
    const float k_eyeHeight = 1.0f;
    const float k_eyeElevation = 6.0f;
    const float k_eyeWidthStep = float(M_PI) / 8.0f;

    const std::array<float, 3> k_eyeElevations = {
        -k_eyeElevation,
        0.0f,
        +k_eyeElevation,
    };

    const std::array<float, 5> k_eyeAngles = {
        -k_eyeWidthStep * 2.0f,
        -k_eyeWidthStep,
        0.0f,
        +k_eyeWidthStep,
        +k_eyeWidthStep * 2.0f
    };

    const float k_groundMaxRange = 10.0f; // <= ground sensor
    const float k_groundHeight = 1.0f;

};

Car::Car(PhysicWorld& physicWorld,
         const glm::vec3& position,
         const glm::vec4& quaternion)
    : _physicWorld(physicWorld)
{
    _vehicle = _physicWorld.createVehicle();

    reset(position, quaternion);
}

void Car::update(const NeuralNetwork& neuralNetwork)
{
    if (!_isAlive)
        return;

    this->updateSensors();
    this->collideEyeSensors();
    this->collideGroundSensor();

    if (_health <= 0)
    {
        _isAlive = false;
        _physicWorld.removeVehicle(_vehicle);
        return;
    }

    std::vector<float> input;
    input.reserve(_eyeSensors.size());
    for (const auto& eyeSensor : _eyeSensors)
    {
        // input range is [0, 1]
        input.push_back(eyeSensor.value);
    }

    std::vector<float> output;

    neuralNetwork.process(input, output);

    // output range is [0, 1]
    _output.steer = output[0];
    _output.speed = output[1];

    _output.steer = std::min(1.0f, std::max(0.0f, _output.steer));
    _output.speed = std::min(1.0f, std::max(0.0f, _output.speed));

    _output.steer = _output.steer * 2.0f - 1.0f;
    _output.speed = _output.speed * 2.0f - 1.0f;

    _vehicle->setSteeringValue(_output.steer * k_steeringMaxValue);
    _vehicle->applyEngineForce(_output.speed * k_speedMaxValue);

    ++_totalUpdateNumber;
}

void Car::updateSensors()
{
    glm::mat4 transform;
    _vehicle->getOpenGLMatrix(transform);

    { // eye sensor

        glm::vec4 newNearValue = transform * glm::vec4(0, 0, k_eyeHeight, 1);

        int sensorIndex = 0;

        for (const auto& k_eyeElevation : k_eyeElevations)
            for (const auto& eyeAngle : k_eyeAngles)
            {
                auto& eyeSensor = _eyeSensors[sensorIndex++];

                glm::vec4 newFarValue = {
                    k_eyeMaxRange * std::sin(eyeAngle),
                    k_eyeMaxRange * std::cos(eyeAngle),
                    k_eyeHeight + k_eyeElevation,
                    1.0f
                };

                eyeSensor.near = newNearValue;
                eyeSensor.far = transform * newFarValue;
            }
    }

    { // ground sensor

        _groundSensor.near = transform * glm::vec4(0, 0, k_groundHeight, 1);
        _groundSensor.far = transform * glm::vec4(0, 0, k_groundHeight - k_groundMaxRange, 1);
    }
}


void Car::collideEyeSensors()
{
    for (auto& sensor : _eyeSensors)
    {
        sensor.value = 1.0f;

        // eye sensors collide ground + walls
        PhysicWorld::t_raycastParams params(sensor.near, sensor.far);
        params.collisionGroup = D_GROUP_SENSOR;
        params.collisionMask = D_GROUP_GROUND | D_GROUP_WALL;

        bool hasHit = _physicWorld.raycast(params);

        if (!hasHit)
            continue;

        sensor.far = params.result.impactPoint;

        sensor.value = glm::length(sensor.far - sensor.near) / k_eyeMaxRange;
    }
}

void Car::collideGroundSensor()
{
    // raycast the ground to get the checkpoints validation

    // ground sensor collide only ground
    PhysicWorld::t_raycastParams params(_groundSensor.near, _groundSensor.far);
    params.collisionGroup = D_GROUP_SENSOR;
    params.collisionMask = D_GROUP_GROUND;

    bool hasHitGround = _physicWorld.raycast(params);

    if (hasHitGround)
    {
        _groundSensor.far = params.result.impactPoint;
        int hasHitGroundIndex = params.result.impactIndex;

        // is this the next "ground geometry" index?
        if (_groundIndex + 1 == hasHitGroundIndex)
        {
            // update so it only get rewarded at the next "ground geometry"
            _groundIndex = hasHitGroundIndex;

            // restore health to full
            _health = k_healthMaxValue;

            // reward the genome
            ++_fitness;
        }
        // else if (_groundIndex + 1 > hasHitGroundIndex)
        // {
        //     _vehicle->disableContactResponse();
        // }
    }

    // reduce the health over time
    // => reduce more if the car does not touch the ground
    // => faster discard of a most probably dying genome
    _health -= hasHitGround ? 1 : 2;
}

void Car::reset(const glm::vec3& position, const glm::vec4& quaternion)
{
    _isAlive = true;
    _fitness = 0;
    _totalUpdateNumber = 0;
    _health = k_healthMaxValue;
    _groundIndex = 0;

    _output.steer = 0.0f;
    _output.speed = 0.0f;

    _vehicle->reset();
    _vehicle->setPosition({ position.x, position.y, position.z });
    _vehicle->setRotation({ quaternion.x, quaternion.y, quaternion.z, quaternion.w });

    updateSensors();

    // _physicWorld.removeVehicle(*_vehicle);
    _physicWorld.addVehicle(_vehicle); // ensure vehicle presence
}

const Car::t_sensors& Car::getEyeSensors() const
{
    return _eyeSensors;
}

const Car::t_sensor& Car::getGroundSensor() const
{
    return _groundSensor;
}

float Car::getFitness() const
{
    return _fitness;
}

bool Car::isAlive() const
{
    return _isAlive;
}

int  Car::getGroundIndex() const
{
    return _groundIndex;
}

const Car::t_neuralNetworkOutput& Car::getNeuralNetworkOutput() const
{
    return _output;
}

const PhysicVehicle& Car::getVehicle() const
{
    return *_vehicle;
}

float Car::getLife() const
{
    return float(_health) / k_healthMaxValue;
}

unsigned int Car::getTotalUpdates() const
{
    return _totalUpdateNumber;
}
