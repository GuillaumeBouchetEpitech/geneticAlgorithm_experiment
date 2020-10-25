
#include "Car.hpp"

#include "./demo/utilities/types.hpp"

#include "./physic/PhysicTrimesh.hpp"

#include <cmath> // <= M_PI
#include <iostream>

namespace constants
{
    const float steeringMaxValue = M_PI / 8.0f;
    const float speedMaxValue = 10.0f;
    const int   healthMaxValue = 100;

    const float eyeMaxRange = 50.0f;
    const float eyeHeight = 1.0f;
    const float eyeElevation = 6.0f;
    const float eyeWidthStep = float(M_PI) / 8.0f;

    const std::array<float, 3> eyeElevations = {
        -eyeElevation,
        0.0f,
        +eyeElevation,
    };

    const std::array<float, 5> eyeAngles = {
        -eyeWidthStep * 2.0f,
        -eyeWidthStep,
        0.0f,
        +eyeWidthStep,
        +eyeWidthStep * 2.0f
    };

    const float groundMaxRange = 10.0f; // <= ground sensor
    const float groundHeight = 1.0f;

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

    _physicVehicle.setSteeringValue(_output.steer * constants::steeringMaxValue);
    _physicVehicle.applyEngineForce(_output.speed * constants::speedMaxValue);

    ++_totalUpdateNumber;
}

void Car::_updateSensors()
{
    glm::mat4 transform;
    _physicVehicle.getOpenGLMatrix(transform);

    { // eye sensor

        glm::vec4 newNearValue = transform * glm::vec4(0, 0, constants::eyeHeight, 1);

        int sensorIndex = 0;

        for (const auto& eyeElevation : constants::eyeElevations)
            for (const auto& eyeAngle : constants::eyeAngles)
            {
                auto& eyeSensor = _eyeSensors[sensorIndex++];

                glm::vec4 newFarValue(
                    constants::eyeMaxRange * std::sin(eyeAngle),
                    constants::eyeMaxRange * std::cos(eyeAngle),
                    constants::eyeHeight + eyeElevation,
                    1.0f
                );

                eyeSensor.near = newNearValue;
                eyeSensor.far = transform * newFarValue;
            }
    }

    { // ground sensor

        _groundSensor.near = transform * glm::vec4(0, 0, constants::groundHeight, 1);
        _groundSensor.far = transform * glm::vec4(0, 0, constants::groundHeight - constants::groundMaxRange, 1);
    }
}


void Car::_collideEyeSensors()
{
    for (auto& sensor : _eyeSensors)
    {
        sensor.value = 1.0f;

        // eye sensors collide ground + walls
        PhysicWorld::RaycastParams params(sensor.near, sensor.far);
        params.collisionGroup = asValue(PhysicWorld::Groups::sensor);
        params.collisionMask = asValue(PhysicWorld::Masks::eyeSensor);

        bool hasHit = _physicWorld.raycast(params);

        if (!hasHit)
            continue;

        sensor.far = params.result.impactPoint;

        sensor.value = glm::length(sensor.far - sensor.near) / constants::eyeMaxRange;
    }
}

void Car::_collideGroundSensor()
{
    // raycast the ground to get the checkpoints validation

    // ground sensor collide only ground
    PhysicWorld::RaycastParams params(_groundSensor.near, _groundSensor.far);
    params.collisionGroup = asValue(PhysicWorld::Groups::sensor);
    params.collisionMask = asValue(PhysicWorld::Masks::groundSensor);

    bool hasHitGround = _physicWorld.raycast(params);

    if (hasHitGround)
    {
        _groundSensor.far = params.result.impactPoint;

        _groundSensor.value = glm::length(_groundSensor.far - _groundSensor.near) / constants::groundMaxRange;

        int hasHitGroundIndex = params.result.impactIndex;

        // is this the next "ground geometry" index?
        if (_groundIndex + 1 == hasHitGroundIndex)
        {
            // update so it only get rewarded at the next "ground geometry"
            _groundIndex = hasHitGroundIndex;

            // restore health to full
            _health = constants::healthMaxValue;

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
    _health = constants::healthMaxValue;
    _groundIndex = 0;

    _output.steer = 0.0f;
    _output.speed = 0.0f;

    _physicVehicle.reset();
    _physicVehicle.setPosition({ position.x, position.y, position.z });
    _physicVehicle.setRotation({ quaternion.x, quaternion.y, quaternion.z, quaternion.w });

    _updateSensors();

    _physicWorld.addVehicle(_physicVehicle); // ensure vehicle presence
}

const Car::Sensors& Car::getEyeSensors() const
{
    return _eyeSensors;
}

const Car::Sensor& Car::getGroundSensor() const
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

const Car::NeuralNetworkOutput& Car::getNeuralNetworkOutput() const
{
    return _output;
}

const PhysicVehicle& Car::getVehicle() const
{
    return _physicVehicle;
}

float Car::getLife() const
{
    return float(_health) / constants::healthMaxValue;
}

unsigned int Car::getTotalUpdates() const
{
    return _totalUpdateNumber;
}
