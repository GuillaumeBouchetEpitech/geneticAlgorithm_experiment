
#include "demo/defines.hpp"

#include "State_Running.hpp"

#include "StateManager.hpp"

#include "demo/logic/Data.hpp"
#include "demo/logic/graphic/wrappers/Shader.hpp"
#include "demo/logic/graphic/Scene.hpp"

// #include "demo/utilities/TraceLogger.hpp"

#include "thirdparty/GLMath.hpp"

#include <limits> // std::numeric_limits<T>::max();
#include <cmath> // std::ceil

void	State_Running::enter()
{
	// D_MYLOG("step");
}

void	State_Running::leave()
{
	// D_MYLOG("step");
}

//

void	State_Running::handleEvent(const SDL_Event& event)
{
	auto&	data = *Data::get();
	auto&	keys = data.inputs.keys;
	auto&	mouse = data.inputs.mouse;

	switch (event.type)
	{
		case SDL_KEYDOWN:
		{
			keys[event.key.keysym.sym] = true;
			break;
		}
		case SDL_KEYUP:
		{
			keys[event.key.keysym.sym] = false;
			break;
		}

		case SDL_MOUSEBUTTONDOWN:
		{
			mouse.position = { event.motion.x, event.motion.y };
			mouse.delta = { 0, 0 };
			mouse.tracking = true;
			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			mouse.tracking = false;
			break;
		}
		case SDL_MOUSEMOTION:
		{
			if (mouse.tracking)
			{
				glm::ivec2 newPosition = { event.motion.x, event.motion.y };
				mouse.delta = newPosition - mouse.position;
				mouse.position = newPosition;
			}
			break;
		}

		default:
			break;
	}
}

