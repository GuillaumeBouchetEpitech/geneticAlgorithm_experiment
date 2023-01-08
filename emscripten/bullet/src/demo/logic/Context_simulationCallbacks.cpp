
#include "Context.hpp"
#include "demo/defines.hpp"
#include "demo/states/StateManager.hpp"

#include <iomanip>
#include <sstream>

void
Context::initialiseSimulationCallbacks() {
  logic.simulation->setOnGenerationResetCallback([this]() -> void {
    { // handle the car trails

      const auto& simulation = *logic.simulation;
      const unsigned int totalCars = simulation.getTotalCars();

      logic.carWheelsTrails.reset(simulation);

      for (unsigned int ii = 0; ii < totalCars; ++ii) {
        const auto& carData = simulation.getCarResult(ii);

        // initialise the new data

        for (auto& transforms : carData.latestTransformsHistory) {
          for (std::size_t jj = 0; jj < transforms.wheels.size(); ++jj) {
            glm::vec3 wheelOrigin =
              transforms.wheels.at(jj) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

            logic.carWheelsTrails.push(ii, jj, wheelOrigin);
          }
        }
      }

    } // handle the car trails
  });

  logic.simulation->setOnGenerationStepCallback([this]() -> void {
    const auto& simulation = *logic.simulation;

    { // handle the car trails

      const unsigned int totalCars = simulation.getTotalCars();

      for (unsigned int ii = 0; ii < totalCars; ++ii) {
        const auto& carData = simulation.getCarResult(ii);

        if (carData.isAlive == false)
          continue;

        for (auto& transforms : carData.latestTransformsHistory) {
          for (std::size_t jj = 0; jj < transforms.wheels.size(); ++jj) {
            glm::vec3 wheelOrigin =
              transforms.wheels.at(jj) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

            logic.carWheelsTrails.push(ii, jj, wheelOrigin);
          }
        }
      }

    } // handle the car trails

    { // handle the core data

      auto& cores = logic.cores;

      cores.profileData.clearLatest();
      for (unsigned int coreIndex = 0; coreIndex < simulation.getTotalCores();
           ++coreIndex)
        cores.profileData.addToLatest(simulation.getCoreState(coreIndex));
      cores.profileData.pushLatest();

    } // handle the core data
  });

  logic.simulation->setOnGenomeDieCallback(
    [this](unsigned int genomeIndex) -> void {
      const auto& simulation = *logic.simulation;

      const auto& carData = simulation.getCarResult(genomeIndex);

      const glm::vec3 extraHeight(0.0f, 0.0f, 1.0f);
      glm::vec4 carPos =
        carData.liveTransforms.chassis * glm::vec4(extraHeight, 1.0f);

      graphic.scene.particleManager.emitParticles(carPos, carData.velocity);
    });

  logic.simulation->setOnGenerationEndCallback([this](bool isSmarter) -> void {
    logic.fitnessStats.update(logic.simulation->getBestGenome().fitness);

    if (isSmarter)
      graphic.scene.carTailsRenderer.updateLatestTrail();

    StateManager::get()->changeState(StateManager::States::EndGeneration);

    logic.leaderCar.reset();
  });
}
