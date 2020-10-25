
#include "demo/defines.hpp"

#include "Data.hpp"

#include "demo/states/StateManager.hpp"

#include <iomanip>
#include <sstream>

void Data::initialiseSimulationCallbacks()
{
    logic.fitnessStats.allStats.resize(logic.fitnessStats.maxStats, 0.0f);

#if defined D_WEB_WEBWORKER_BUILD

    logic.simulation->setOnWorkersReadyCallback([this]() -> void
    {
        // leave the "WEB WORKERS LOADING" message for at least 1 second
        logic.state.countdown = 1000;
    });

#endif

    logic.simulation->setOnGenerationResetCallback([this]() -> void
    {
        { // handle the car trails

            auto& carsTrails = logic.carsTrails;
            const auto& simulation = *logic.simulation;
            const auto& genomes = simulation.getGenomes();
            unsigned int totalCars = simulation.getTotalCars();

            // reset trail map
            carsTrails.genomeIndexMap.clear();

            for (unsigned int ii = 0; ii < totalCars; ++ii)
            {
                // initialise trail map

                const auto& genome = genomes[ii];
                const auto& carData = simulation.getCarResult(ii);

                // record the trail index with it's genome id in the lookup map
                carsTrails.genomeIndexMap[genome.id] = ii;

                auto& currentTrail = carsTrails.allWheelsTrails[ii];

                // reset the old data

                for (unsigned int ii = 0; ii < currentTrail.wheels.size(); ++ii)
                    currentTrail.wheels[ii].clear();

                // initialise the new data

                for (unsigned int ii = 0; ii < carData.wheelsTransform.size(); ++ii)
                {
                    glm::vec4 wheelOrigin = carData.wheelsTransform[ii] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

                    currentTrail.wheels[ii].emplace_back(wheelOrigin);
                }
            }

        } // handle the car trails
    });

    logic.simulation->setOnGenerationStepCallback([this]() -> void
    {
        const auto& simulation = *logic.simulation;

        { // handle the car trails

            unsigned int totalCars = simulation.getTotalCars();

            for (unsigned int ii = 0; ii < totalCars; ++ii)
            {
                const auto& carData = simulation.getCarResult(ii);

                if (carData.isAlive == false)
                    continue;

                auto& currentWheelsTrail = logic.carsTrails.allWheelsTrails[ii];

                for (unsigned int ii = 0; ii < carData.wheelsTransform.size(); ++ii)
                {
                    glm::vec3 wheelOrigin = carData.wheelsTransform[ii] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

                    currentWheelsTrail.wheels[ii].emplace_back(wheelOrigin);
                }
            }

        } // handle the car trails

        { // handle the core data

            auto& cores = logic.cores;

            for (unsigned int ii = 0; ii < simulation.getTotalCores(); ++ii)
            {
                const auto& coreState = simulation.getCoreState(ii);

                cores.statesData[ii].delta = coreState.delta;
                cores.statesData[ii].genomesAlive = coreState.genomesAlive;

                // record core state history
                auto& currCoreHistory = cores.statesHistory[ii];
                currCoreHistory[cores.currHistoryIndex] = cores.statesData[ii];
            }

            // move to next core state history index
            cores.currHistoryIndex = (cores.currHistoryIndex + 1) % cores.maxStateHistory;

        } // handle the core data
    });

    logic.simulation->setOnGenomeDieCallback([this](unsigned int genomeIndex) -> void
    {
        const auto& simulation = *logic.simulation;

        const auto& carData = simulation.getCarResult(genomeIndex);

        const glm::vec3 extraHeight(0.0f, 0.0f, 1.0f);
        glm::vec4 carPos = carData.transform * glm::vec4(extraHeight, 1.0f);

        graphic.particleManager.emitParticles(carPos, carData.velocity);
        sounds.manager.playRandom(carPos);
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
             * ===> erase pop_back and push_back new one
             */

            const float prevLastOne = allStats[allStats.size() - 2];
            const float currLastOne = allStats[allStats.size() - 1];

            if (currLastOne > prevLastOne)
            {
                // last one is smarter -> erase first, push latest
                allStats.erase(allStats.begin());
                allStats.emplace_back(bestGenome.fitness);
            }
            else
            {
                // last one is not smarter -> replace last
                allStats.pop_back();
                allStats.emplace_back(bestGenome.fitness);
            }

        } // handle the stats

        { // handle the car trails

            if (isSmarter)
            {
                const auto& bestGenome = logic.simulation->getBestGenome();
                auto& carsTrails = logic.carsTrails;
                auto& currentTrailIndex = carsTrails.currentTrailIndex;

                auto it = carsTrails.genomeIndexMap.find(bestGenome.id);
                if (it != carsTrails.genomeIndexMap.end()) // <= this should never fail
                {
                    auto dataIndex = it->second;

                    const auto& bestWheelsTrailData = carsTrails.allWheelsTrails[dataIndex];
                    auto& bestNewCarsTrails = graphic.geometries.wireframes.bestNewCarsTrails;
                    auto& currCarNewTrail = bestNewCarsTrails[currentTrailIndex];

                    for (unsigned int ii = 0; ii < currCarNewTrail.wheels.size(); ++ii)
                    {
                        currCarNewTrail.wheels[ii].updateBuffer(0, bestWheelsTrailData.wheels[ii]);
                        currCarNewTrail.wheels[ii].setPrimitiveCount(bestWheelsTrailData.wheels[ii].size());
                    }

                    // increase the currently used trail index (loop if too high)
                    currentTrailIndex = (currentTrailIndex + 1) % bestNewCarsTrails.size();
                }
            }

        } // handle the car trails

        { // switch the current state

            StateManager::get()->changeState(StateManager::States::EndGeneration);

            logic.leaderCar.index = -1;

        } // switch the current state

    });
}

