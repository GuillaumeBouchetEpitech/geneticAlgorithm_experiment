
#include "demo/defines.hpp"

#include "Data.hpp"

#include "demo/states/StateManager.hpp"

#include <iomanip>
#include <sstream>

void	Data::initialiseStates()
{
#if defined D_WEB_WEBWORKER_BUILD

	logic.simulation->setOnWorkersReadyCallback([]() {

		StateManager::get()->changeState(StateManager::States::eRunning);
	});

#endif

	logic.simulation->setOnResetAndProcessCallback([this]() {

		auto&		carsTrails = logic.carsTrails;
		const auto&	simulation = *logic.simulation;
		const auto&	genomes = simulation.getGenomes();
		unsigned int totalCars = simulation.getTotalCars();

		// reset trail map

		carsTrails.lookupMap.clear();

		for (unsigned int ii = 0; ii < totalCars; ++ii)
		{
			// initialise trail map

			const auto& genome = genomes[ii];
			const auto& carData = simulation.getCarResult(ii);
			auto& currentTrail = carsTrails.allData[ii];

			// record the trail index with it's genome id in the lookup map
			carsTrails.lookupMap[genome.id] = ii;

			// reset the old data
			currentTrail.clear();

			// initialise the new data

			// this part elevate the origin of the car along it's up axis
			// => without it the origin is on the ground
			const glm::vec3	chassisHeight(0.0f, 0.0f, 1.0f);
			glm::mat4	carTransform = glm::translate(carData.transform, chassisHeight);
			glm::vec4	carOrigin = carTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

			currentTrail.push_back(carOrigin);
		}
	});



	logic.simulation->setOnProcessCallback([this]() {

		auto&		carsTrails = logic.carsTrails;
		const auto&	simulation = *logic.simulation;
		unsigned int totalCars = simulation.getTotalCars();

		for (unsigned int ii = 0; ii < totalCars; ++ii)
		{
			const auto&	carData = simulation.getCarResult(ii);

			if (!carData.isAlive)
				continue;

			auto& currentTrail = carsTrails.allData[ii];

			// this part elevate the origin of the car along it's up axis
			// => without it the origin is on the ground
			const glm::vec3	move(0.0f, 0.0f, 1.0f);
			glm::mat4	transform = glm::translate(carData.transform, move);
			glm::vec4	pos = transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

			// const auto&	output = carData.neuralNetworkOutput;

			currentTrail.push_back(pos);
		}
	});

	logic.simulation->setOnGenerationEndCallback([this](bool isSmarter) {

		if (isSmarter)
		{
			auto&		carsTrails = logic.carsTrails;
			const auto&	bestGenome = logic.simulation->getBestGenome();

			auto it = carsTrails.lookupMap.find(bestGenome.id);
			if (it == carsTrails.lookupMap.end())
				return;

			auto		dataIndex = it->second;
			const auto&	bestCarTrailData = carsTrails.allData[dataIndex];

			auto&	bestCarsTrails = graphic.geometries.monoColor.bestCarsTrails;
			auto&	currentIndex = carsTrails.currentIndex;
			auto&	currentCarTrail = bestCarsTrails[currentIndex];

			currentCarTrail.updateBuffer(0, bestCarTrailData);
			currentCarTrail.setPrimitiveCount(bestCarTrailData.size());

			currentIndex = (currentIndex + 1) % bestCarsTrails.size();
		}

		// {
		// 	unsigned int generationNumber = logic.simulation->getGenerationNumber();

		// 	logic.generationDatas.push_back({ generationNumber, bestGenome.fitness });
		// 	if (logic.generationDatas.size() > 5)
		// 		logic.generationDatas.erase(logic.generationDatas.begin());

		// 	std::stringstream sstr;

		// 	for (const auto& generationData : logic.generationDatas)
		// 	{
		// 		sstr
		// 			<< generationData.generationNumber
		// 			<< " fitness " << generationData.bestFitness
		// 			<< std::endl;
		// 	}

		// 	std::string str = sstr.str();

		// 	graphic.textRenderer.clear();
		// 	graphic.textRenderer.push({ 20, 580 }, str, 4.0f);
		// }

	});
}

