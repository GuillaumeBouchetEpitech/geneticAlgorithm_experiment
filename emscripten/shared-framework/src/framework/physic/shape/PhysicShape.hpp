
#pragma once

#include "PhysicShapeDef.hpp"

class btCollisionShape;

class PhysicShape {
protected:
  PhysicShapeDef::Type _type;

  struct Bullet {
    btCollisionShape* shape = nullptr;
  } _bullet;

protected:
  PhysicShape(const PhysicShapeDef& def);

public:
  ~PhysicShape();

public:
  static PhysicShape* create(const PhysicShapeDef& def, bool isDynamic);

public:
  btCollisionShape* getRawShape();
};

//
//
//

class CapsuleShape : public PhysicShape {
  friend PhysicShape;

protected:
  CapsuleShape(const PhysicShapeDef& def);
};

//
//
//

class BoxShape : public PhysicShape {
  friend PhysicShape;

protected:
  BoxShape(const PhysicShapeDef& def);
};

//
//
//

class SphereShape : public PhysicShape {
  friend PhysicShape;

protected:
  SphereShape(const PhysicShapeDef& def);
};

//
//
//

class StaticMeshShape : public PhysicShape {
  friend PhysicShape;

private:
  float* _verticesData = nullptr;
  int32_t* _indicesData = nullptr;

protected:
  StaticMeshShape(const PhysicShapeDef& def, bool isDynamic);
  ~StaticMeshShape();
};

//
//
//

class CompoundShape : public PhysicShape {
  friend PhysicShape;

protected:
  CompoundShape(const PhysicShapeDef& def, bool isDynamic);
  ~CompoundShape();
};
