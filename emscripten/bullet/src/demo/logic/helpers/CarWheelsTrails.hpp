
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <array>
#include <unordered_map>
#include <vector>

class AbstactSimulation;

class CarWheelsTrails {
public:
  using WheelTrail = std::vector<glm::vec3>;

  struct WheelsTrail {
    std::array<WheelTrail, 4> wheels;
  };

private:
  std::unordered_map<unsigned int, unsigned int> _genomeIndexMap;

  std::vector<WheelsTrail> _allWheelsTrails;

  // unsigned int _currentTrailIndex = 0;

public:
  void reset(const AbstactSimulation& simulation);
  void push(unsigned int carIndex, int wheelIndex, const glm::vec3& value);

public:
  bool isEmpty() const;
  const WheelsTrail& getTrailByIndex(int index) const;
  const WheelsTrail& getTrailById(int id) const;
};
