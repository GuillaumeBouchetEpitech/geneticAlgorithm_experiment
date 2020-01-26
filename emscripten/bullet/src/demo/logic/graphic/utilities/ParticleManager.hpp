
#pragma once

#include "thirdparty/GLMath.hpp"

#include <vector>
#include <array>

#define D_PARTICLE_TRAIL_SIZE 6

class ParticleManager
{
public:
    ParticleManager();

public:
    void update(float delta);

public:
    void emitParticles(const glm::vec3& position);

private:
    struct t_particle
    {
        glm::vec3   position;
        glm::vec3   linearVelocity;
        float       scale;
        glm::vec3   color;
        float       life;
        float       maxLife;
        std::array<glm::vec3, D_PARTICLE_TRAIL_SIZE>    trail;

        t_particle(const glm::vec3& position,
                   const glm::vec3& linearVelocity,
                   const glm::vec3& color,
                   float scale,
                   float life);
    };
    typedef std::vector<t_particle> t_particles;

private:
    t_particles _particles;

};
