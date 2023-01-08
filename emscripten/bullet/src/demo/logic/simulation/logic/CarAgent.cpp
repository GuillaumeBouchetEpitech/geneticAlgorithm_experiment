
#include "CarAgent.hpp"

#include "../common.hpp"

#include "framework/system/TraceLogger.hpp"
#include "framework/system/asValue.hpp"
#include "framework/system/math/GenericEasing.hpp"
#include "framework/system/math/constants.hpp"

#include <cmath>
#include <iostream>

namespace constants {

constexpr float steeringMaxValue = math::opi;
// constexpr float speedMaxValue = 10.0f;
constexpr float speedMaxValue = 20.0f;
constexpr float healthMaxValue = 3.2f;

constexpr float eyeMaxRange = 50.0f;
constexpr float eyeHeight = 1.0f;
constexpr float eyeElevation = 6.0f;
constexpr float eyeWidthStep = math::opi;

const std::array<float, 3> eyeElevations = {
  +eyeElevation,
  0.0f,
  -eyeElevation,
};

const std::array<float, 5> eyeAngles = {
  -eyeWidthStep * 2.0f, -eyeWidthStep, 0.0f, +eyeWidthStep,
  +eyeWidthStep * 2.0f};

constexpr float groundMaxRange = 10.0f; // <= ground sensor
constexpr float groundHeight = 1.0f;

} // namespace constants

void
CarAgent::update(
  float elapsedTime, const std::shared_ptr<NeuralNetwork> neuralNetwork) {
  if (!_isAlive)
    return;

  _updateSensors();
  _collideEyeSensors();

  bool hasHitGround = _collideGroundSensor();

  // reduce the health over time
  // => reduce more if the car does not touch the ground
  // => faster discard of a most probably dying genome
  _health -= (hasHitGround ? 1 : 2) * elapsedTime;

  if (_health <= 0) {
    _isAlive = false;
    // _physicWorld->removeVehicle(_physicVehicle);
    _physicWorld->getPhysicVehicleManager().removeVehicle(_physicVehicle);
    return;
  }

  std::vector<float> input;
  input.reserve(_eyeSensors.size()); // pre-allocate
  for (const auto& eyeSensor : _eyeSensors) {
    // ensure input range is [0..1]
    input.push_back(glm::clamp(eyeSensor.value, 0.0f, 1.0f));
  }

  const float speed = _physicVehicle->getCurrentSpeedKmHour();
  // D_MYLOG("speed=" << speed);
  input.push_back(glm::clamp(speed / 100.0f, 0.0f, 1.0f));

  std::vector<float> output;

  neuralNetwork->compute(input, output);

  // ensure output range is [0..1]
  output.at(0) = glm::clamp(output.at(0), 0.0f, 1.0f);
  output.at(1) = glm::clamp(output.at(1), 0.0f, 1.0f);

  // switch output range to [-1..1]
  output.at(0) = output.at(0) * 2.0f - 1.0f;
  // output.at(1) = output.at(1) * 2.0f - 1.0f;

  _output.steer = output.at(0); // steering angle: left/right
  _output.speed = output.at(1); // speed coef: forward/backward

  _physicVehicle->setSteeringValue(
    0, _output.steer * constants::steeringMaxValue);
  _physicVehicle->setSteeringValue(
    1, _output.steer * constants::steeringMaxValue);

  // const float engineFore = GenericEasing<4>()
  auto engineEasing =
    GenericEasing<5>()
      .push(0.0f, -constants::speedMaxValue)
      .push(0.5f, 0.0f)
      .push(0.5f + 0.5f * 0.25f, constants::speedMaxValue * 4.0f)
      .push(0.5f + 0.5f * 0.50f, constants::speedMaxValue)
      .push(0.5f + 0.5f * 1.00f, constants::speedMaxValue);

  const float engineForce = engineEasing.get(_output.speed);

  _physicVehicle->applyEngineForce(2, engineForce);
  _physicVehicle->applyEngineForce(3, engineForce);

  ++_totalUpdateNumber;
}

