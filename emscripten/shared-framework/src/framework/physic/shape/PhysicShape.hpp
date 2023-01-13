
#pragma once

#include "PhysicShapeDef.hpp"

#include <memory>

class btCollisionShape;
class btTriangleIndexVertexArray;

class PhysicShape {
protected:
  PhysicShapeDef::Type _type;

  struct Bullet {
    btCollisionShape* shape = nullptr;
  } _bullet;

protected:
  PhysicShape(const PhysicShapeDef& def);

public:
  PhysicShape(const PhysicShape& other) = delete;
  PhysicShape& operator=(const PhysicShape& other) = delete;

  PhysicShape(PhysicShape&& other) = delete;
  PhysicShape& operator=(PhysicShape&& other) = delete;

  virtual ~PhysicShape();

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
  std::unique_ptr<float[]> _verticesData{nullptr};
  std::unique_ptr<int32_t[]> _indicesData{nullptr};
  btTriangleIndexVertexArray* _indexVertexArrays = nullptr;

protected:
  StaticMeshShape(const PhysicShapeDef& def, bool isDynamic);

public:
  ~StaticMeshShape();
};

//
//
//

class CompoundShape : public PhysicShape {
  friend PhysicShape;

private:
  std::vector<PhysicShape*> _children;

protected:
  CompoundShape(const PhysicShapeDef& def, bool isDynamic);

public:
  ~CompoundShape();
};
