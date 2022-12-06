
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <array>
#include <vector>

struct CarData {
  bool isAlive;
  float life;
  float fitness;
  unsigned int totalUpdates;
  int groundIndex;

  struct Transforms {
    glm::mat4 chassis;
    std::array<glm::mat4, 4> wheels;
  };
  Transforms liveTransforms;

  std::vector<Transforms> latestTransformsHistory;

  glm::vec3 velocity;

  struct SensorData {
    glm::vec3 near;
    glm::vec3 far;
    float value;
  };

  std::array<SensorData, 15> eyeSensors;
  SensorData groundSensor;

  struct NeuralNetworkOutput {
    float steer = 0.0f;
    float speed = 0.0f;
  } neuralNetworkOutput;
};

using CarDatas = std::vector<CarData>;
