
#include "demo/defines.hpp"

#include "Data.hpp"

#include "demo/states/StateManager.hpp"

#include <iomanip>
#include <sstream>

void Data::initialiseSimulationCallbacks()
{
    auto& allStats = logic.fitnessStats.allStats;
    std::memset(&allStats[0], 0, allStats.size() * sizeof(allStats[0]));

// #if defined D_WEB_WEBWORKER_BUILD

//     logic.simulation->setOnWorkersReadyCallback([this]() -> void
//     {
//         // leave the "WEB WORKERS LOADING" message for at least 1 second
//         logic.state.countdown = 1000;
//     });

// #endif

    logic.simulation->setOnGenerationResetCallback([this]() -> void
    {
        { // handle the car trails

            const auto& simulation = *logic.simulation;
            const unsigned int totalCars = simulation.getTotalCars();

            logic.carWheelsTrails.reset(simulation);

            for (unsigned int ii = 0; ii < totalCars; ++ii)
            {
                const auto& carData = simulation.getCarResult(ii);

                // initialise the new data

                for (auto& transforms : carData.latestTransformsHistory)
                {
                    for (std::size_t jj = 0; jj < transforms.wheels.size(); ++jj)
                    {
                        glm::vec3 wheelOrigin = transforms.wheels[jj] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

                        logic.carWheelsTrails.push(ii, jj, wheelOrigin);
                    }
                }
            }

        } // handle the car trails
    });

    logic.simulation->setOnGenerationStepCallback([this]() -> void
    {
        const auto& simulation = *logic.simulation;

        { // handle the car trails

            const unsigned int totalCars = simulation.getTotalCars();

            for (unsigned int ii = 0; ii < totalCars; ++ii)
            {
                const auto& carData = simulation.getCarResult(ii);

                if (carData.isAlive == false)
                    continue;

                for (auto& transforms : carData.latestTransformsHistory)
                {
                    for (std::size_t jj = 0; jj < transforms.wheels.size(); ++jj)
                    {
                        glm::vec3 wheelOrigin = transforms.wheels[jj] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

                        logic.carWheelsTrails.push(ii, jj, wheelOrigin);
                    }
                }
            }

        } // handle the car trails

        { // handle the core data

            auto& cores = logic.cores;

            cores.profileData.clearLatest();
            for (unsigned int coreIndex = 0; coreIndex < simulation.getTotalCores(); ++coreIndex)
                cores.profileData.addToLatest(simulation.getCoreState(coreIndex));
            cores.profileData.pushLatest();

        } // handle the core data
    });

    logic.simulation->setOnGenomeDieCallback([this](unsigned int genomeIndex) -> void
    {
        const auto& simulation = *logic.simulation;

        const auto& carData = simulation.getCarResult(genomeIndex);

        const glm::vec3 extraHeight(0.0f, 0.0f, 1.0f);
        glm::vec4 carPos = carData.liveTransforms.chassis * glm::vec4(extraHeight, 1.0f);

        graphic.particleManager.emitParticles(carPos, carData.velocity);
    });

    logic.simulation->setOnGenerationEndCallback([this](bool isSmarter) -> void
    {
        { // handle the stats

            const auto& bestGenome = logic.simulation->getBestGenome();
            auto& fitnessStats = logic.fitnessStats;
            auto& allStats = fitnessStats.allStats;

            /**
             * => if last stat was smartest
             * ===> erase first and push_back new one
             * => else
             * ===> erase back and push_back new one
             */

            const float prevLastOne = allStats[allStats.size() - 2];
            const float currLastOne = allStats[allStats.size() - 1];

            if (currLastOne > prevLastOne)
            {
                // last one is smarter -> erase first, add last
                std::memmove(&allStats[0], &allStats[1], 9 * sizeof(allStats[0]));
                allStats.back() = bestGenome.fitness;
            }
            else
            {
                // last one is not smarter -> replace last
                allStats.back() = bestGenome.fitness;
            }

        } // handle the stats

        { // handle the car trails

            if (isSmarter)
            {
                // const auto& bestGenome = logic.simulation->getBestGenome();
                // auto& currentTrailIndex = graphic.geometries.wireframes.currentTrailIndex;

                // const auto& bestWheelsTrailData = logic.carWheelsTrails.getTrailById(bestGenome.id);

                // auto& bestNewCarsTrails = graphic.geometries.wireframes.bestNewCarsTrails;
                // auto& currCarNewTrail = bestNewCarsTrails[currentTrailIndex];

                // for (std::size_t ii = 0; ii < currCarNewTrail.wheels.size(); ++ii)
                // {
                //     currCarNewTrail.wheels[ii].updateBuffer(0, bestWheelsTrailData.wheels[ii]);
                //     currCarNewTrail.wheels[ii].setPrimitiveCount(bestWheelsTrailData.wheels[ii].size());
                // }

                // // increase the currently used trail index (loop if too high)
                // currentTrailIndex = (currentTrailIndex + 1) % bestNewCarsTrails.size();

                graphic.carTailsRenderer.updateLatestTrail();
            }

        } // handle the car trails

        { // switch the current state

            StateManager::get()->changeState(StateManager::States::EndGeneration);

            logic.leaderCar.index = -1;

        } // switch the current state
    });
}

