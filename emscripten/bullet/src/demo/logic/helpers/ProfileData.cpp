
#include "ProfileData.hpp"

void
ProfileData::initialise(unsigned int totalCores, unsigned int maxStateHistory) {
  _maxStateHistory = maxStateHistory;
  _totalCores = totalCores;

  _latestStatesData.resize(totalCores);
  _statesHistory.resize(totalCores);
  for (auto& stateHistory : _statesHistory)
    stateHistory.resize(maxStateHistory);
}

void
ProfileData::clearLatest() {
  _latestStatesData.clear();
}

void
ProfileData::addToLatest(const AbstactSimulation::CoreState& latest) {
  _latestStatesData.push_back(latest);
}

void
ProfileData::pushLatest() {
  for (unsigned int coreIndex = 0; coreIndex < _latestStatesData.size();
       ++coreIndex)
    _statesHistory.at(coreIndex).at(_currHistoryIndex) =
      _latestStatesData.at(coreIndex);

  _currHistoryIndex =
    (_currHistoryIndex + 1) % _maxStateHistory; // rotate index
}

unsigned int
ProfileData::getMaxStateHistory() const {
  return _maxStateHistory;
}

unsigned int
ProfileData::getTotalCores() const {
  return _totalCores;
}

const AbstactSimulation::CoreState&
ProfileData::getCoreData(unsigned int index) const {
  return _latestStatesData.at(index);
}

const AbstactSimulation::CoreState&
ProfileData::getCoreHistoryData(
  unsigned int coreIndex, unsigned int dataIndex) const {
  const unsigned int actualIndex =
    (dataIndex + _currHistoryIndex) % _maxStateHistory;

  return _statesHistory.at(coreIndex).at(actualIndex);
}

unsigned int
ProfileData::getLatestTotalDelta() const {
  unsigned int totalDelta = 0;
  for (std::size_t statIndex = 0; statIndex < _latestStatesData.size();
       ++statIndex)
    totalDelta += _latestStatesData.at(statIndex).delta;
  return totalDelta;
}

unsigned int
ProfileData::getAllTimeMaxDelta() const {
  unsigned int maxDelta = 0;
  for (unsigned int coreIndex = 0; coreIndex < _totalCores; ++coreIndex)
    for (unsigned int statIndex = 0; statIndex < _maxStateHistory; ++statIndex)
      maxDelta =
        std::max(maxDelta, _statesHistory.at(coreIndex).at(statIndex).delta);
  return maxDelta;
}
