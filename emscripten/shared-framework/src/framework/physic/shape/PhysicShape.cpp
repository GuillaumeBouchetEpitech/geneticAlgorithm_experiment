
#include "PhysicShape.hpp"

#include "framework/helpers/internals/BulletPhysics.hpp"
#include "framework/system/ErrorHandler.hpp"
#include "framework/system/TraceLogger.hpp"

PhysicShape::PhysicShape(const PhysicShapeDef& def) : _type(def.type) {}

PhysicShape::~PhysicShape() { delete _bullet.shape, _bullet.shape = nullptr; }

PhysicShape* PhysicShape::create(const PhysicShapeDef& def, bool isDynamic) {
  switch (def.type) {
  case PhysicShapeDef::Type::sphere:
    return new SphereShape(def);
  case PhysicShapeDef::Type::capsule:
    return new CapsuleShape(def);
  case PhysicShapeDef::Type::box:
    return new BoxShape(def);
  case PhysicShapeDef::Type::staticMesh:
    return new StaticMeshShape(def, isDynamic);
  case PhysicShapeDef::Type::compound:
    return new CompoundShape(def, isDynamic);
  default:
    D_THROW(std::runtime_error, "unknown physic shape type");
  }
}

btCollisionShape* PhysicShape::getRawShape() { return _bullet.shape; }

//
//
//

CapsuleShape::CapsuleShape(const PhysicShapeDef& def) : PhysicShape(def) {
  const auto& data = def.data.capsule;
  _bullet.shape = new btCapsuleShapeZ(data.radius, data.height);
}

//
//
//

BoxShape::BoxShape(const PhysicShapeDef& def) : PhysicShape(def) {
  const glm::vec3& size = def.data.box.size;
  _bullet.shape =
    new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
}

//
//
//

SphereShape::SphereShape(const PhysicShapeDef& def) : PhysicShape(def) {
  _bullet.shape = new btSphereShape(def.data.sphere.radius);
}

//
//
//

StaticMeshShape::StaticMeshShape(const PhysicShapeDef& def, bool isDynamic)
  : PhysicShape(def) {
  if (isDynamic)
    D_THROW(std::runtime_error, "physic staticMesh cannot have non zero mass");

  const auto& data = def.data.staticMesh;

  const int32_t vertNumber = int32_t(data.verticesLength) * 3;
  _verticesData = std::make_unique<float[]>(std::size_t(vertNumber));
  const uint32_t verticesSizeInBytes =
    uint32_t(data.verticesLength) * uint32_t(sizeof(glm::vec3));
  std::memcpy(_verticesData.get(), data.verticesData, verticesSizeInBytes);
  const int verticesStride = sizeof(glm::vec3);

  const int32_t triangleNumber = int32_t(data.indicesLength) / 3;
  _indicesData = std::make_unique<int32_t[]>(std::size_t(data.indicesLength));
  const uint32_t indicesSizeInBytes =
    uint32_t(data.indicesLength) * uint32_t(sizeof(int));
  std::memcpy(_indicesData.get(), data.indicesData, indicesSizeInBytes);
  const int indicesStride = 3 * sizeof(int);

  _indexVertexArrays = new btTriangleIndexVertexArray(
    triangleNumber, _indicesData.get(), indicesStride, vertNumber,
    _verticesData.get(), verticesStride);

  constexpr bool useQuantizedAabbCompression = true;
  _bullet.shape =
    new btBvhTriangleMeshShape(_indexVertexArrays, useQuantizedAabbCompression);
}

StaticMeshShape::~StaticMeshShape() {
  delete _indexVertexArrays, _indexVertexArrays = nullptr;
}

//
//
//

CompoundShape::CompoundShape(const PhysicShapeDef& def, bool isDynamic)
  : PhysicShape(def) {
  const auto& data = def.data.compound;

  btCompoundShape* compoundShape = new btCompoundShape();

  btTransform localTrans;

  _children.reserve(data.childShapes.size());
  for (auto& shapeDef : data.childShapes) {
    localTrans.setIdentity();
    localTrans.setFromOpenGLMatrix(glm::value_ptr(shapeDef.transform));

    PhysicShape* childShape = PhysicShape::create(*shapeDef.shape, isDynamic);
    _children.push_back(childShape);

    compoundShape->addChildShape(localTrans, childShape->getRawShape());
  }

  _bullet.shape = compoundShape;
}

CompoundShape::~CompoundShape() {
  delete _bullet.shape, _bullet.shape = nullptr;
  for (PhysicShape* childShape : _children)
    delete childShape;
  _children.clear();
}
