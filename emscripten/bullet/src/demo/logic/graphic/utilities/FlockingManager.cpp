
#include "FlockingManager.hpp"

#include "demo/logic/Data.hpp"

#include "demo/utilities/math/RandomNumberGenerator.hpp"

constexpr int totalBoids = 128;
constexpr float maxLinkRange = 75.0f;

FlockingManager::FlockingManager()
{
    _circuitKnots.reserve(256);
    _boids.reserve(totalBoids);

    _particlesInstances.reserve(1024);
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

void FlockingManager::update(float elapsedTime)
{
    // stay within the circuit
    // avoid car
    // avoid circuit

    auto& data = Data::get();
    auto& graphic = data.graphic;
    auto& geometry = graphic.geometries.particles.boids;
    const auto& logic = data.logic;
    const auto& boundaries = logic.circuitAnimation.boundaries;
    const auto& simulation = *logic.simulation;

    { // activation animation

        bool noneActivated = true;
        bool allActivated = true;

        for (auto& boid : _boids)
        {
            if (boid.activated)
            {
                noneActivated = false;

                if (boid.activationTime > 0.0f)
                    allActivated = false;
            }
            else
            {
                allActivated = false;
            }
        }

        if (allActivated)
        {
            // reset
            for (auto& boid : _boids)
            {
                boid.activated = false;
                boid.parent = nullptr;
            }

            noneActivated = true;
        }

        if (noneActivated)
        {
            const int boidIndex = RNG::getRangedValue(0, totalBoids);

            auto& boid = _boids[boidIndex];

            boid.activated = true;
            boid.activationTime = 1.0f;
        }

        // update
        for (unsigned int ii = 0; ii < _boids.size(); ++ii)
        {
            if (!_boids[ii].activated)
                continue;

            if (_boids[ii].activationTime > 0.0f)
                _boids[ii].activationTime -= elapsedTime;

            if (_boids[ii].activationTime < 0.5f)
            {
                // spread it
                for (unsigned int jj = 0; jj < _boids.size(); ++jj)
                {
                    if (ii == jj)
                        continue;

                    if (_boids[jj].activated)
                        continue;

                    const float distance = glm::distance(_boids[ii].pos, _boids[jj].pos);

                    if (distance > maxLinkRange)
                        continue;

                    _boids[jj].activated = true;
                    _boids[jj].activationTime = 1.0f;
                    _boids[jj].parent = &_boids[ii];
                }
            }

            if (_boids[ii].activationTime < 0.0f)
                _boids[ii].activationTime = 0.0f;
        }

    } // activation animation

    _particlesInstances.clear();

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

                diff = diff / magnitude; // normalise

                if (magnitude > 10.0f)
                {
                    // cohesion
                    sum -= diff * 1.0f;
                }
                else if (magnitude < 5.0f)
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

                const glm::vec3 carPos = carData.transforms.chassis * glm::vec4(0, 0, 0, 1);
                const glm::vec3 diff = currBoid.pos - carPos;
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

        constexpr float maxAcc = 0.025f * 60.0f;
        constexpr float maxVel = 0.25f * 60.0f;

        // limitate acceleration
        const float accMagnitude = glm::length(acc);
        if (accMagnitude > 0.0f)
        {
            acc = (acc / accMagnitude) * maxAcc;

            currBoid.vel += acc * elapsedTime;

            // limitate velocity
            const float velMagnitude = glm::length(currBoid.vel);
            if (velMagnitude > 0.0f)
            {
                currBoid.vel = (currBoid.vel / velMagnitude) * maxVel * elapsedTime;
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

        if (currBoid.activated && currBoid.activationTime > 0.0f)
        {
            float scale = 1.0f;

            if (currBoid.activationTime > 0.5f)
                scale += (1.0f - (currBoid.activationTime - 0.5f) * 2.0f) * 4.0f;
            else
                scale += (currBoid.activationTime * 2.0f) * 4.0f;

            _particlesInstances.emplace_back(currBoid.pos, scale, glm::vec3(1, 0, 0));
        }
        else
        {
            _particlesInstances.emplace_back(currBoid.pos, 1, glm::vec3(0.5f, 0.5f, 1.0f));
        }

        if (currBoid.activated && currBoid.activationTime > 0.5f && currBoid.parent)
        {
            const float coef = (1.0f - (currBoid.activationTime - 0.5f) * 2.0f) * 1.0f;

            glm::vec3 pos = currBoid.parent->pos + (currBoid.pos - currBoid.parent->pos) * coef;

            _particlesInstances.emplace_back(pos, 3.0f, glm::vec3(1, 1, 1));
        }
    }

    geometry.updateBuffer(1, _particlesInstances);
    geometry.setInstancedCount(_particlesInstances.size());
}

void FlockingManager::render(const glm::mat4& sceneMatrix) const
{
    auto& graphic = Data::get().graphic;

    { // boids here

        const auto& shader = *graphic.shaders.particles;
        const auto& geometry = graphic.geometries.particles.boids;

        shader.bind();
        shader.setUniform("u_composedMatrix", sceneMatrix);

        //
        //

        geometry.render();

    } // boids here

    { // links here

        auto& stackRenderer = graphic.stackRenderer;

        const auto& shader = *graphic.shaders.stackRenderer;

        shader.bind();
        shader.setUniform("u_composedMatrix", sceneMatrix);

        //
        //

        const glm::vec3 lowColor{0.0f, 0.0f, 1.0f};
        const glm::vec3 highColor{1.0f, 0.0f, 0.0f};

        for (int ii = 0; ii < totalBoids; ++ii)
        {
            const auto& currBoid = _boids[ii];

            float coefA = 0.0f;
            if (currBoid.activated)
            {
                if (currBoid.activationTime > 0.5f)
                    coefA = (1.0f - (currBoid.activationTime - 0.5f) * 2.0f);
                else
                    coefA = (currBoid.activationTime * 2.0f);
            }

            for (int jj = ii + 1; jj < totalBoids; ++jj)
            {
                const auto& otherBoid = _boids[jj];

                const float distance = glm::distance(currBoid.pos, otherBoid.pos);

                if (distance > maxLinkRange)
                    continue;

                float coefB = 0.0f;
                if (otherBoid.activated)
                {
                    if (otherBoid.activationTime > 0.5f)
                        coefB = (1.0f - (otherBoid.activationTime - 0.5f) * 2.0f);
                    else
                        coefB = (otherBoid.activationTime * 2.0f);
                }

                glm::vec3 colorA = glm::mix(lowColor, highColor, coefA);
                glm::vec3 colorB = glm::mix(lowColor, highColor, coefB);
                stackRenderer.pushThickTriangle3DLine(
                    currBoid.pos, otherBoid.pos,
                    0.1f + coefA * 0.3f, 0.1f + coefB * 0.3f,
                    colorA, colorB);
            }
        }

        stackRenderer.flushLines();

    } // links here
}
