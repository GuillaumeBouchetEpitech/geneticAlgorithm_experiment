
#include "Car.hpp"

#include "./demo/utilities/types.hpp"

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
    , _physicVehicle(*_physicWorld.createVehicle())
{
    // _physicVehicle = _physicWorld.createVehicle();

    reset(position, quaternion);
}

void Car::update(const NeuralNetwork& neuralNetwork)
{
    if (!_isAlive)
        return;

    _updateSensors();
    _collideEyeSensors();
    _collideGroundSensor();

    if (_health <= 0)
    {
        _isAlive = false;
        _physicWorld.removeVehicle(_physicVehicle);
        return;
    }

    std::vector<float> input;
    input.reserve(_eyeSensors.size()); // pre-allocate
    for (const auto& eyeSensor : _eyeSensors)
    {
        // ensure input range is [0..1]
        input.push_back(glm::clamp(eyeSensor.value, 0.0f, 1.0f));
    }

    std::vector<float> output;

    neuralNetwork.process(input, output);

    // ensure output range is [0..1]
    output[0] = glm::clamp(output[0], 0.0f, 1.0f);
    output[1] = glm::clamp(output[1], 0.0f, 1.0f);

    // switch output range to [-1..1]
    output[0] = output[0] * 2.0f - 1.0f;
    output[1] = output[1] * 2.0f - 1.0f;

    _output.steer = output[0]; // steering angle: left/right
    _output.speed = output[1]; // speed coef: forward/backward

    _physicVehicle.setSteeringValue(_output.steer * k_steeringMaxValue);
    _physicVehicle.applyEngineForce(_output.speed * k_speedMaxValue);

    ++_totalUpdateNumber;
}

void Car::_updateSensors()
{
    glm::mat4 transform;
    _physicVehicle.getOpenGLMatrix(transform);

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


void Car::_collideEyeSensors()
{
    for (auto& sensor : _eyeSensors)
    {
        sensor.value = 1.0f;

        // eye sensors collide ground + walls
        PhysicWorld::t_raycastParams params(sensor.near, sensor.far);
        params.collisionGroup = asValue(PhysicWorld::Groups::sensor);
        params.collisionMask = asValue(PhysicWorld::Masks::eyeSensor);

        bool hasHit = _physicWorld.raycast(params);

        if (!hasHit)
            continue;

        sensor.far = params.result.impactPoint;

        sensor.value = glm::length(sensor.far - sensor.near) / k_eyeMaxRange;
    }
}

void Car::_collideGroundSensor()
{
    // raycast the ground to get the checkpoints validation

    // ground sensor collide only ground
    PhysicWorld::t_raycastParams params(_groundSensor.near, _groundSensor.far);
    params.collisionGroup = asValue(PhysicWorld::Groups::sensor);
    params.collisionMask = asValue(PhysicWorld::Masks::groundSensor);

    bool hasHitGround = _physicWorld.raycast(params);

    if (hasHitGround)
    {
        _groundSensor.far = params.result.impactPoint;

        _groundSensor.value = glm::length(_groundSensor.far - _groundSensor.near) / k_groundMaxRange;

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
        //     _physicVehicle.disableContactResponse();
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

    _physicVehicle.reset();
    _physicVehicle.setPosition({ position.x, position.y, position.z });
    _physicVehicle.setRotation({ quaternion.x, quaternion.y, quaternion.z, quaternion.w });

    _updateSensors();

    _physicWorld.addVehicle(_physicVehicle); // ensure vehicle presence
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
    return _physicVehicle;
}

float Car::getLife() const
{
    return float(_health) / k_healthMaxValue;
}

unsigned int Car::getTotalUpdates() const
{
    return _totalUpdateNumber;
}
