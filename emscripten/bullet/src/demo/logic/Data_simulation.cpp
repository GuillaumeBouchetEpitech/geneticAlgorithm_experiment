
#include "demo/defines.hpp"

#include "Data.hpp"

#include "demo/states/StateManager.hpp"

#include <iomanip>
#include <sstream>

void Data::initialiseSimulation()
{
    logic.fitnessStats.allStats.reserve(logic.fitnessStats.maxStats);
    for (unsigned int ii = 0; ii < logic.fitnessStats.maxStats; ++ii)
        logic.fitnessStats.allStats.push_back(0.0f);

#if defined D_WEB_WEBWORKER_BUILD

    logic.simulation->setOnWorkersReadyCallback([this]() {

        logic.state.countdown = 1000;
    });

#endif

    logic.simulation->setOnGenerationResetCallback([this]() {

        StateManager::get()->changeState(StateManager::States::eStartGeneration);

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

            {
                // auto& currentTrail = carsTrails.allTrails[ii];

                // // reset the old data
                // currentTrail.clear();

                // // initialise the new data

                // // this part elevate the origin of the car along it's up axis
                // // => without it the origin is on the ground
                // const glm::vec3 chassisHeight(0.0f, 0.0f, 1.0f);
                // glm::mat4 carTransform = glm::translate(carData.transform, chassisHeight);
                // glm::vec4 carOrigin = carTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

                // currentTrail.push_back(carOrigin);
            }

            {
                auto& currentTrail = logic.carsTrails.allWheelsTrails[ii];

                // reset the old data

                for (unsigned int ii = 0; ii < currentTrail.wheels.size(); ++ii)
                    currentTrail.wheels[ii].clear();

                // initialise the new data

                for (unsigned int ii = 0; ii < carData.wheelsTransform.size(); ++ii)
                {
                    glm::vec4 wheelOrigin = carData.wheelsTransform[ii] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

                    currentTrail.wheels[ii].push_back(wheelOrigin);
                }
            }
        }
    });

    logic.simulation->setOnGenerationStepCallback([this]() {

        const auto& simulation = *logic.simulation;
        unsigned int totalCars = simulation.getTotalCars();

        const glm::vec3 extraHeight(0.0f, 0.0f, 1.0f);

        for (unsigned int ii = 0; ii < totalCars; ++ii)
        {
            const auto& carData = simulation.getCarResult(ii);

            if (carData.isAlive == false)
                continue;

            {
                // // this part elevate the origin of the car along it's up axis
                // // => without it the origin is on the ground
                // glm::vec4 carPos = carData.transform * glm::vec4(extraHeight, 1.0f);

                // auto& currentTrail = logic.carsTrails.allTrails[ii];

                // currentTrail.push_back(carPos);
            }

            {
                auto& currentWheelsTrail = logic.carsTrails.allWheelsTrails[ii];

                for (unsigned int ii = 0; ii < carData.wheelsTransform.size(); ++ii)
                {
                    glm::vec3 wheelOrigin = carData.wheelsTransform[ii] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

                    currentWheelsTrail.wheels[ii].push_back(wheelOrigin);
                }
            }
        }

        auto& cores = logic.cores;

        for (unsigned int ii = 0; ii < simulation.getTotalCores(); ++ii)
        {
            const auto& coreState = simulation.getCoreState(ii);

            if (!logic.isAccelerated)
                cores.statesData[ii].delta = 0;

            cores.statesData[ii].delta += coreState.delta;
            cores.statesData[ii].genomesAlive = coreState.genomesAlive;

            // record core state history
            auto& currCoreHistory = cores.statesHistory[ii];
            currCoreHistory[cores.currHistoryIndex] = cores.statesData[ii];
        }

        // move to next core state history index
        cores.currHistoryIndex = (cores.currHistoryIndex + 1) % cores.maxStateHistory;
    });

    logic.simulation->setOnGenomeDieCallback([this](unsigned int genomeIndex) {

        const auto& simulation = *logic.simulation;

        const auto& carData = simulation.getCarResult(genomeIndex);

        const glm::vec3 extraHeight(0.0f, 0.0f, 1.0f);
        glm::vec4 carPos = carData.transform * glm::vec4(extraHeight, 1.0f);

        graphic.particleManager.emitParticles(carPos);
        sounds.soundManager.playRandom(carPos);
    });

    logic.simulation->setOnGenerationEndCallback([this](bool isSmarter) {

        {
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

            if (currLastOne > prevLastOne) // last one is smarter
            {
                if (allStats.size() == fitnessStats.maxStats)
                    allStats.erase(allStats.begin() + 0); // erase fisrt
            }
            else // last one is not smarter
            {
                allStats.pop_back(); // erase last
            }

            allStats.push_back(bestGenome.fitness);
        }

        if (!isSmarter)
            return;

        const auto& bestGenome = logic.simulation->getBestGenome();
        auto& carsTrails = logic.carsTrails;
        auto& currentTrailIndex = carsTrails.currentTrailIndex;

        auto it = carsTrails.genomeIndexMap.find(bestGenome.id);
        if (it == carsTrails.genomeIndexMap.end())
            return;

        auto dataIndex = it->second;

        {
            // const auto& bestCarTrailData = carsTrails.allTrails[dataIndex];
            // auto& bestCarsTrails = graphic.geometries.wireframes.bestCarsTrails;
            // auto& currentCarTrail = bestCarsTrails[currentTrailIndex];

            // currentCarTrail.updateBuffer(0, bestCarTrailData);
            // currentCarTrail.setPrimitiveCount(bestCarTrailData.size());
        }

        {
            const auto& bestWheelsTrailData = carsTrails.allWheelsTrails[dataIndex];
            auto& bestNewCarsTrails = graphic.geometries.wireframes.bestNewCarsTrails;
            auto& currCarNewTrail = bestNewCarsTrails[currentTrailIndex];

            for (unsigned int ii = 0; ii < currCarNewTrail.wheels.size(); ++ii)
            {
                currCarNewTrail.wheels[ii].updateBuffer(0, bestWheelsTrailData.wheels[ii]);
                currCarNewTrail.wheels[ii].setPrimitiveCount(bestWheelsTrailData.wheels[ii].size());
            }
        }

        currentTrailIndex = (currentTrailIndex + 1) % 5; // <= hardcoded
    });
}

