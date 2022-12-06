
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <array>
#include <unordered_map>
#include <vector>

class AbstactSimulation;

class CarWheelsTrails {
private:
  std::unordered_map<unsigned int, unsigned int> _genomeIndexMap;

  struct WheelsTrail {
    std::array<std::vector<glm::vec3>, 4> wheels;
  };
  std::vector<WheelsTrail> _allWheelsTrails;

  // unsigned int _currentTrailIndex = 0;

public:
  void reset(const AbstactSimulation& simulation);
  void push(unsigned int carIndex, int wheelIndex, const glm::vec3& value);

public:
  const WheelsTrail& getTrailByIndex(int index) const;
  const WheelsTrail& getTrailById(int id) const;
};
