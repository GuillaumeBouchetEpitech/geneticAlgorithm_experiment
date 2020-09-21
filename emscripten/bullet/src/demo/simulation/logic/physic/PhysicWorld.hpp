
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
    enum class Groups : short
    {
        all         = -1,
        sensor      = (1 << 0),
        ground      = (1 << 1),
        wall        = (1 << 2),
        vehicle     = (1 << 3),
    };

    enum class Masks : short
    {
        ground              = asValue(Groups::all),
        wall                = asValue(Groups::all),
        vehicle             = asValue(Groups::ground),
        eyeSensor           = asValue(Groups::ground) | asValue(Groups::wall),
        groundSensor        = asValue(Groups::ground),
    };

public:
    typedef std::function<void(const glm::vec3&, const glm::vec3&)> t_onContact;

    typedef std::vector<glm::vec3>  t_vertices;
    typedef std::vector<int>        t_indices;

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
    void    createGround(const t_vertices& vertices, const t_indices& indices, int id);
    void    createWall(const t_vertices& vertices, const t_indices& indices);

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
    struct t_raycastParams
    {
        glm::vec3   from;
        glm::vec3   to;

        short   collisionGroup;
        short   collisionMask;

        struct t_result
        {
            bool        hasHit = false;
            glm::vec3   impactPoint;
            int         impactIndex = -1;
        }
        result;

        t_raycastParams(const glm::vec3& rayFrom,
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
    bool    raycast(t_raycastParams& params);

};
