
#pragma once

#include "thirdparty/GLMath.hpp"

#include <array>
#include <vector>

struct CarData
{
    bool isAlive;
    float life;
    float fitness;
    unsigned int totalUpdates;
    int groundIndex;

    struct Transforms
    {
        glm::mat4 chassis;
        std::array<glm::mat4, 4> wheels;
    };
    Transforms transforms;

    std::vector<Transforms> latestTransformsHistory;

    glm::vec3 velocity;

    struct t_sensor
    {
        glm::vec3 near;
        glm::vec3 far;
        float value;
    };

    std::array<t_sensor, 15> eyeSensors;
    t_sensor groundSensor;

    struct t_neuralNetworkOutput
    {
        float steer = 0.0f;
        float speed = 0.0f;
    }
    neuralNetworkOutput;

};

using CarDatas = std::vector<CarData>;
