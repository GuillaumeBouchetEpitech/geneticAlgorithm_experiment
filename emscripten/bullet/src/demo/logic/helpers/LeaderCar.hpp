
#pragma once

#include "demo/logic/simulation/logic/CarData.hpp"

#include "framework/helpers/GLMath.hpp"

#include <optional>

class LeaderCar {
private:
  float _countdownUntilNewLeader = 0;
  int _carIndex = -1;
  float _totalTimeAsLeader = 0;
  glm::vec3 _carPosition = {0, 0, 0};

public:
  LeaderCar() = default;
  ~LeaderCar() = default;

public:
  void update(float elapsedTime);
  void reset();

public:
  bool hasLeader() const;
  int leaderIndex() const;
  std::optional<glm::vec3> leaderPosition() const;
  std::optional<CarData> leaderData() const;
  float totalTimeAsLeader() const;
};
