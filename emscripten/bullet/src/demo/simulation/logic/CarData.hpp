
#pragma once

#include "thirdparty/GLMath.hpp"

#include <array>
#include <vector>

struct CarData
{
    bool            isAlive;
    float           life;
    float           fitness;
    unsigned int    totalUpdates;
    int             groundIndex;

    glm::mat4                   transform;
    std::array<glm::mat4, 4>    wheelsTransform;
    glm::vec3                   velocity;


    struct t_sensor
    {
        glm::vec3   near;
        glm::vec3   far;
        float       value;
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
