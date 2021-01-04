
#pragma once

#include "thirdparty/GLMath.hpp"

#include <vector>
#include <array>

class ParticleManager
{
private:
    struct Particle
    {
        glm::vec3   position;
        glm::vec3   linearVelocity;
        float       scale;
        glm::vec3   color;
        float       life;
        float       maxLife;

        static constexpr std::size_t trail_size = 6;
        std::array<glm::vec3, trail_size> trail;

        Particle(const glm::vec3& position,
                 const glm::vec3& linearVelocity,
                 const glm::vec3& color,
                 float scale,
                 float life);
    };
    using Particles = std::vector<Particle>;

private:

    struct t_attributes
    {
        glm::vec3   position;
        float       scale;
        glm::vec3   color;

        t_attributes(const glm::vec3& position, float scale, const glm::vec3& color)
            : position(position)
            , scale(scale)
            , color(color)
        {}
    };

    std::vector<t_attributes> _particlesInstances;

    Particles _particles;

public:
    ParticleManager();

public:
    void update(float delta);

public:
    void emitParticles(const glm::vec3& position);
    void emitParticles(const glm::vec3& position, const glm::vec3& velocity);
};
