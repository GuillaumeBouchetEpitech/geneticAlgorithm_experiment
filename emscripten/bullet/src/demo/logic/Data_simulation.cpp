
#include "demo/defines.hpp"

#include "Data.hpp"

#include "demo/states/StateManager.hpp"

#include <iomanip>
#include <sstream>

void	Data::initialiseSimulation()
{
	logic.simulation->setOnGenerationResetCallback([this]() {

		auto&		carsTrails = logic.carsTrails;
		const auto&	simulation = *logic.simulation;
		const auto&	genomes = simulation.getGenomes();
		unsigned int totalCars = simulation.getTotalCars();

		// reset trail map

		carsTrails.genomeIndexMap.clear();

		for (unsigned int ii = 0; ii < totalCars; ++ii)
		{
			// initialise trail map

			const auto& genome = genomes[ii];
			const auto& carData = simulation.getCarResult(ii);
			auto& currentTrail = carsTrails.allTrailsData[ii];

			// record the trail index with it's genome id in the lookup map
			carsTrails.genomeIndexMap[genome.id] = ii;

			currentTrail.isAlive = true;

			// reset the old data
			currentTrail.trail.clear();

			// initialise the new data

			// this part elevate the origin of the car along it's up axis
			// => without it the origin is on the ground
			const glm::vec3	chassisHeight(0.0f, 0.0f, 1.0f);
			glm::mat4	carTransform = glm::translate(carData.transform, chassisHeight);
			glm::vec4	carOrigin = carTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

			currentTrail.trail.push_back(carOrigin);
		}
	});

	logic.simulation->setOnGenerationStepCallback([this]() {

		auto&		carsTrails = logic.carsTrails;
		const auto&	simulation = *logic.simulation;
		unsigned int totalCars = simulation.getTotalCars();

		const glm::vec3	extraHeight(0.0f, 0.0f, 1.0f);

		for (unsigned int ii = 0; ii < totalCars; ++ii)
		{
			const auto&	carData = simulation.getCarResult(ii);

			auto& currentTrail = carsTrails.allTrailsData[ii];

			if (carData.isAlive == false)
			{
				if (currentTrail.isAlive == true)
				{
					currentTrail.isAlive = false;

					glm::mat4 transform = glm::translate(carData.transform, extraHeight);
					glm::vec4 pos = transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

					graphic.particleManager.emitParticles({ pos.x, pos.y, pos.z});
				}

				continue;
			}

			// this part elevate the origin of the car along it's up axis
			// => without it the origin is on the ground
			glm::mat4 transform = glm::translate(carData.transform, extraHeight);
			glm::vec4 pos = transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

			currentTrail.trail.push_back(pos);
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

	logic.simulation->setOnGenerationEndCallback([this](bool isSmarter) {

		if (!isSmarter)
			return;

		auto&		carsTrails = logic.carsTrails;
		const auto&	bestGenome = logic.simulation->getBestGenome();

		auto it = carsTrails.genomeIndexMap.find(bestGenome.id);
		if (it == carsTrails.genomeIndexMap.end())
			return;

		auto		dataIndex = it->second;
		const auto&	bestCarTrailData = carsTrails.allTrailsData[dataIndex];

		auto&	bestCarsTrails = graphic.geometries.wireframes.bestCarsTrails;
		auto&	currentTrailIndex = carsTrails.currentTrailIndex;
		auto&	currentCarTrail = bestCarsTrails[currentTrailIndex];

		currentCarTrail.updateBuffer(0, bestCarTrailData.trail);
		currentCarTrail.setPrimitiveCount(bestCarTrailData.trail.size());

		currentTrailIndex = (currentTrailIndex + 1) % bestCarsTrails.size();
	});
}

