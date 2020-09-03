
#include "ParticleManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/utilities/math/RandomNumberGenerator.hpp"

ParticleManager::ParticleManager()
{
    _particles.reserve(2048); // pre-allocate
    _particlesInstances.reserve(2048); // pre-allocate
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

    // pre-allocate (1 position + N trailing positions)
    _particlesInstances.reserve(_particles.size() * D_PARTICLE_TRAIL_SIZE);
    _particlesInstances.clear();

    for (auto& particle : _particles)
    {
        // make a trail by reusing the previous positions N times
        for (unsigned int ii = particle.trail.size() - 1; ii > 0; --ii)
            particle.trail[ii] = particle.trail[ii - 1];
        particle.trail[0] = particle.position;

        // update current position
        particle.position += particle.linearVelocity * delta;

        // apply fake gravity
        particle.linearVelocity.z -= 20.0f * delta;

        // update scale
        float localScale = particle.life / particle.maxLife * particle.scale;

        // push as instance
        for (const auto& trailPos : particle.trail)
            _particlesInstances.push_back({ trailPos, localScale, particle.color });
        _particlesInstances.push_back({ particle.position, localScale, particle.color });
    }

    auto& firework = Data::get().graphic.geometries.particles.firework;

    firework.updateBuffer(1, _particlesInstances);
    firework.setInstancedCount(_particlesInstances.size());
}

void ParticleManager::emitParticles(const glm::vec3& position)
{
    const int totalParticles = t_RNG::getRangedValue(10, 15);
    const float maxVelocity = 25.0f;

    for (int ii = 0; ii < totalParticles; ++ii)
    {
        glm::vec3 velocity = {
            t_RNG::getRangedValue(-1.0f, 1.0f),
            t_RNG::getRangedValue(-1.0f, 1.0f),
            t_RNG::getRangedValue(-1.0f, 1.0f),
        };
        velocity = glm::normalize(velocity) * maxVelocity;

        glm::vec3 color = {
            t_RNG::getRangedValue(0.5f, 1.0f),
            t_RNG::getRangedValue(0.5f, 1.0f),
            t_RNG::getRangedValue(0.5f, 1.0f),
        };

        float scale = t_RNG::getRangedValue(0.5f, 1.5f);

        float life = t_RNG::getRangedValue(0.5f, 1.5f);

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
{
    for (auto& trailPos : trail)
        trailPos = position;
}
