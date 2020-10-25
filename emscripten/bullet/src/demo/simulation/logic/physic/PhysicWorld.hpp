
#pragma once


#include "./demo/utilities/types.hpp"

#include "thirdparty/GLMath.hpp"

#include <vector>
#include <unordered_set>
#include <functional>

class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btIDebugDraw;

class PhysicTrimesh;
class PhysicVehicle;

class PhysicWorld
{
public:
    enum class Groups : unsigned short
    {
        all     = 0b1111'1111'1111'1111,
        sensor  = 0b0000'0000'0000'0001,
        ground  = 0b0000'0000'0000'0010,
        wall    = 0b0000'0000'0000'0100,
        vehicle = 0b0000'0000'0000'1000,
    };

    enum class Masks : unsigned short
    {
        ground       = asValue(Groups::all),
        wall         = asValue(Groups::all),
        vehicle      = asValue(Groups::ground),
        eyeSensor    = asValue(Groups::ground) | asValue(Groups::wall),
        groundSensor = asValue(Groups::ground),
    };

public:
    using Vertices = std::vector<glm::vec3>;
    using Indices = std::vector<int>;

private:

    struct bullet
    {
        btBroadphaseInterface*                  broadphase = nullptr;
        btDefaultCollisionConfiguration*        collisionConfiguration = nullptr;
        btCollisionDispatcher*                  dispatcher = nullptr;
        btSequentialImpulseConstraintSolver*    solver = nullptr;
        btDiscreteDynamicsWorld*                dynamicsWorld = nullptr;
    }
    _bullet;

    // t_onContact  m_onContact;

public:
    PhysicWorld();
    ~PhysicWorld();

    //
    //
    // world

public:
    void    step();

    //
    //
    // trimesh

private:
    std::vector<PhysicTrimesh*> _groundsTrimesh;
    std::vector<PhysicTrimesh*> _wallsTrimesh;
public:
    void    createGround(const Vertices& vertices, const Indices& indices, int id);
    void    createWall(const Vertices& vertices, const Indices& indices);

    //
    //
    // vehicle

private:
    std::vector<PhysicVehicle*>         _vehicles;
    std::unordered_set<PhysicVehicle*>  _liveVehicles;
public:
    PhysicVehicle*  createVehicle();
    void            destroyVehicle(PhysicVehicle* vehicle);
    void            addVehicle(PhysicVehicle& vehicle);
    void            removeVehicle(PhysicVehicle& vehicle);

    const std::vector<PhysicVehicle*>&  getVehicles() const;

    //
    //
    // raycast

public:
    struct RaycastParams
    {
        glm::vec3   from;
        glm::vec3   to;

        short   collisionGroup;
        short   collisionMask;

        struct Result
        {
            bool        hasHit = false;
            glm::vec3   impactPoint;
            int         impactIndex = -1;
        }
        result;

        RaycastParams(const glm::vec3& rayFrom,
                      const glm::vec3& rayTo,
                      short group = asValue(Groups::all),
                      short mask = asValue(Groups::all))
            : from(rayFrom)
            , to(rayTo)
            , collisionGroup(group)
            , collisionMask(mask)
        {}
    };

public:
    bool    raycast(RaycastParams& params);

};
