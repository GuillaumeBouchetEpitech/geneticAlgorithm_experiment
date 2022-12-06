
#include "PhysicHingeConstraint.hpp"

#include "PhysicBody.hpp"

#include "helpers/BulletPhysics.hpp"

PhysicHingeConstraint::PhysicHingeConstraint(
  const PhysicHingeConstraintDef& def) {
  btVector3 pivot_in_a = btVector3(def.coord_a.x, def.coord_a.y, def.coord_a.z);
  btVector3 pivot_in_b = btVector3(def.coord_b.x, def.coord_b.y, def.coord_b.z);
  // btVector3 axisInA = btVector3(def.axis.x, def.axis.y, def.axis.z);
  // btVector3 axisInB = btVector3(def.axis.x, def.axis.y, def.axis.z);
  btVector3 axis = btVector3(def.axis.x, def.axis.y, def.axis.z);
  bool useReferenceFrameA = true;

  _bullet.constraint =
    new btHingeConstraint(*def.body_a->_bullet.body, *def.body_b->_bullet.body,
                          pivot_in_a, pivot_in_b,
                          // axisInA, axisInB,
                          axis, axis, useReferenceFrameA);

  // const float limitLow = -1.5f;
  // const float limitHigh = +1.5f;
  // _bullet.constraint->setLimit(limitLow, limitHigh);

  // _bullet.constraint->enableAngularMotor(true, 10, 10);

  _bullet.constraint->setDbgDrawSize(2.0f);
}

PhysicHingeConstraint::~PhysicHingeConstraint() { delete _bullet.constraint; }
