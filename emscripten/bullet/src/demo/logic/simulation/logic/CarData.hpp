
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <array>
#include <vector>

struct CarData {
  bool isAlive = false;
  float life = 0.0f;
  float fitness = 0.0f;
  unsigned int totalUpdates = 0;
  int groundIndex = -1;

  struct Transforms {
    glm::mat4 chassis;
    std::array<glm::mat4, 4> wheels;
  };
  Transforms liveTransforms;

  std::vector<Transforms> latestTransformsHistory;

  glm::vec3 velocity = {0,0,0};

  struct SensorData {
    glm::vec3 near = {0,0,0};
    glm::vec3 far = {0,0,0};
    float value = 0.0f;
  };

  std::array<SensorData, 15> eyeSensors;
  SensorData groundSensor;

  struct NeuralNetworkOutput {
    float steer = 0.0f;
    float speed = 0.0f;
  } neuralNetworkOutput;
};

using CarDatas = std::vector<CarData>;
