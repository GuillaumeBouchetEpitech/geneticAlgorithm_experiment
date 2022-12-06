
#pragma once

#include "PhysicDefinitions.hpp"

class btFixedConstraint;

class PhysicWorld;

class PhysicFixedConstraint {
private:
  friend PhysicWorld;

private:
  struct bullet {
    btFixedConstraint* constraint = nullptr;
  } _bullet;

public:
  PhysicFixedConstraint(const PhysicFixedConstraintDef& def);
  ~PhysicFixedConstraint();
};
