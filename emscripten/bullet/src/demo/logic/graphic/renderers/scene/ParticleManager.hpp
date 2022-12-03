
#pragma once

#include "framework/graphic/camera/Camera.hpp"

#include "framework/graphic/ShaderProgram.hpp"
#include "framework/graphic/Geometry.hpp"

#include "framework/helpers/GLMath.hpp"

#include <vector>
#include <array>
#include <memory>

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

    struct ExplosionParticle
    {
        glm::vec3   position;
        float       scale;
        glm::vec3   color;
        float       life;
        float       maxLife;

        ExplosionParticle(
            const glm::vec3& position,
            const glm::vec3& color,
            float scale,
            float life);
    };
    using ExplosionParticles = std::vector<ExplosionParticle>;

private:

    struct ParticleInstance
    {
        glm::vec3   position;
        float       scale;
        glm::vec3   color;

        ParticleInstance(
            const glm::vec3& position,
            float scale,
            const glm::vec3& color)
            : position(position)
            , scale(scale)
            , color(color)
        {}
    };

    Particles _particles;
    ExplosionParticles _explosionParticles;

    std::vector<ParticleInstance> _particlesInstances;
    std::shared_ptr<ShaderProgram> _shader;
    Geometry _geometry;

    Camera::MatricesData _matricesData;

public:
    ParticleManager() = default;

public:
    void initialise();
    void setMatricesData(const Camera::MatricesData& matricesData);

public:
    void update(float delta);
    void render();

public:
    void emitParticles(const glm::vec3& position, const glm::vec3& velocity);
};
