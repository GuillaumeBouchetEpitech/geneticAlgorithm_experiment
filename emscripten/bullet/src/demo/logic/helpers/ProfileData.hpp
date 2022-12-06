
#pragma once

#include "demo/logic/simulation/AbstactSimulation.hpp"

#include <vector>

class ProfileData {
private:
  using StatesData = std::vector<AbstactSimulation::CoreState>;
  using StatesHistory = std::vector<StatesData>;

  unsigned int _maxStateHistory = 60;
  unsigned int _totalCores = 0;
  unsigned int _currHistoryIndex = 0;
  StatesData _latestStatesData;
  StatesHistory _statesHistory;

public:
  ProfileData() = default;

public:
  void initialise(unsigned int totalCores, unsigned int maxStateHistory);

public:
  void clearLatest();
  void addToLatest(const AbstactSimulation::CoreState& latest);
  void pushLatest();

public:
  unsigned int getMaxStateHistory() const;
  unsigned int getTotalCores() const;

public:
  const AbstactSimulation::CoreState& getCoreData(unsigned int index) const;
  const AbstactSimulation::CoreState&
  getCoreHistoryData(unsigned int coreIndex, unsigned int dataIndex) const;

  unsigned int getLatestTotalDelta() const;
  unsigned int getAllTimeMaxDelta() const;
};
