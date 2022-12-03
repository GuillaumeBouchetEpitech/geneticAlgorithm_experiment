
#pragma once

#include "framework/containers/weak_ref_data_pool.hpp"

// #include "framework/helpers/GLMath.hpp"

#include "PhysicBodyDef.hpp"
#include "PhysicBody.hpp"

class PhysicWorld;

class PhysicBodyManager
{
  friend PhysicWorld;

public:
  using BodyContainer = weak_ref_data_pool<PhysicBody, AbstractPhysicBody, 256, false>;
  using BodyWeakRef = BodyContainer::weak_ref;

private:
  PhysicWorld& _physicWorld;
  BodyContainer _bodies;

private:
  PhysicBodyManager(PhysicWorld& physicWorld);
  ~PhysicBodyManager();

private:
  void clear();

public:
  BodyWeakRef createBody(const PhysicBodyDef& def);
  BodyWeakRef createAndAddBody(const PhysicBodyDef& def);
  void destroyBody(BodyWeakRef ref);

  void addBody(BodyWeakRef ref, short group, short mask);
  void removeBody(BodyWeakRef ref);

  BodyWeakRef getBody(unsigned int index);
  const BodyWeakRef getBody(unsigned int index) const;
  std::size_t bodySize() const;
  bool bodyEmpty() const;

};
