
#include "FlockingManager.hpp"

#include "demo/logic/Data.hpp"

#include "demo/utilities/math/RandomNumberGenerator.hpp"

constexpr int totalBoids = 128;

FlockingManager::FlockingManager()
{
    _circuitKnots.reserve(256);
    _boids.reserve(totalBoids);
}

void FlockingManager::initialise()
{
    // generate within the circuit

    const auto& data = Data::get();
    const auto& boundaries = data.logic.circuitAnimation.boundaries;

    for (int ii = 0; ii < totalBoids; ++ii)
    {
        Boid newBoid;
        newBoid.pos.x = RNG::getRangedValue(boundaries.min.x, boundaries.max.x);
        newBoid.pos.y = RNG::getRangedValue(boundaries.min.y, boundaries.max.y);
        newBoid.pos.z = RNG::getRangedValue(boundaries.min.z, boundaries.max.z);

        _boids.push_back(newBoid);
    }

    // force the simulation to be in a stable state
    for (int ii = 0; ii < 1000; ++ii)
        update(1.0f / 60.0f);
}

void FlockingManager::addKnot(const glm::vec3& knot)
{
    _circuitKnots.push_back(knot);
}

void FlockingManager::update(float delta)
{
    // stay within the circuit
    // avoid car
    // avoid circuit

    auto& graphic = Data::get().graphic;
    auto& geometry = graphic.geometries.particles.boids;

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

    std::vector<t_attributes> particlesInstances;

    const auto& data = Data::get();
    const auto& logic = data.logic;
    const auto& boundaries = logic.circuitAnimation.boundaries;
    const auto& simulation = *logic.simulation;

    for (int ii = 0; ii < totalBoids; ++ii)
    {
        auto& currBoid = _boids[ii];

        glm::vec3 acc{0, 0, 0};

        // boids
        // - cohesion
        // - separation
        // within the circuit
        // circuit
        // - separation
        // - cohesion
        // avoid car

        { // ciruit cohesion/separation

            glm::vec3 sum{0, 0, 0};
            int total = 0;

            constexpr float maxRange = 100.0f;

            for (const glm::vec3& knot : _circuitKnots)
            {
                glm::vec3 diff = currBoid.pos - knot;

                const float magnitude = glm::length(diff);

                if (magnitude == 0.0f || magnitude > maxRange)
                    continue;

                // normalise
                diff = diff / magnitude;
                // diff = diff;
                // diff = diff / maxRange;

                if (magnitude > 10.0f)
                {
                    // cohesion
                    sum -= diff * 1.0f;
                }
                else
                {
                    // separate
                    sum += diff * 2.0f;
                }

                ++total;
            }

            if (total > 0)
            {
                acc += (sum / float(total)) * 2.0f;
            }
            else
            {
                acc += boundaries.center - currBoid.pos;
            }

        } // ciruit cohesion/separation

        { // separation from other boids

            constexpr float maxRange = 50.0f;

            glm::vec3 sum{0, 0, 0};
            int total = 0;

            for (int jj = 0; jj < totalBoids; ++jj)
            {
                if (ii == jj)
                    continue;

                auto& otherBoid = _boids[jj];

                glm::vec3 diff = currBoid.pos - otherBoid.pos;

                const float distance = glm::length(diff);

                if (distance > maxRange)
                    continue;

                sum += diff / distance;
                ++total;
            }

            if (total > 0)
            {
                acc += (sum / float(total)) * 2.0f;
            }

        } // separation from other boids

        { // separation from cars

            constexpr float maxRange = 50.0f;

            glm::vec3 sum{0, 0, 0};
            int total = 0;

            const unsigned int totalCars = simulation.getTotalCars();

            for (unsigned int ii = 0; ii < totalCars; ++ii)
            {
                const auto& carData = simulation.getCarResult(ii);

                if (!carData.isAlive)
                    continue;

                const glm::vec3 carPos = carData.transform * glm::vec4(0, 0, 0, 1);

                glm::vec3 diff = currBoid.pos - carPos;

                const float distance = glm::length(diff);

                if (distance > maxRange)
                    continue;

                sum += diff / distance;
                ++total;
            }

            if (total > 0)
            {
                acc += (sum / float(total)) * 5.0f;
            }

        } // separation from cars

        constexpr float maxAcc = 0.025f;
        constexpr float maxVel = 0.25f;

        // limitate acceleration
        float accMagnitude = glm::length(acc);
        if (accMagnitude > 0.0f)
        {
            acc = (acc / accMagnitude) * maxAcc;

            currBoid.vel += acc;

            // limitate velocity
            float velMagnitude = glm::length(currBoid.vel);
            if (velMagnitude > 0.0f || velMagnitude > maxVel)
            {
                currBoid.vel = (currBoid.vel / velMagnitude) * maxVel;
            }
        }

        currBoid.pos += currBoid.vel;

        if (currBoid.pos.z > boundaries.max.z)
            currBoid.pos.z = boundaries.max.z;
        if (currBoid.pos.z < boundaries.min.z)
            currBoid.pos.z = boundaries.min.z;

        if (currBoid.pos.x > boundaries.max.x)
            currBoid.pos.x = boundaries.max.x;
        if (currBoid.pos.x < boundaries.min.x)
            currBoid.pos.x = boundaries.min.x;

        if (currBoid.pos.y > boundaries.max.y)
            currBoid.pos.y = boundaries.max.y;
        if (currBoid.pos.y < boundaries.min.y)
            currBoid.pos.y = boundaries.min.y;

        particlesInstances.emplace_back(currBoid.pos, 1, glm::vec3(1, 1, 1));
    }

    geometry.updateBuffer(1, particlesInstances);
    geometry.setInstancedCount(particlesInstances.size());
}