void	State_Running::update(int deltaTime)
{
	float elapsedTime = float(deltaTime) / 1000.0f;

	auto&	data = *Data::get();
	auto&	logic = data.logic;
	auto&	simulation = *logic.simulation;
	auto&	graphic = data.graphic;
	auto&	camera = graphic.camera;

	{ // events

		{ // mouse/touch event(s)

			auto& mouse = data.inputs.mouse;

			if (mouse.tracking)
			{
				camera.rotations.x += float(mouse.delta.x) * 0.5f * elapsedTime;
				camera.rotations.y -= float(mouse.delta.y) * 0.5f * elapsedTime;
				mouse.delta = { 0, 0 };
			}
		}

		{ // keyboard event(s)

			auto& keys = data.inputs.keys;

			bool rotateLeft = (
				keys[SDLK_LEFT] ||	// ARROW
				keys[SDLK_q] ||		// QWERTY (UK-US keyboard layout)
				keys[SDLK_a]		// AZERTY (FR keyboard layout)
			);

			bool rotateRight = (
				keys[SDLK_RIGHT] ||	// ARROW
				keys[SDLK_d]
			);

			bool rotateUp = (
				keys[SDLK_UP] ||	// ARROW
				keys[SDLK_w] ||		// QWERTY (UK-US keyboard layout)
				keys[SDLK_z]		// AZERTY (FR keyboard layout)
			);

			bool rotateDown = (
				keys[SDLK_DOWN] ||	// ARROW
				keys[SDLK_s]
			);

			if (rotateLeft)
				camera.rotations.x -= 2.0f * elapsedTime;
			else if (rotateRight)
				camera.rotations.x += 2.0f * elapsedTime;

			if (rotateUp)
				camera.rotations.y += 1.0f * elapsedTime;
			else if (rotateDown)
				camera.rotations.y -= 1.0f * elapsedTime;

#if not defined D_WEB_WEBWORKER_BUILD

			logic.isAccelerated = (keys[SDLK_SPACE]); // spacebar

#endif
		}
	}

#if not defined D_WEB_WEBWORKER_BUILD
	{
		int	simualtionStep = (logic.isAccelerated ? 50 : 1);

		for (unsigned int ii = 0; ii < simulation.getTotalCores(); ++ii)
			logic.cores.statesData[ii].delta = 0;

		for (int ii = 0; ii < simualtionStep; ++ii)
			simulation.update();
	}
#endif

	{ // camera

		glm::vec3	cameraNextCenter = { 0.0f, 0.0f, 0.0f };
		float		cameraNextDistance = 30.0f;

		//
		//

		auto&	leaderCar = logic.leaderCar;

		if (logic.isAccelerated)
		{
			leaderCar.timeout = 0.0f;
			leaderCar.index = -1;

			cameraNextCenter = logic.circuitAnimation.boundaries.center;
			cameraNextDistance = 200.0f;
		}
		else
		{
			cameraNextDistance = 30.0f;

			leaderCar.timeout -= elapsedTime;

			// timeout of the camera focus on the currently best car
			if (leaderCar.timeout <= 0.0f)
			{
				// reset the timeout
				leaderCar.timeout = 0.5f; // <= half a second

				// refresh the currently best car

				unsigned int totalCars = simulation.getTotalCars();

				leaderCar.index = -1;
				int	bestGroundIndex = -1;
				for (unsigned int ii = 0; ii < totalCars; ++ii)
				{
					const auto& carData = simulation.getCarResult(ii);

					if (!carData.isAlive || bestGroundIndex > carData.groundIndex)
						continue;

					bestGroundIndex = carData.groundIndex;
					leaderCar.index = ii;
				}
			}
		}

		// do we have a car to focus the camera on?
		if (leaderCar.index >= 0)
		{
			const auto& carResult = simulation.getCarResult(leaderCar.index);

			// this part elevate where the camera look along the up axis of the car
			// => without it the camera look at the ground
			// => mostly useful for a shoulder camera (TODO)
			glm::vec3	sensorsHeight(0.0f, 0.0f, 2.0f);
			glm::mat4	carTransform = glm::translate(carResult.transform, sensorsHeight);
			glm::vec4	carOrigin = carTransform * glm::vec4(0.0f, 0.0f, 0.0f, -1.0f);

			cameraNextCenter = glm::vec3(carOrigin);
		}

		//
		//

		const float	lerpRatio = 0.1f;

		camera.center -= (camera.center - cameraNextCenter) * lerpRatio;
		camera.distance -= (camera.distance - cameraNextDistance) * lerpRatio;

	} // camera

	{ // circuit animation

		auto&	leaderCar = logic.leaderCar;
		auto&	animation = logic.circuitAnimation;

		if (logic.isAccelerated)
		{
			animation.targetValue = animation.maxUpperValue;
			animation.lowerValue = animation.maxUpperValue;
			animation.upperValue = animation.maxUpperValue;
		}
		else
		{
			animation.targetValue = 3.0f; // <= default value

			// do we have a car to focus the camera on?
			if (leaderCar.index >= 0)
			{
				const auto& car = simulation.getCarResult(leaderCar.index);

				animation.targetValue += car.groundIndex;
			}

			if (animation.lowerValue < animation.targetValue)
			{
				animation.lowerValue += 0.1f; // rise slowly
				if (animation.lowerValue > animation.targetValue)
					animation.lowerValue = animation.targetValue;
			}
			else if (animation.lowerValue > animation.targetValue + 10.0f)
			{
				animation.lowerValue -= 1.5f; // fall really quickly
				if (animation.lowerValue < animation.targetValue)
					animation.lowerValue = animation.targetValue;
			}
			else if (animation.lowerValue > animation.targetValue)
			{
				animation.lowerValue -= 0.3f; // fall quickly
				if (animation.lowerValue < animation.targetValue)
					animation.lowerValue = animation.targetValue;
			}

			if (animation.upperValue < animation.targetValue)
			{
				animation.upperValue += 0.3f; // rise quickly
				if (animation.upperValue > animation.targetValue)
					animation.upperValue = animation.targetValue;
			}
			else if (animation.upperValue > animation.targetValue + 10.0f)
			{
				animation.upperValue -= 1.0f; // fall really quickly
				if (animation.upperValue < animation.targetValue)
					animation.upperValue = animation.targetValue;
			}
			else if (animation.upperValue > animation.targetValue)
			{
				animation.upperValue -= 0.1f; // fall slowly
				if (animation.upperValue < animation.targetValue)
					animation.upperValue = animation.targetValue;
			}
		}

		auto& animatedCircuit = graphic.geometries.animatedCircuit;

		// <= 3 * 12 triangles
		int indexValue = std::ceil(animation.upperValue) * 36;
		if (indexValue > animation.maxPrimitiveCount)
			indexValue = animation.maxPrimitiveCount;

		animatedCircuit.ground.setPrimitiveCount(indexValue);
		animatedCircuit.walls.setPrimitiveCount(indexValue * 2); // <= 2 walls

	} // circuit animation

	{ // update instancing matrices

		unsigned int totalCars = simulation.getTotalCars();

		struct t_attributes
		{
			glm::mat4	tranform;
			glm::vec4	color;
		};

		std::vector<t_attributes> chassisMatrices, wheelsMatrices;
		chassisMatrices.reserve(totalCars);
		wheelsMatrices.reserve(totalCars * 4);

		glm::vec3	chassisHeight(0.0f, 0.0f, 1.0f);

		for (unsigned int ii = 0; ii < totalCars; ++ii)
		{
			const auto& carData = simulation.getCarResult(ii);

			if (!carData.isAlive)
				continue;

			// const float colorMax = 0.85f;
			const float colorMax = 1.0f;
			// float revLife = (1.0f - carData.life) * colorMax;
			float revLife = 0.0f;
			glm::vec4	chassisColor(colorMax, revLife, revLife, 1.0f);
			glm::vec4	wheelsColor(colorMax, colorMax, revLife, 1.0f);

			glm::mat4	carTransform = glm::translate(carData.transform, chassisHeight);

			chassisMatrices.push_back({ carTransform, chassisColor });

			for (const auto& wheelTransform : carData.wheelsTransform)
				wheelsMatrices.push_back({ wheelTransform, wheelsColor });
		}

		auto&	instanced = graphic.geometries.instanced;

		instanced.chassis.updateBuffer(1, chassisMatrices);
		instanced.wheels.updateBuffer(1, wheelsMatrices);
		instanced.chassis.setInstancedCount(chassisMatrices.size());
		instanced.wheels.setInstancedCount(wheelsMatrices.size());
	}

#if defined D_WEB_WEBWORKER_BUILD

	simulation.update();

#endif
}


void	State_Running::render(const SDL_Window& window)
{
	static_cast<void>(window); // <= unused

	Scene::renderAll();
}

void	State_Running::resize(int width, int height)
{
	Data::get()->graphic.camera.viewportSize = { width, height };
}

void	State_Running::visibility(bool visible)
{
	if (!visible)
		StateManager::get()->changeState(StateManager::States::ePaused);
}
