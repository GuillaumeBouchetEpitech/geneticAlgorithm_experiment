
#include "ParticleManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/utilities/math/RandomNumberGenerator.hpp"

ParticleManager::ParticleManager()
{
}

void ParticleManager::update(float delta)
{
    for (unsigned int ii = 0; ii < _particles.size();)
    {
        auto& particle = _particles[ii];

        // update particle's life
        particle.life -= delta;
        if (particle.life > 0.0f)
        {
            ++ii;
            continue;
        }

        // faster removal than erase (no reallocation)
        std::swap(_particles[ii], _particles.back());
        _particles.pop_back();
    }

    struct t_attributes
    {
        glm::vec3	position;
        float       scale;
        glm::vec4	color;
    };

    std::vector<t_attributes> particlesInstances;

    // pre allocate (1 position + 5 trailing positions)
    particlesInstances.reserve(_particles.size() * D_PARTICLE_TRAIL_SIZE);

    for (auto& particle : _particles)
    {
        // make a trail by reusing the previous positions N times
        for (unsigned int ii = particle.trail.size() - 1; ii > 0; --ii)
            particle.trail[ii] = particle.trail[ii - 1];
        particle.trail[0] = particle.position;

        // update current position
        particle.position += particle.linearVelocity * delta;

        // apply fake gravity
        particle.linearVelocity.z -= 10.0f * delta;

        // update color
        const float alpha = particle.life / particle.maxLife;
        glm::vec4 particleColor = glm::vec4(particle.color, alpha);

        // push as instance
        for (unsigned int ii = 0; ii < particle.trail.size(); ++ii)
            particlesInstances.push_back({ particle.trail[ii], particle.scale, particleColor });
        particlesInstances.push_back({ particle.position, particle.scale, particleColor });
    }

    auto& firework = Data::get()->graphic.geometries.particles.firework;

    firework.updateBuffer(1, particlesInstances);
    firework.setInstancedCount(particlesInstances.size());
}

void ParticleManager::emitParticles(const glm::vec3& position)
{
    // {
    //     glm::vec3 velocity = { 0.0f, 0.0f, 10.0f };
    //     glm::vec3 color = { 1.0f, 0.0f, 0.0f };
    //     float scale = 5.0f;
    //     float life = 0.5f;

    //     _particles.push_back({ position, velocity, color, scale, life });
    // }

    const int totalParticles = t_RNG::getRangedValue(10, 15);
    const float maxVelocity = 25.0f;

    for (int ii = 0; ii < totalParticles; ++ii)
    {
        glm::vec3 velocity = {
            t_RNG::getRangedValue(-maxVelocity, maxVelocity),
            t_RNG::getRangedValue(-maxVelocity, maxVelocity),
            t_RNG::getRangedValue(-maxVelocity, maxVelocity),
        };

        glm::vec3 color = {
            t_RNG::getRangedValue(0.5f, 1.0f),
            t_RNG::getRangedValue(0.5f, 1.0f),
            t_RNG::getRangedValue(0.5f, 1.0f),
        };

        float scale = t_RNG::getRangedValue(1.0f, 1.5f);

        float life = t_RNG::getRangedValue(1.5f, 2.0f);

        _particles.push_back({ position, velocity, color, scale, life });
    }
}

ParticleManager::t_particle::t_particle(const glm::vec3& position,
                                        const glm::vec3& linearVelocity,
                                        const glm::vec3& color,
                                        float scale,
                                        float life)
    : position(position)
    , linearVelocity(linearVelocity)
    , scale(scale)
    , color(color)
    , life(life)
    , maxLife(life)
{}
 