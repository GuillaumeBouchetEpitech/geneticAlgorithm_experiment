

#include "State_Main.hpp"

#include "StateManager.hpp"

#include "Logic/Data.hpp"
#include "Logic/Graphic/Shader.hpp"

#include "Utility/TraceLogger.hpp"


#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate/rotate/scale/perspective
#include <glm/gtc/type_ptr.hpp> // glm:value_ptr


State_Main::State_Main()
{
	auto&	logic = Data::get()->logic;

	logic.pSimulation->setOnResetAndProcessCallback([&logic]() {

		auto&	carsTrails = logic.carsTrails;
		auto&&	genomes = logic.pSimulation->getGenomes();

		// reset trail map

		carsTrails.lookupMap.clear();

		for (int ii = 0; ii < D_CARS_NUMBER_TOTAL; ++ii)
		{
			// initialise trail map

			auto&& genome = genomes[ii];
			carsTrails.lookupMap[genome.id] = ii;

			auto& currentTrail = carsTrails.allData[ii];

			// reset trail data

			currentTrail.clear();

			// initialise trail data

			auto&& carResult = logic.pSimulation->getCarResult(ii);

			glm::mat4	chassisMat4 = glm::make_mat4(carResult.chassisMat4);
			chassisMat4 = glm::translate(chassisMat4, glm::vec3(0.0f, 0.0f, 1.0f));
			glm::vec4	carOrigin = chassisMat4 * glm::vec4(0, 0, 0, 1);

			currentTrail.push_back(carOrigin.x);
			currentTrail.push_back(carOrigin.y);
			currentTrail.push_back(carOrigin.z);
		}
	});

	logic.pSimulation->setOnProcessCallback([&logic]() {

		auto&	carsTrails = logic.carsTrails;

		for (int ii = 0; ii < D_CARS_NUMBER_TOTAL; ++ii)
		{
			auto&&	carResult = logic.pSimulation->getCarResult(ii);

			if (!carResult.isAlive)
				continue;

			auto&	currentTrail = carsTrails.allData[ii];

			glm::mat4	chassisMat4 = glm::make_mat4(carResult.chassisMat4);
			chassisMat4 = glm::translate(chassisMat4, glm::vec3(0.0f, 0.0f, 1.0f));
			glm::vec4	carOrigin = chassisMat4 * glm::vec4(0, 0, 0, 1);

			currentTrail.push_back(carOrigin.x);
			currentTrail.push_back(carOrigin.y);
			currentTrail.push_back(carOrigin.z);
		}
	});

	logic.pSimulation->setOnSmarterCallback([&logic]() {

		auto&	carsTrails = logic.carsTrails;
		auto&&	bestGenome = logic.pSimulation->getBestGenome();

		auto	itMap = carsTrails.lookupMap.find(bestGenome.id);
		if (itMap == carsTrails.lookupMap.end())
			return;

		auto&& bestCarTrailData = carsTrails.allData[itMap->second];

		// save latest best trail
		carsTrails.bestData.push_back(bestCarTrailData);

		// trim the best cars trails container
		while (carsTrails.bestData.size() > 5)
			carsTrails.bestData.pop_front();
	});
}

State_Main::~State_Main()
{
}

//

void	State_Main::handleEvent(const SDL_Event& event)
{
	auto&	keys = Data::get()->input.keys;

    switch (event.type)
    {
    case SDL_KEYDOWN:
        keys[event.key.keysym.sym] = true;
        break;

    case SDL_KEYUP:
        keys[event.key.keysym.sym] = false;
        break;

    default:
        break;
    }
}

