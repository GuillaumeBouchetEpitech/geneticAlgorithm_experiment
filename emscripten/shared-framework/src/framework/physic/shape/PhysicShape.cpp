
#include "PhysicShape.hpp"

#include "framework/TraceLogger.hpp"
#include "framework/ErrorHandler.hpp"

#include "framework/helpers/internals/BulletPhysics.hpp"

PhysicShape::PhysicShape(const PhysicShapeDef& def)
  : _type(def.type)
{}

PhysicShape::~PhysicShape()
{
  delete _bullet.shape, _bullet.shape = nullptr;
}

PhysicShape* PhysicShape::create(const PhysicShapeDef& def, bool isDynamic)
{
  switch (def.type)
  {
  case PhysicShapeDef::Type::sphere : return new SphereShape(def);
  case PhysicShapeDef::Type::capsule : return new CapsuleShape(def);
  case PhysicShapeDef::Type::box : return new BoxShape(def);
  case PhysicShapeDef::Type::staticMesh : return new StaticMeshShape(def, isDynamic);
  case PhysicShapeDef::Type::compound : return new CompoundShape(def, isDynamic);
  default: D_THROW(std::runtime_error, "unknown physic shape type");
  }
}

btCollisionShape* PhysicShape::getRawShape()
{
  return _bullet.shape;
}

//
//
//

CapsuleShape::CapsuleShape(const PhysicShapeDef& def)
  : PhysicShape(def)
{
  const auto& data = def.data.capsule;
  _bullet.shape = new btCapsuleShapeZ(data.radius, data.height);
}

//
//
//

BoxShape::BoxShape(const PhysicShapeDef& def)
  : PhysicShape(def)
{
  const glm::vec3& size = def.data.box.size;
  _bullet.shape = new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
}

//
//
//

SphereShape::SphereShape(const PhysicShapeDef& def)
  : PhysicShape(def)
{
  _bullet.shape = new btSphereShape(def.data.sphere.radius);
}

//
//
//

StaticMeshShape::StaticMeshShape(const PhysicShapeDef& def, bool isDynamic)
  : PhysicShape(def)
{
  if (isDynamic)
    D_THROW(std::runtime_error, "physic staticMesh cannot have non zero mass");

  const auto& data = def.data.staticMesh;

  const int32_t vertNumber = int32_t(data.verticesLength) * 3;
  _verticesData = new float[std::size_t(vertNumber)];
  const uint32_t verticesSizeInBytes = uint32_t(data.verticesLength) * uint32_t(sizeof(glm::vec3));
  std::memcpy(_verticesData, data.verticesData, verticesSizeInBytes);
  const int verticesStride = sizeof(glm::vec3);

  const int32_t triangleNumber = int32_t(data.indicesLength) / 3;
  _indicesData = new int32_t[std::size_t(data.indicesLength)];
  const uint32_t indicesSizeInBytes = uint32_t(data.indicesLength) * uint32_t(sizeof(int));
  std::memcpy(_indicesData, data.indicesData, indicesSizeInBytes);
  const int indicesStride = 3 * sizeof(int);

  btTriangleIndexVertexArray* indexVertexArrays = new btTriangleIndexVertexArray(
    triangleNumber, _indicesData, indicesStride,
    vertNumber, _verticesData, verticesStride);

  const bool useQuantizedAabbCompression = true;
  _bullet.shape = new btBvhTriangleMeshShape(indexVertexArrays, useQuantizedAabbCompression);
}

StaticMeshShape::~StaticMeshShape()
{
  delete _bullet.shape, _bullet.shape = nullptr;
  delete _verticesData, _verticesData = nullptr;
  delete _indicesData, _indicesData = nullptr;
}

//
//
//

CompoundShape::CompoundShape(const PhysicShapeDef& def, bool isDynamic)
  : PhysicShape(def)
{
  const auto& data = def.data.compound;

  btCompoundShape* compound = new btCompoundShape();

  btTransform localTrans;

  for (auto& childShape : data.childShapes)
  {
    localTrans.setIdentity();
    localTrans.setFromOpenGLMatrix(glm::value_ptr(childShape.transform));

    PhysicShape* child = PhysicShape::create(*childShape.shape, isDynamic);

    compound->addChildShape(localTrans, child->getRawShape());
  }

  _bullet.shape = compound;
}

CompoundShape::~CompoundShape()
{
  btCompoundShape* compound = static_cast<btCompoundShape*>(_bullet.shape);
  while (compound->getNumChildShapes() > 0)
  {
    delete compound->getChildShape(0);
    compound->removeChildShape(0);
  }

  delete _bullet.shape, _bullet.shape = nullptr;
}