void
CarAgent::_createVehicle() {
  if (_physicVehicle)
    _physicWorld->getPhysicVehicleManager().destroyVehicle(_physicVehicle);
  if (_physicBody)
    _physicWorld->getPhysicBodyManager().destroyBody(_physicBody);

  PhysicBodyDef bodyDef;

  // const glm::vec3 chassisSize = { 1.0f, 2.0f, 0.5f };
  const glm::vec3 chassisSize = {2.0f, 4.0f, 1.0f};
  const glm::vec3 chassisHSize = chassisSize * 0.5f;

  bodyDef.shape.type = PhysicShapeDef::Type::compound;
  {
    PhysicShapeDef::Data::Compound::ChildShape childShape;
    {
      // elevate the chassis
      childShape.transform = glm::identity<glm::mat4>();
      childShape.transform =
        glm::translate(childShape.transform, glm::vec3(0.0f, 0.0f, 0.9f));
    }
    {
      childShape.shape = std::make_shared<PhysicShapeDef>();
      childShape.shape->type = PhysicShapeDef::Type::box;
      childShape.shape->data.box.size = chassisSize;
    }
    bodyDef.shape.data.compound.childShapes.push_back(childShape);
  }

  bodyDef.mass = 5.0f;
  bodyDef.group = asValue(Groups::vehicle);
  bodyDef.mask = asValue(Groups::ground);

  _physicBody = _physicWorld->getPhysicBodyManager().createAndAddBody(bodyDef);
  _physicBody->setPosition({30, 30, 5});
  _physicBody->setFriction(1.0f);
  _physicBody->disableSleep();

  //
  //

  const glm::vec3 wheelDirectionCS0 = {
    0.0f, 0.0f, -1.0f}; // down axis: -Z (toward the ground)
  const glm::vec3 wheelAxleCS = {1.0f, 0.0f, 0.0f}; // rotation axis: +X
  const float wheelRadius = 0.5f;
  const float wheelWidth = 0.2f;
  const float wheelSide = wheelWidth * 0.3f;

  // The maximum length of the suspension (metres)
  const float suspensionRestLength = 0.3f;

  // The maximum distance the suspension can be compressed (centimetres)
  const float maxSuspensionTravelCm = 20.0f; // <= 0.2 metres

  // The coefficient of friction between the tyre and the ground.
  // Should be about 0.8 for realistic cars, but can increased for better
  // handling. Set large (10000.0) for kart racers const float wheelFriction =
  // 100.0f; // <= "kart racer"
  const float wheelFriction = 10000.0f; // <= "kart racer"

  // The stiffness constant for the suspension.
  // => 10.0: "Offroad buggy"
  // => 50.0: "Sports car"
  // => 200.0: "F1 Car"
  const float suspensionStiffness = 100.0f; // <= "Sports/F1 Car"

  // The damping coefficient for when the suspension is compressed.
  // Set to k * 2.0 * btSqrt(m_suspensionStiffness) so k is proportional to
  // critical damping. k = 0.0 undamped & bouncy, k = 1.0 critical damping 0.1
  // to 0.3 are good values
  const float wheelsDampingCompression = 0.3f;

  // The damping coefficient for when the suspension is expanding.
  // See the comments for m_wheelsDampingCompression for how to set k.
  // m_wheelsDampingRelaxation should be slightly larger than
  // m_wheelsDampingCompression, eg 0.2 to 0.5
  const float wheelsDampingRelaxation = 0.5f;

  // Reduces the rolling torque applied from the wheels that cause the vehicle
  // to roll over. This is a bit of a hack, but it's quite effective. 0.0 = no
  // roll, 1.0 = physical behaviour. If m_frictionSlip is too high, you'll need
  // to reduce this to stop the vehicle rolling over. You should also try
  // lowering the vehicle's centre of mass
  const float rollInfluence = 0.5f;

  PhysicVehicleDef vehicleDef;
  vehicleDef.body = _physicBody;
  vehicleDef.coordinateSystem = {0, 2, 1};
  vehicleDef.wheelsCollisionGroup = asValue(Groups::vehicle);
  vehicleDef.wheelsCollisionMask = asValue(Groups::ground);
  vehicleDef.allWheelStats.reserve(4);

  struct WheelPosition {
    glm::vec3 connectionPoint;
    bool isFrontWheel;
  };

  std::array<WheelPosition, 4> wheelPositions{
    {// front right
     {{+chassisHSize.x - wheelSide, +chassisHSize.y - wheelRadius, 0.5f},
      /*isFrontWheel = */ true},
     // front left
     {{-chassisHSize.x + wheelSide, +chassisHSize.y - wheelRadius, 0.5f},
      /*isFrontWheel = */ true},
     // rear right
     {{+chassisHSize.x - wheelSide, -chassisHSize.y + wheelRadius, 0.5f},
      /*isFrontWheel = */ false},
     // rear left
     {{-chassisHSize.x + wheelSide, -chassisHSize.y + wheelRadius, 0.5f},
      /*isFrontWheel = */ false}}};

  for (auto& wheelPos : wheelPositions) {
    PhysicVehicleDef::WheelStats wheelStats;

    wheelStats.connectionPoint = wheelPos.connectionPoint;
    wheelStats.isFrontWheel = wheelPos.isFrontWheel;
    wheelStats.maxSuspensionTravelCm = maxSuspensionTravelCm;
    wheelStats.rollInfluence = rollInfluence;
    wheelStats.suspensionRestLength = suspensionRestLength;
    wheelStats.suspensionStiffness = suspensionStiffness;
    wheelStats.wheelAxleCS = wheelAxleCS;
    wheelStats.wheelDirectionCS0 = wheelDirectionCS0;
    wheelStats.wheelFriction = wheelFriction;
    wheelStats.wheelRadius = wheelRadius;
    wheelStats.wheelsDampingCompression = wheelsDampingCompression;
    wheelStats.wheelsDampingRelaxation = wheelsDampingRelaxation;

    vehicleDef.allWheelStats.push_back(wheelStats);
  }

  _physicVehicle =
    _physicWorld->getPhysicVehicleManager().createAndAddVehicle(vehicleDef);
}