void	State_Main::update(int delta)
{
	float elapsed_time = static_cast<float>(delta) / 1000;

    auto&	logic = Data::get()->logic;
    auto&	pSimulation = logic.pSimulation;
	auto&	graphic = Data::get()->graphic;
	auto&	camera = graphic.camera;

	{ // events

		auto&	keys = Data::get()->input.keys;

		bool	moveLeft = (
			keys[SDLK_LEFT] || // ARROW
			keys[SDLK_q] || // QWERTY
			keys[SDLK_a] // AZERTY (FR keyboard layout)
		);

		bool	moveRight = (
			keys[SDLK_RIGHT] || // ARROW
			keys[SDLK_d]
		);

		bool	moveUp = (
			keys[SDLK_UP] || // ARROW
			keys[SDLK_w] || // QWERTY
			keys[SDLK_z] // AZERTY (FR keyboard layout)
		);

		bool	moveDown = (
			keys[SDLK_DOWN] || // ARROW
			keys[SDLK_s]
		);

		if (moveLeft)
			camera.rotations.x -= 2.0f * elapsed_time;
		else if (moveRight)
			camera.rotations.x += 2.0f * elapsed_time;

		if (moveUp)
			camera.rotations.y += 1.0f * elapsed_time;
		else if (moveDown)
			camera.rotations.y -= 1.0f * elapsed_time;
	}

	{ // camera

		glm::vec3	cameraNextCenter = { 0.0f, 0.0f, 0.0f };

		//
		//

		auto&	leaderCar = logic.leaderCar;

		leaderCar.timeout -= delta;

		if (leaderCar.timeout <= 0)
		{
			leaderCar.timeout = 500;

	        float	curr_fitness = -1;

	        for (int ii = 0; ii < D_CARS_NUMBER_TOTAL; ++ii)
	        {
				auto&& carResult = pSimulation->getCarResult(ii);

	            if (!carResult.isAlive)
	                continue;

	            if (curr_fitness > carResult.fitness)
	                continue;

	            curr_fitness = carResult.fitness;

				leaderCar.index = ii;

				glm::mat4	tmp_mat = glm::make_mat4(carResult.chassisMat4);
				glm::vec4	tmp_pos = tmp_mat * glm::vec4(0.0f,0.0f,-2.0f,-1.0f);

				cameraNextCenter = glm::vec3(tmp_pos);
	        }
		}
		else if (leaderCar.index >= 0)
		{
			auto&& carResult = pSimulation->getCarResult(leaderCar.index);

			glm::mat4	tmp_mat = glm::make_mat4(carResult.chassisMat4);

			glm::vec4	tmp_pos = tmp_mat * glm::vec4(0.0f,0.0f,-2.0f,-1.0f);

			cameraNextCenter = glm::vec3(tmp_pos);
		}

        //
        //

        const float	lerpRatio = 0.10;

        camera.center.x -= (camera.center.x - cameraNextCenter.x) * lerpRatio;
        camera.center.y -= (camera.center.y - cameraNextCenter.y) * lerpRatio;
        camera.center.z -= (camera.center.z - cameraNextCenter.z) * lerpRatio;

		//
		//

		const float fovy = glm::radians(70.0f);
		const float aspectRatio = float(camera.viewportSize.x) / camera.viewportSize.y;
		const float distance = 50.0f;

		glm::mat4	projectionMatrix = glm::perspective(fovy, aspectRatio, 1.0f, 1000.f);
		glm::mat4	viewMatrix = glm::mat4(1.0f); // <= identity matrix
		glm::mat4	modelMatrix = glm::mat4(1.0f); // <= identity matrix

		viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -distance));
		viewMatrix = glm::rotate(viewMatrix, camera.rotations.y, glm::vec3(-1.0f, 0.0f, 0.0f));
		viewMatrix = glm::rotate(viewMatrix, camera.rotations.x, glm::vec3(0.0f, 0.0f, 1.0f));

		viewMatrix = glm::translate(viewMatrix, camera.center);

		camera.composedMatrix = projectionMatrix * viewMatrix * modelMatrix;

		//
		//

		graphic.stackRenderer.setMatrix(glm::value_ptr(camera.composedMatrix));

	} // camera

	pSimulation->update();
}


