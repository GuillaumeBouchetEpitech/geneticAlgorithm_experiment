
#include "ParticleManager.hpp"

#include "demo/logic/Data.hpp"

#include "demo/logic/graphic/helpers/generateSphereVerticesFilled.hpp"

#include "demo/logic/graphicIds.hpp"

#include "framework/asValue.hpp"

#include "framework/graphic/ResourceManager.hpp"
#include "framework/graphic/GeometryBuilder.hpp"

#include "framework/math/RandomNumberGenerator.hpp"

namespace /*anonymous*/
{

    std::array<const glm::vec3, 3> k_particleColors
    {{
        { 1.0f, 0.0f, 0.0f }, // red
        { 1.0f, 1.0f, 0.0f }, // yellow
        { 1.0f, 0.5f, 0.0f }, // orange
    }};

    glm::vec3 getRandomVec3(float radius)
    {
        return glm::vec3(
            RNG::getRangedValue(-radius, radius),
            RNG::getRangedValue(-radius, radius),
            RNG::getRangedValue(-radius, radius)
        );
    }

};

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

ParticleManager::ExplosionParticle::ExplosionParticle(
    const glm::vec3& position,
    const glm::vec3& color,
    float scale,
    float life)
    : position(position)
    , scale(scale)
    , color(color)
    , life(life)
    , maxLife(life)
{}

void ParticleManager::initialise()
{
    _shader = Data::get().graphic.resourceManager.getShader(asValue(Shaders::particles));

    {

        GeometryBuilder geometryBuilder;

        std::vector<glm::vec3> particlesVertices;
        generateSphereVerticesFilled(0.5f, 1, particlesVertices);

        geometryBuilder
            .reset()
            .setShader(*_shader)
            .setPrimitiveType(Geometry::PrimitiveType::triangles)
            .addVbo()
            .addVboAttribute("a_position", Geometry::AttrType::Vec3f, 0)
            .addVbo()
            .setVboAsInstanced()
            .addVboAttribute("a_offsetPosition", Geometry::AttrType::Vec3f, 0)
            .addVboAttribute("a_offsetScale", Geometry::AttrType::Float, 3)
            .addVboAttribute("a_offsetColor", Geometry::AttrType::Vec3f, 4);


        geometryBuilder.build(_geometry);
        _geometry.updateBuffer(0, particlesVertices);
        _geometry.setPrimitiveCount(particlesVertices.size());

    }

    _particles.reserve(2048); // pre-allocate
    _particlesInstances.reserve(2048); // pre-allocate
}

void ParticleManager::setMatricesData(const Camera::MatricesData& matricesData)
{
    _matricesData = matricesData;
}

void ParticleManager::update(float delta)
{
    // update particle's life and handle removal of dead particles
    for (std::size_t ii = 0; ii < _particles.size();)
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

    // update particle's life and handle removal of dead particles
    for (std::size_t ii = 0; ii < _explosionParticles.size();)
    {
        _explosionParticles[ii].life -= delta;
        if (_explosionParticles[ii].life > 0.0f)
        {
            ++ii;
            continue;
        }

        // faster removal than erase (no reallocation)
        std::swap(_explosionParticles[ii], _explosionParticles.back());
        _explosionParticles.pop_back();
    }

    for (auto& particle : _particles)
    {
        // make a trail by reusing the previous positions N times
        for (std::size_t ii = particle.trail.size() - 1; ii > 0; --ii)
            particle.trail[ii] = particle.trail[ii - 1];
        particle.trail[0] = particle.position;

        // update current position
        particle.position += particle.linearVelocity * delta;

        if (particle.position.z < 0.0f)
        {
            // bounce off the ground
            particle.linearVelocity.z += 100.0f * delta;
        }
        else
        {
            // apply fake gravity
            particle.linearVelocity.z -= 20.0f * delta;
        }
    }

}

void ParticleManager::render()
{

    for (auto& particle : _explosionParticles)
    {
        // update scale
        const float localScale = particle.life / particle.maxLife * particle.scale;

        _particlesInstances.emplace_back(particle.position, localScale, particle.color);
    }

    if (!_particlesInstances.empty())
    {
        if (!_shader)
            D_THROW(std::runtime_error, "shader not setup");

        _shader->bind();
        _shader->setUniform("u_composedMatrix", _matricesData.composed);

        _geometry.updateBuffer(1, _particlesInstances, true);
        _geometry.setInstancedCount(uint32_t(_particlesInstances.size()));
        _geometry.render();

        _particlesInstances.clear();
    }

    {
        auto& stackRenderer = Data::get().graphic.stackRenderer;

        for (auto& particle : _particles)
        {
            // update scale
            const float localScale = particle.life / particle.maxLife * particle.scale;

            for (std::size_t ii = 0; ii + 1 < particle.trail.size(); ++ii)
            {
                stackRenderer.pushThickTriangle3DLine(
                    particle.trail[ii + 0],
                    particle.trail[ii + 1],
                    localScale * 0.5f,
                    localScale * 0.5f,
                    glm::vec4(particle.color, 1),
                    glm::vec4(particle.color, 1));
            }
            stackRenderer.pushThickTriangle3DLine(
                particle.position,
                particle.trail[0],
                localScale * 0.5f,
                localScale * 0.5f,
                glm::vec4(particle.color, 1),
                glm::vec4(particle.color, 1));
        }
    }
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
        const glm::vec3 particleVel = glm::normalize(normalizedVel + getRandomVec3(0.25f)) * maxVelocity;
        const glm::vec3 color = k_particleColors.at(RNG::getRangedValue(0, 3));
        const float scale = RNG::getRangedValue(0.5f, 1.5f);
        const float life = RNG::getRangedValue(0.5f, 1.5f);

        _particles.emplace_back(position, particleVel, color, scale, life);
    }

    for (unsigned int ii = 0; ii < 3; ++ii)
    {
        const glm::vec3 particlePos = position + getRandomVec3(1.0f);

        const int colorIndex = RNG::getRangedValue(0, 3);
        const glm::vec3 color = k_particleColors.at(colorIndex);

        const float scale = RNG::getRangedValue(3.0f, 5.0f);

        const float life = RNG::getRangedValue(0.25f, 0.75f);

        _explosionParticles.emplace_back(particlePos, color, scale, life);
    }
}
