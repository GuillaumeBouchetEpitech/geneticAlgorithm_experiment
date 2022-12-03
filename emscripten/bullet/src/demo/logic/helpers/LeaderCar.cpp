
#include "LeaderCar.hpp"

#include "demo/logic/Data.hpp"


void LeaderCar::update(float elapsedTime)
{
  auto& simulation = *Data::get().logic.simulation;

  if (_timeoutUntilNewLeader > 0)
    _timeoutUntilNewLeader -= elapsedTime;

  _totalTimeAsLeader += elapsedTime;

  if (
    // no leader yet
    _index == -1 ||
    // the current leader is dead
    simulation.getCarResult(_index).isAlive == false ||
    // time to check for a potentially better leader
    _timeoutUntilNewLeader <= 0.0f)
  {
    // reset the timeout
    _timeoutUntilNewLeader = 1.0f; // <= one second

    // refresh the currently best car

    unsigned int totalCars = simulation.getTotalCars();

    float bestFitness = 0.0f;
    int oldLeaderCarIndex = _index;
    _index = -1;
    for (unsigned int ii = 0; ii < totalCars; ++ii)
    {
      const auto& carData = simulation.getCarResult(ii);

      if (!carData.isAlive)
        continue;

      if (
        _index != -1 &&
        !(carData.fitness > bestFitness + 2.0f)
      )
      {
        continue;
      }

      bestFitness = carData.groundIndex;
      _index = ii;
    }

    if (_index >= 0 && _index != oldLeaderCarIndex)
      _totalTimeAsLeader = 0.0f; // new leader
  }

  // do we have a car to focus the camera on?
  if (_index >= 0)
  {
    const auto& carResult = simulation.getCarResult(_index);

    // this part elevate where the camera look along the up axis of the car
    // => without it the camera look at the ground
    // => mostly useful for a shoulder camera0
    _carPosition = carResult.liveTransforms.chassis * glm::vec4(0.0f, 0.0f, 2.0f, 1.0f);
  }
}

void LeaderCar::reset()
{
  _index = -1;
  _timeoutUntilNewLeader = 0.0f;
  _totalTimeAsLeader = 0.0f;
}

bool LeaderCar::hasLeader() const
{
  return _index >= 0;
}

int LeaderCar::leaderIndex() const
{
  return _index;
}

std::optional<CarData> LeaderCar::leaderData() const
{
  if (_index < 0)
    return {};

  return Data::get().logic.simulation->getCarResult(_index);
}

std::optional<glm::vec3> LeaderCar::leaderPosition() const
{
  if (_index < 0)
    return {};

  return _carPosition;
}

float LeaderCar::totalTimeAsLeader() const
{
  return _totalTimeAsLeader;
}