void	State_Main::render(const SDL_Window& window)
{
	auto&	graphic = Data::get()->graphic;
	auto&	camera = graphic.camera;
	auto&	stackRenderer = graphic.stackRenderer;
	auto&&	logic = Data::get()->logic;
	auto&&	pSimulation = logic.pSimulation;
	float*	pComposedMatrix = glm::value_ptr(camera.composedMatrix);

	{
		glViewport(0, 0, camera.viewportSize.x, camera.viewportSize.y);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	{ // update instancing matrices

		std::vector<float>	chassisMatrices;
		std::vector<float>	wheelsMatrices;
		chassisMatrices.reserve(16 * D_CARS_NUMBER_TOTAL);
		wheelsMatrices.reserve(16 * 4 * D_CARS_NUMBER_TOTAL);

		for (unsigned int ii = 0; ii < D_CARS_NUMBER_TOTAL; ++ii)
		{
			auto&& carResult = pSimulation->getCarResult(ii);

			if (!carResult.isAlive)
				continue;

			{
				glm::mat4	chassisMat4 = glm::make_mat4(carResult.chassisMat4);

				chassisMat4 = glm::translate(chassisMat4, glm::vec3(0.0f, 0.0f, 1.0f));

				float*	pMatrix = glm::value_ptr(chassisMat4);

				for (int jj = 0; jj < 16; ++jj)
					chassisMatrices.push_back(pMatrix[jj]);
			}

			{
				for (int jj = 0; jj < 4; ++jj)
					for (int kk = 0; kk < 16; ++kk)
						wheelsMatrices.push_back(carResult.wheelsMat4[jj][kk]);
			}
		}

		auto&	instanced = graphic.geometries.instanced;

		instanced.chassis.updateMatrix(chassisMatrices.data(), chassisMatrices.size());
		instanced.wheels.updateMatrix(wheelsMatrices.data(), wheelsMatrices.size());
	}

	{ // render raycast

		StackRenderer::t_color	eyeSensorsColor(0, 1, 0, 1.0);
		StackRenderer::t_color	groundSensorColor(1, 0, 0, 1.0);

		auto&&	leaderCar = logic.leaderCar;

		if (leaderCar.index >= 0)
		{
			auto&& carResult = pSimulation->getCarResult(leaderCar.index);

			if (carResult.isAlive)
			{
				for (int ii = 0; ii < 15; ++ii)
				{
					const float* near = carResult.sensors[ii].near;
					const float* far = carResult.sensors[ii].far;

					StackRenderer::t_pos	nearPos(near[0], near[1], near[2]);
					StackRenderer::t_pos	farPos(far[0], far[1], far[2]);
					stackRenderer.push_vertex(nearPos, eyeSensorsColor);
					stackRenderer.push_vertex(farPos, eyeSensorsColor);
				}

				{
					const float* near = carResult.groundSensor.near;
					const float* far = carResult.groundSensor.far;

					StackRenderer::t_pos	nearPos(near[0], near[1], near[2]);
					StackRenderer::t_pos	farPos(far[0], far[1], far[2]);
					stackRenderer.push_vertex(nearPos, groundSensorColor);
					stackRenderer.push_vertex(farPos, groundSensorColor);
				}
			}
		}

		stackRenderer.flush();
	}

	{ // trails

		auto&&	carsTrails = logic.carsTrails;

		StackRenderer::t_color	trailsColor(1, 1, 1, 1.0);

		for (auto&& vertices : carsTrails.bestData)
		{
			for (unsigned int ii = 3; ii < vertices.size(); ii += 3)
			{
				const float*	prevVertex = &vertices[ii - 3];
				const float*	currVertex = &vertices[ii];

				StackRenderer::t_pos	previousPos(prevVertex[0], prevVertex[1], prevVertex[2]);
				StackRenderer::t_pos	currentPos(currVertex[0], currVertex[1], currVertex[2]);

				stackRenderer.push_vertex(previousPos, trailsColor);
				stackRenderer.push_vertex(currentPos, trailsColor);
			}
		}
		stackRenderer.flush();
	}

	{ // instanced geometries

		auto&&	pShader = graphic.shaders.pInstancing;
		auto&&	instanced = graphic.geometries.instanced;

		Shader::bind(pShader);

			instanced.chassis.render(GL_LINES, pComposedMatrix);
			instanced.wheels.render(GL_LINES, pComposedMatrix);

		Shader::bind(nullptr);
	}

	{ // static gemetries

		auto&&	pShader = graphic.shaders.pBasic;
		auto&&	basic = graphic.geometries.basic;

		Shader::bind(pShader);

			basic.circuitSkelton.render(GL_LINES, pComposedMatrix);
			basic.circuitGround.render(GL_TRIANGLES, pComposedMatrix, 0.8f);
			basic.circuitWalls.render(GL_TRIANGLES, pComposedMatrix, 0.2f);

		Shader::bind(nullptr);
	}
}

void	State_Main::resize(int width, int height)
{
	Data::get()->graphic.camera.viewportSize = { width, height };
}