// void FlockingManager::render(const Data::Graphic::CameraData::MatricesData::Matrices& matrices) const
// void FlockingManager::render(const glm::mat4& projection, const glm::mat4& modelview) const
void FlockingManager::render(const glm::mat4& sceneMatrix) const
{
    auto& graphic = Data::get().graphic;



    { // boids here

        const auto& shader = *graphic.shaders.particles;
        const auto& geometry = graphic.geometries.particles.boids;

        shader.bind();

        GLint composedMatrixLoc = shader.getUniform("u_composedMatrix");
        glUniformMatrix4fv(composedMatrixLoc, 1, false, glm::value_ptr(sceneMatrix));

        //
        //

        geometry.render();

    } // boids here

    { // links here

        auto& stackRenderer = graphic.stackRenderer;

        const auto& shader = *graphic.shaders.stackRenderer;

        shader.bind();

        GLint composedMatrixLoc = shader.getUniform("u_composedMatrix");
        glUniformMatrix4fv(composedMatrixLoc, 1, false, glm::value_ptr(sceneMatrix));

        //
        //

        const glm::vec3 lowColor{0.05f, 0.05f, 0.5f};
        const glm::vec3 highColor{0.25f, 0.25f, 0.9f};
        constexpr float maxRange = 75.0f;


        for (int ii = 0; ii < totalBoids; ++ii)
        {
            auto& currBoid = _boids[ii];

            for (int jj = ii + 1; jj < totalBoids; ++jj)
            {
                auto& otherBoid = _boids[jj];

                const float distance = glm::distance(currBoid.pos, otherBoid.pos);

                if (distance > maxRange)
                    continue;

                // stackRenderer.pushLine(currBoid.pos, otherBoid.pos, {1, 1, 1});

                const float coef = distance / maxRange;

                // lerp
                glm::vec3 color = lowColor + (highColor - lowColor) * coef;

                stackRenderer.pushLine(currBoid.pos, otherBoid.pos, color);
            }
        }

        stackRenderer.flushLines();

    } // links here
}
