
#include "LeaderCar.hpp"

#include "demo/logic/Context.hpp"


void LeaderCar::update(float elapsedTime)
{
  auto& simulation = *Context::get().logic.simulation;

  if (_countdownUntilNewLeader > 0.0f)
    _countdownUntilNewLeader -= elapsedTime;

  _totalTimeAsLeader += elapsedTime;

  if (
    // no leader yet
    _carIndex == -1 ||
    // the current leader is dead
    simulation.getCarResult(_carIndex).isAlive == false ||
    // the current leader is not on the ground
    simulation.getCarResult(_carIndex).groundSensor.value > 0.5f  ||
    // time to check for a potentially better leader
    _countdownUntilNewLeader <= 0.0f)
  {
    // reset the timeout
    _countdownUntilNewLeader = 1.0f; // <= one second

    // refresh the currently best car

    unsigned int totalCars = simulation.getTotalCars();

    float bestFitness = 0.0f;
    int oldLeaderCarIndex = _carIndex;
    _carIndex = -1;
    for (unsigned int carIndex = 0; carIndex < totalCars; ++carIndex)
    {
      const auto& carData = simulation.getCarResult(carIndex);

      if (!carData.isAlive)
        continue;

      if (
        _carIndex != -1 &&
        !(carData.fitness > bestFitness + 2.0f)
      )
      {
        continue;
      }

      bestFitness = carData.groundIndex;
      _carIndex = carIndex;
    }

    if (_carIndex >= 0 && _carIndex != oldLeaderCarIndex)
      _totalTimeAsLeader = 0.0f; // new leader
  }

  // do we have a car to focus the camera on?
  if (_carIndex >= 0)
  {
    const auto& carResult = simulation.getCarResult(_carIndex);

    // this part elevate where the camera look along the up axis of the car
    // => without it the camera look at the ground
    // => mostly useful for a shoulder camera0
    _carPosition = carResult.liveTransforms.chassis * glm::vec4(0.0f, 0.0f, 2.0f, 1.0f);
  }
}

void LeaderCar::reset()
{
  _carIndex = -1;
  _countdownUntilNewLeader = 0.0f;
  _totalTimeAsLeader = 0.0f;
}

bool LeaderCar::hasLeader() const
{
  return _carIndex >= 0;
}

int LeaderCar::leaderIndex() const
{
  return _carIndex;
}

std::optional<CarData> LeaderCar::leaderData() const
{
  if (_carIndex < 0)
    return {};

  return Context::get().logic.simulation->getCarResult(_carIndex);
}

std::optional<glm::vec3> LeaderCar::leaderPosition() const
{
  if (_carIndex < 0)
    return {};

  return _carPosition;
}

float LeaderCar::totalTimeAsLeader() const
{
  return _totalTimeAsLeader;
}
