
#include "PhysicFixedConstraint.hpp"

#include "PhysicBody.hpp"

#include "helpers/BulletPhysics.hpp"

PhysicFixedConstraint::PhysicFixedConstraint(
  const PhysicFixedConstraintDef& def) {
  btTransform frameInA = btTransform::getIdentity();
  btTransform frameInB = btTransform::getIdentity();
  frameInA.setFromOpenGLMatrix(glm::value_ptr(def.transformA));
  frameInB.setFromOpenGLMatrix(glm::value_ptr(def.transformB));

  _bullet.constraint = new btFixedConstraint(
    *def.body_a->_bullet.body, *def.body_b->_bullet.body, frameInA, frameInB);

  _bullet.constraint->setDbgDrawSize(2.0f);
}

PhysicFixedConstraint::~PhysicFixedConstraint() { delete _bullet.constraint; }
