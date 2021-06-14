
#pragma once

#include "./physic/PhysicWorld.hpp"
#include "./physic/PhysicVehicle.hpp"

#include "demo/logic/simulation/machineLearning/NeuralNetwork.hpp"

#include "demo/helpers/GLMath.hpp"

#include <array>
#include <vector>

class PhysicWorld;
class PhysicVehicle;

class Car
{
public: // external structures
    struct Sensor
    {
        glm::vec3 near, far;
        float value = 0.0f;
    };
    using Sensors = std::array<Sensor, 15>;

    struct NeuralNetworkOutput
    {
        float steer = 0.0f;
        float speed = 0.0f;
    };

private: // attributs
    PhysicWorld& _physicWorld;
    PhysicVehicle& _physicVehicle;

    float _fitness;
    bool _isAlive;
    float _health;
    unsigned int _totalUpdateNumber;

    // TODO : use enumeration
    Sensors _eyeSensors;
    Sensor _groundSensor;

    int _groundIndex;

    NeuralNetworkOutput _output;

public: // ctor/dtor
    Car(PhysicWorld& physicWorld,
        const glm::vec3& position,
        const glm::vec4& quaternion);

public: // methods
    void update(float elapsedTime, const std::shared_ptr<NeuralNetwork> nn);
    void reset(const glm::vec3& position, const glm::vec4& quaternion);

private: // methods
    void _updateSensors();
    void _collideEyeSensors();
    bool _collideGroundSensor();

public: // setter/getter
    const Sensors& getEyeSensors() const;
    const Sensor& getGroundSensor() const;
    float getFitness() const;
    bool isAlive() const;
    int getGroundIndex() const;
    const NeuralNetworkOutput& getNeuralNetworkOutput() const;
    const PhysicVehicle& getVehicle() const;
    float getLife() const;
    unsigned int getTotalUpdates() const;
};

using Cars = std::vector<Car>;
