
#pragma once

#include "PhysicDefinitions.hpp"

class btHingeConstraint;

class PhysicWorld;

class PhysicHingeConstraint {
private:
  friend PhysicWorld;

private:
  struct bullet {
    btHingeConstraint* constraint = nullptr;
  } _bullet;

public:
  PhysicHingeConstraint(const PhysicHingeConstraintDef& def);
  ~PhysicHingeConstraint();
};
