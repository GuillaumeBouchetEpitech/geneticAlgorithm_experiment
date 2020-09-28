
#include "ParticleManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/utilities/math/RandomNumberGenerator.hpp"

ParticleManager::Particle::Particle(const glm::vec3& position,
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
    // initialise the particle's trail
    for (auto& trailPos : trail)
        trailPos = position;
}

ParticleManager::ParticleManager()
{
    _particles.reserve(2048); // pre-allocate
    _particlesInstances.reserve(2048); // pre-allocate
}

void ParticleManager::update(float delta)
{
    // update particle's life and handle removal of dead particles
    for (unsigned int ii = 0; ii < _particles.size();)
    {
        _particles[ii].life -= delta;
        if (_particles[ii].life > 0.0f)
        {
            ++ii;
            continue;
        }

        // faster removal than erase (no reallocation)
        std::swap(_particles[ii], _particles.back());
        _particles.pop_back();
    }

    // pre-allocate (1 position + N trailing positions)
    _particlesInstances.reserve(_particles.size() * (1 + Particle::trail_size));
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
        const float localScale = particle.life / particle.maxLife * particle.scale;

        // push as instance
        for (const auto& trailPos : particle.trail)
            _particlesInstances.emplace_back(trailPos, localScale, particle.color);
        _particlesInstances.emplace_back(particle.position, localScale, particle.color);
    }

    auto& firework = Data::get().graphic.geometries.particles.firework;

    firework.updateBuffer(1, _particlesInstances);
    firework.setInstancedCount(_particlesInstances.size());
}

void ParticleManager::emitParticles(const glm::vec3& position)
{
    emitParticles(position, glm::vec3(0));
}

void ParticleManager::emitParticles(const glm::vec3& position, const glm::vec3& velocity)
{
    const unsigned int totalParticles = RNG::getRangedValue(10, 15);

    const float maxVelLength = 10.0f;
    const float velLength = std::min(glm::length(velocity), maxVelLength);
    glm::vec3 normalizedVel(0);
    if (velLength > 0)
    {
        if (velLength < maxVelLength)
            normalizedVel = velocity / maxVelLength; // smaller than max velocity
        else
            normalizedVel = glm::normalize(velocity); // max velocity
    }

    for (unsigned int ii = 0; ii < totalParticles; ++ii)
    {
        const float maxVelocity = RNG::getRangedValue(15.0f, 25.0f);

        const glm::vec3 velPertubation = {
            RNG::getRangedValue(-1.0f, 1.0f),
            RNG::getRangedValue(-1.0f, 1.0f),
            RNG::getRangedValue(-1.0f, 1.0f),
        };
        const glm::vec3 particleVel = glm::normalize(normalizedVel + velPertubation) * maxVelocity;

        const glm::vec3 color = {
            RNG::getRangedValue(0.5f, 1.0f),
            RNG::getRangedValue(0.5f, 1.0f),
            RNG::getRangedValue(0.5f, 1.0f),
        };

        const float scale = RNG::getRangedValue(0.5f, 1.5f);

        const float life = RNG::getRangedValue(0.5f, 1.5f);

        _particles.emplace_back(position, particleVel, color, scale, life);
    }
}