void
CarAgent::_updateSensors() {
  glm::mat4 vehicleTransform;
  _physicBody->getTransform(vehicleTransform);

  { // eye sensor

    glm::vec4 newNearValue =
      vehicleTransform * glm::vec4(0, 0, constants::eyeHeight, 1);

    int sensorIndex = 0;

    for (const auto& eyeElevation : constants::eyeElevations)
      for (const auto& eyeAngle : constants::eyeAngles) {
        CarAgent::Sensor& eyeSensor = _eyeSensors.at(sensorIndex++);

        const glm::vec4 newFarValue(
          constants::eyeMaxRange * std::sin(eyeAngle),
          constants::eyeMaxRange * std::cos(eyeAngle),
          constants::eyeHeight + eyeElevation, 1.0f);

        eyeSensor.near = newNearValue;
        eyeSensor.far = vehicleTransform * newFarValue;
      }

  } // eye sensor

  { // ground sensor

    _groundSensor.near =
      vehicleTransform * glm::vec4(0, 0, constants::groundHeight, 1);
    _groundSensor.far =
      vehicleTransform *
      glm::vec4(0, 0, constants::groundHeight - constants::groundMaxRange, 1);

  } // ground sensor
}

void
CarAgent::_collideEyeSensors() {
  for (auto& sensor : _eyeSensors) {
    sensor.value = 1.0f;

    // eye sensors collide ground + walls
    Raycaster::RaycastParams params(
      sensor.near, sensor.far, 0, asValue(Groups::sensor),
      asValue(Masks::eyeSensor));

    Raycaster::RaycastParams::ResultArray<1> result;

    _physicWorld->getRaycaster().raycast(params, result);

    if (!result.hasHit)
      continue;

    sensor.far = result.allImpactsData.front().impactPoint;

    sensor.value =
      glm::length(sensor.far - sensor.near) / constants::eyeMaxRange;
  }
}

bool
CarAgent::_collideGroundSensor() {
  // raycast the ground to get the checkpoints validation

  // ground sensor collide only ground
  Raycaster::RaycastParams params(
    _groundSensor.near, _groundSensor.far, 0, asValue(Groups::sensor),
    asValue(Masks::groundSensor));

  Raycaster::RaycastParams::ResultArray<1> result;

  _physicWorld->getRaycaster().raycast(params, result);

  if (!result.hasHit)
    return false;

  _groundSensor.far = result.allImpactsData.front().impactPoint;

  _groundSensor.value = glm::length(_groundSensor.far - _groundSensor.near) /
                        constants::groundMaxRange;

  // int hitGroundIndex = params.result.impactIndex;
  const int hitGroundIndex = result.allImpactsData.front().body->getUserValue();

  // is this the next "ground geometry" index?
  if (_groundIndex + 1 == hitGroundIndex) {
    // update so it only get rewarded at the next "ground geometry"
    _groundIndex = hitGroundIndex;

    // restore health to full
    _health = constants::healthMaxValue;

    // reward the genome
    ++_fitness;
  }
  // else if (_groundIndex + 1 > hitGroundIndex)
  // {
  //     _physicBody->disableContactResponse();
  // }

  // if (_groundIndex != hitGroundIndex)
  // {
  //     _physicBody->disableContactResponse();
  // }

  return true;
}

void
CarAgent::reset(
  PhysicWorld* physicWorld, const glm::vec3& position,
  const glm::vec4& quaternion) {
  _isAlive = true;
  _fitness = 0;
  _totalUpdateNumber = 0;
  _health = constants::healthMaxValue;
  _groundIndex = 0;

  _output.steer = 0.0f;
  _output.speed = 0.0f;

  _physicWorld = physicWorld;

  _createVehicle();

  // _physicVehicle->reset();
  _physicBody->setPosition(position);
  _physicBody->setRotation(quaternion);

  _updateSensors();

  _physicVehicle->reset();
}

const CarAgent::Sensors&
CarAgent::getEyeSensors() const {
  return _eyeSensors;
}

const CarAgent::Sensor&
CarAgent::getGroundSensor() const {
  return _groundSensor;
}

float
CarAgent::getFitness() const {
  return _fitness;
}

bool
CarAgent::isAlive() const {
  return _isAlive;
}

int
CarAgent::getGroundIndex() const {
  return _groundIndex;
}

const CarAgent::NeuralNetworkOutput&
CarAgent::getNeuralNetworkOutput() const {
  return _output;
}

const PhysicBodyManager::BodyWeakRef
CarAgent::getBody() const {
  return _physicBody;
}

const PhysicVehicleManager::VehicleWeakRef
CarAgent::getVehicle() const {
  return _physicVehicle;
}

float
CarAgent::getLife() const {
  return float(_health) / constants::healthMaxValue;
}

unsigned int
CarAgent::getTotalUpdates() const {
  return _totalUpdateNumber;
}
