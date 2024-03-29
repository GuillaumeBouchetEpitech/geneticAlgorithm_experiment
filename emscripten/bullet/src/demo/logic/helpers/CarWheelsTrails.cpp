
#include "CarWheelsTrails.hpp"

#include "demo/logic/simulation/AbstactSimulation.hpp"

#include "framework/system/ErrorHandler.hpp"

void
CarWheelsTrails::reset(const AbstactSimulation& simulation) {
  _genomeIndexMap.clear();

  const unsigned int totalCars = simulation.getTotalCars();

  if (_allWheelsTrails.size() < totalCars)
    _allWheelsTrails.resize(totalCars);

  for (auto& trail : _allWheelsTrails) {
    for (auto& wheel : trail.wheels) {
      wheel.reserve(2048); // pre-allocate
      wheel.clear();
    }
  }

  const auto& genomes = simulation.getGenomes();

  const std::size_t safeSize = std::min<std::size_t>(totalCars, genomes.size());
  for (std::size_t ii = 0; ii < safeSize; ++ii) {
    // record the trail index with it's genome id in the lookup map
    _genomeIndexMap[genomes.at(ii).id] = ii;
  }
}

void
CarWheelsTrails::push(
  unsigned int carIndex, int wheelIndex, const glm::vec3& value) {
  _allWheelsTrails.at(carIndex).wheels.at(wheelIndex).emplace_back(value);
}

bool
CarWheelsTrails::isEmpty() const {
  return _allWheelsTrails.empty();
}

const CarWheelsTrails::WheelsTrail&
CarWheelsTrails::getTrailByIndex(int index) const {
  return _allWheelsTrails.at(index);
}

const CarWheelsTrails::WheelsTrail&
CarWheelsTrails::getTrailById(int id) const {
  auto it = _genomeIndexMap.find(id);
  if (it == _genomeIndexMap.end())
    D_THROW(std::runtime_error, "trail not found, id: " << id);

  return _allWheelsTrails.at(it->second);
}
