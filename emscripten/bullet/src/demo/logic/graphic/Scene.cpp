
#include "Scene.hpp"

#include "demo/logic/Data.hpp"
#include "demo/logic/graphic/wrappers/Shader.hpp"

#include "demo/utilities/TraceLogger.hpp"

#include "./utilities/sceneToScreen.hpp"


#include "thirdparty/GLMath.hpp"

#include <memory> // <= make_unique
#include <cstring> // <= std::memcpy

#include <iomanip>
#include <sstream>

void writeTime(std::stringstream& sstr, unsigned int time, bool isMS)
{
	if (isMS)
	{
		if (time < 1000)
		{
			sstr << time << "ms";
		}
		else
		{
			sstr
				<< std::fixed << std::setprecision(1)
				<< (float(time) / 1000) << "s ";
		}
	}
	else
	{
		if (time < 1000)
		{
			sstr << time << "us";
		}
		else if (time < 1000000)
		{
			sstr
				<< std::fixed << std::setprecision(1)
				<< (float(time) / 1000) << "ms";
		}
		else
		{
			sstr
				<< std::fixed << std::setprecision(1)
				<< (float(time) / 1000000) << "s";
		}
	}
}


void	Scene::renderSimple()
{
	Scene::updateMatrices();

	Scene::clear();

	{ // scene

		Scene::renderCircuitSkeleton();
	}

	{ // HUD

		Scene::renderHUD();
	}

	Shader::unbind();
}

void	Scene::renderAll()
{
	Scene::updateMatrices();

	Scene::clear();

	{ // scene

		if (!Data::get()->logic.isAccelerated)
			Scene::renderLeadingCarSensors();

		Scene::renderCars();
		Scene::renderCircuitSkeleton();
		Scene::renderBestCarsTrails();
		Scene::renderAnimatedCircuit();
	}

	{ // HUD

		Scene::renderHUD();
	}

	Shader::unbind();
}

void	Scene::updateMatrices()
{
	auto&	camera = Data::get()->graphic.camera;

	{ // scene

		const float fovy = glm::radians(70.0f);
		const float aspectRatio = float(camera.viewportSize.x) / camera.viewportSize.y;

		glm::mat4	projectionMatrix = glm::perspective(fovy, aspectRatio, 1.0f, 1000.f);
		glm::mat4	viewMatrix = glm::mat4(1.0f); // <= identity matrix
		glm::mat4	modelMatrix = glm::mat4(1.0f); // <= identity matrix

		viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -camera.distance));
		viewMatrix = glm::rotate(viewMatrix, camera.rotations.y, glm::vec3(-1.0f, 0.0f, 0.0f));
		viewMatrix = glm::rotate(viewMatrix, camera.rotations.x, glm::vec3(0.0f, 0.0f, 1.0f));

		modelMatrix = glm::translate(modelMatrix, camera.center);

		camera.projectionMatrix = projectionMatrix;
		camera.modeviewMatrix = viewMatrix * modelMatrix;

		camera.sceneMatrix = projectionMatrix * viewMatrix * modelMatrix;
	}

	{ // hud

		const auto& vSize = camera.viewportSize;
		glm::mat4	projectionMatrix = glm::ortho(0.0f, vSize.x, 0.0f, vSize.y, -1.0f, 1.0f);

		glm::vec3	eye = { 0.0f, 0.0f, 0.5f };
		glm::vec3	center = { 0.0f, 0.0f, 0.0f };
		glm::vec3	upAxis = { 0.0f, 1.0f, 0.0f };
		glm::mat4	viewMatrix = glm::lookAt(eye, center, upAxis);

		camera.hudMatrix = projectionMatrix * viewMatrix;
	}
}

void	Scene::clear()
{
	const auto& viewportSize = Data::get()->graphic.camera.viewportSize;

	glViewport(0, 0, viewportSize.x, viewportSize.y);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void	Scene::renderLeadingCarSensors()
{
	auto&		data = *Data::get();
	const auto&	logic = data.logic;
	const auto&	leaderCar = logic.leaderCar;
	const auto&	simulation = *logic.simulation;
	auto&		graphic = data.graphic;
	auto&		stackRenderer = graphic.stackRenderer;
	const auto&	shader = *graphic.shaders.stackRenderer;

	// valid leading car?
	if (leaderCar.index < 0)
		return;

	const auto& carData = simulation.getCarResult(leaderCar.index);

	// leading car alive?
	if (!carData.isAlive)
		return;

	shader.bind();

	const auto&	sceneMatrix = graphic.camera.sceneMatrix;
	GLint composedMatrixLocation = shader.getUniform("u_composedMatrix");
	glUniformMatrix4fv(composedMatrixLocation, 1, false, glm::value_ptr(sceneMatrix));


	// float revLife = 1.0f - carData.life;
	float revLife = 0.0f;

	const glm::vec3	greenColor(revLife, 1.0f, revLife);
	const glm::vec3	orangeColor(1.0f, 0.5f, 0.0f);

	for (const auto& sensor : carData.eyeSensors)
	{
		stackRenderer.pushLine(sensor.near, sensor.far, greenColor);
		stackRenderer.pushCross(sensor.far, greenColor, 1.0f);
	}

	const auto& groundSensor = carData.groundSensor;
	stackRenderer.pushLine(groundSensor.near, groundSensor.far, orangeColor);
	stackRenderer.pushCross(groundSensor.far, orangeColor, 1.0f);

	stackRenderer.flush();
}

void	Scene::renderCars()
{
	// instanced geometrie(s)

	const auto&	graphic = Data::get()->graphic;
	const auto&	shader = *graphic.shaders.instanced;
	const auto&	instanced = graphic.geometries.instanced;

	shader.bind();

	const auto&	sceneMatrix = graphic.camera.sceneMatrix;
	GLint composedMatrixLocation = shader.getUniform("u_composedMatrix");
	glUniformMatrix4fv(composedMatrixLocation, 1, false, glm::value_ptr(sceneMatrix));

	// GLint colorLocation = shader.getUniform("u_color");

	// glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);

	instanced.chassis.render();

	// glUniform4f(colorLocation, 1.0f, 1.0f, 0.0f, 1.0f);

	instanced.wheels.render();
}

void	Scene::renderCircuitSkeleton()
{
	Scene::renderMonoColorGeometries(true, false);
}

void	Scene::renderBestCarsTrails()
{
	Scene::renderMonoColorGeometries(false, true);
}

void	Scene::renderMonoColorGeometries(bool circuit /*= true*/, bool trails /*= true*/)
{
	// static geometrie(s) (mono color)

	if (!circuit && !trails)
		return;

	const auto&	graphic = Data::get()->graphic;
	const auto&	shader = *graphic.shaders.monoColor;
	const auto&	monoColor = graphic.geometries.monoColor;

	shader.bind();

	const auto&	sceneMatrix = graphic.camera.sceneMatrix;
	GLint composedMatrixLocation = shader.getUniform("u_composedMatrix");
	glUniformMatrix4fv(composedMatrixLocation, 1, false, glm::value_ptr(sceneMatrix));

	GLint colorLocation = shader.getUniform("u_color");

	if (circuit)
	{
		glUniform4f(colorLocation, 0.6f, 0.6f, 0.6f, 1.0f);

		monoColor.circuitSkelton.render();
	}

	if (trails)
	{
		glUniform4f(colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);

		for (const auto& geometry : monoColor.bestCarsTrails)
			geometry.render();
	}
}

void	Scene::renderAnimatedCircuit()
{
	// static geometrie(s) (animated)

	const auto&	graphic = Data::get()->graphic;
	const auto&	shader = *graphic.shaders.animatedCircuit;
	const auto&	animatedCircuit = graphic.geometries.animatedCircuit;

	shader.bind();

	const auto&	sceneMatrix = graphic.camera.sceneMatrix;
	GLint composedMatrixLocation = shader.getUniform("u_composedMatrix");
	glUniformMatrix4fv(composedMatrixLocation, 1, false, glm::value_ptr(sceneMatrix));

	const auto&	animation = Data::get()->logic.circuitAnimation;
	GLint lowerLimitLocation = shader.getUniform("u_lowerLimit");
	GLint upperLimitLocation = shader.getUniform("u_upperLimit");
	glUniform1f(lowerLimitLocation, animation.lowerValue);
	glUniform1f(upperLimitLocation, animation.upperValue);

	GLint alphaLocation = shader.getUniform("u_alpha");
	glUniform1f(alphaLocation, 0.8f);

	animatedCircuit.ground.render();

	glDisable(GL_DEPTH_TEST); // <= prevent "blending artifact"

	glUniform1f(alphaLocation, 0.2f);

	animatedCircuit.walls.render();

	glEnable(GL_DEPTH_TEST);
}

void	Scene::renderHUD()
{
	auto&		data = *Data::get();
	auto&		graphic = data.graphic;
	auto&		logic = data.logic;

	glDisable(GL_DEPTH_TEST); // <= not useful for a HUD

	{
		const auto&	shader = *graphic.shaders.hudText;
		auto&		textRenderer = graphic.hudText.renderer;
		const auto&	simulation = *logic.simulation;
		const auto&	hudText = logic.hudText;

		shader.bind();

		const auto&	hudMatrix = graphic.camera.hudMatrix;
		GLint composedMatrixLocation = shader.getUniform("u_composedMatrix");
		glUniformMatrix4fv(composedMatrixLocation, 1, false, glm::value_ptr(hudMatrix));

		graphic.textures.textFont.bind();

		textRenderer.clear();
		textRenderer.push({ 8, 600 - 16 - 8 }, hudText.header, 1.0f);

		if (!hudText.pthreadWarning.empty())
			textRenderer.push({ 800 - 19 * 16, 2 * 16 }, hudText.pthreadWarning, 1.0f);

		{
			std::stringstream sstr;

#if defined D_WEB_WEBWORKER_BUILD
			bool isMillisecond = true;
#else
			bool isMillisecond = false;
#endif

			sstr << "update: ";
			writeTime(sstr, logic.metrics.updateTime, isMillisecond);
			sstr << std::endl << "render: ";
			writeTime(sstr, logic.metrics.renderTime, isMillisecond);


			std::string str = sstr.str();

			textRenderer.push({ 8, 600 - 5 * 16 - 8 }, str, 1.0f);
		}

		{
			const unsigned int	totalCars = logic.cores.totalCars;
			unsigned int		carsLeft = 0;
			float				localBestFitness = 0.0f;
			for (unsigned int ii = 0; ii < totalCars; ++ii)
			{
				const auto& carData = simulation.getCarResult(ii);

				if (carData.isAlive)
					++carsLeft;

				if (localBestFitness < carData.fitness)
					localBestFitness = carData.fitness;
			}

			std::stringstream sstr;

			sstr
				<< "Generation: " << simulation.getGenerationNumber() << std::endl
				<< "Fitness: " << localBestFitness << "/" << simulation.getBestGenome().fitness << std::endl
				<< "Cars: " << carsLeft << "/" << totalCars;

			std::string str = sstr.str();

			textRenderer.push({ 8, 8 + 2 * 16 }, str, 1.0f);
		}

		{
			/**
			{
				const auto& camera = graphic.camera;

				glm::vec3 screenCoord;

				const unsigned int	totalCars = logic.cores.totalCars;
				for (unsigned int ii = 0; ii < totalCars; ++ii)
				{
					const auto& carData = simulation.getCarResult(ii);

					if (!carData.isAlive)
						continue;

					const glm::vec3 pos = carData.transform * glm::vec4(0,0,0,1);

					sceneToScreen(
						pos,
						camera.modeviewMatrix, camera.projectionMatrix,
						glm::vec2(0,0), glm::vec2(800, 600),
						screenCoord
					);

					if (screenCoord.z > 1.0f)
						continue;

					std::stringstream sstr;

					sstr
						<< "id" << ii << std::endl
						<< carData.life << std::endl
						<< carData.fitness;

					std::string str = sstr.str();

					textRenderer.push({ screenCoord.x, screenCoord.y }, str, 0.75f);

					// if (localBestFitness < carData.fitness)
					// 	localBestFitness = carData.fitness;
				}
			}

			// const auto& camera = graphic.camera;

			// glm::vec3 screenCoord;

			// bool result = sceneToScreen(
			// 	glm::vec3(0, 0, 0),
			// 	camera.modeviewMatrix, camera.projectionMatrix,
			// 	glm::vec2(0,0), glm::vec2(800, 600),
			// 	screenCoord
			// );

			// if (screenCoord.z < 1.0f)
			// {
			// 	std::stringstream sstr;

			// 	sstr
			// 		<< "result: " << result << std::endl
			// 		<< "coord:" << std::endl
			// 		<< screenCoord.x << std::endl
			// 		<< screenCoord.y << std::endl
			// 		<< screenCoord.z;

			// 	std::string str = sstr.str();

			// 	textRenderer.push({ screenCoord.x, screenCoord.y }, str, 1.0f);
			// }
			//*/
		}

		/**
		{
			std::stringstream sstr;

			for (unsigned int ii = 0; ii < logic.cores.statesData.size(); ++ii)
			{
				const auto& coreState = logic.cores.statesData[ii];

				sstr << "worker: " << ii;

				for (int jj = 0; jj < 6; ++jj)
					sstr << std::endl;

#if defined D_WEB_WEBWORKER_BUILD
				// sstr << std::setw(3) << coreState.delta << "ms";
				if (coreState.delta < 1000)
				{
					sstr
						<< std::setw(4)
						<< coreState.delta << "ms";
				}
				else
				{
					sstr
						<< std::fixed << std::setprecision(1)
						<< std::setw(4)
						<< (float(coreState.delta) / 1000) << "s ";
				}
#else
				if (coreState.delta < 1000)
				{
					sstr
						<< std::setw(4)
						<< coreState.delta << "us";
				}
				else if (coreState.delta < 1000000)
				{
					sstr
						<< std::fixed << std::setprecision(1)
						<< std::setw(4)
						<< (float(coreState.delta) / 1000) << "ms";
				}
				else
				{
					sstr
						<< std::fixed << std::setprecision(1)
						<< std::setw(4)
						<< (float(coreState.delta) / 1000000) << "s";
				}
#endif

				sstr
					<< " "
					<< std::setw(2)
					<< coreState.genomesAlive << "car(s)"
					<< std::endl
					<< std::endl;
			}

			std::string str = sstr.str();

			// textRenderer.push({ 8, 8 + 15 * 16 }, str, 1.0f);
			textRenderer.push({ 16, 8 + 30 * 16 }, str, 1.0f);
		}
		//*/

		{
			std::stringstream sstr;

			for (unsigned int ii = 0; ii < logic.cores.statesData.size(); ++ii)
			{
				const auto& coreState = logic.cores.statesData[ii];

#if defined D_WEB_WEBWORKER_BUILD

				sstr << "WORKER_" << (ii + 1) << std::endl;

				sstr << ">time: ";
				writeTime(sstr, coreState.delta, true);

				// // sstr << std::setw(3) << coreState.delta << "ms";
				// if (coreState.delta < 1000)
				// {
				// 	sstr << coreState.delta << "ms";
				// }
				// else
				// {
				// 	sstr
				// 		<< std::fixed << std::setprecision(1)
				// 		<< (float(coreState.delta) / 1000) << "s ";
				// }
#else
				sstr << "THREAD_" << (ii + 1) << std::endl;

				sstr << ">time: ";
				writeTime(sstr, coreState.delta, false);

				// if (coreState.delta < 1000)
				// {
				// 	sstr
				// 		<< coreState.delta << "us";
				// }
				// else if (coreState.delta < 1000000)
				// {
				// 	sstr
				// 		<< std::fixed << std::setprecision(1)
				// 		<< (float(coreState.delta) / 1000) << "ms";
				// }
				// else
				// {
				// 	sstr
				// 		<< std::fixed << std::setprecision(1)
				// 		<< (float(coreState.delta) / 1000000) << "s";
				// }
#endif

				sstr
					<< std::endl
					<< ">cars: " << coreState.genomesAlive << std::endl
					<< std::endl;
			}

			std::string str = sstr.str();

			textRenderer.push({ 8, 8 + 15 * 16 }, str, 1.0f);
		}

		if (logic.isPaused)
			textRenderer.push({ 400 - 3 * 16 * 5, 300 - 8 * 5 }, "PAUSED", 5.0f);

		textRenderer.render();
	}

	/**
	{
		auto&		stackRenderer = graphic.stackRenderer;
		const auto&	shader = *graphic.shaders.stackRenderer;

		shader.bind();

		const auto&	hudMatrix = graphic.camera.hudMatrix;
		GLint composedMatrixLocation = shader.getUniform("u_composedMatrix");
		glUniformMatrix4fv(composedMatrixLocation, 1, false, glm::value_ptr(hudMatrix));

		const glm::vec3	whiteColor(1.0f, 1.0f, 1.0f);
		const glm::vec3	redColor(0.75f, 0.0f, 0.0f);
		const glm::vec3	greenColor(0.0f, 0.75f, 0.0f);

		glm::vec2	borderPos(20, 410);
		glm::vec2	borderSize(150, 75);

		const auto& cores = data.logic.cores;

		for (unsigned core = 0; core < cores.statesHistory.size(); ++core)
		{
			const auto& stateHistory = cores.statesHistory[core];

			unsigned int maxDelta = 0;
			for (unsigned int ii = 0; ii < stateHistory.size(); ++ii)
				maxDelta = std::max(maxDelta, stateHistory[ii].delta);

			//
			//

			const glm::vec2	currPos = {
				borderPos.x,
				borderPos.y - core * (borderSize.y + 52)
			};

			stackRenderer.pushRectangle(currPos, borderSize, whiteColor);

			//
			//

			float widthStep = borderSize.x / cores.maxStateHistory;

			for (unsigned int ii = 0; ii + 1 < cores.maxStateHistory; ++ii)
			{
				unsigned int prevIndex = (ii + cores.currHistoryIndex) % cores.maxStateHistory;
				unsigned int currIndex = (ii + 1 + cores.currHistoryIndex) % cores.maxStateHistory;

				const auto& prevState = stateHistory[prevIndex];
				const auto& currState = stateHistory[currIndex];

				//

				float prevRatio = float(prevState.delta) / maxDelta;
				float currRatio = float(currState.delta) / maxDelta;

				// maxDelta
				glm::vec2	prevCoord(ii * widthStep, borderSize.y * prevRatio);
				glm::vec2	currCoord((ii + 1) * widthStep, borderSize.y * currRatio);
				glm::vec3	color = glm::mix(greenColor, redColor, currRatio);

				stackRenderer.pushLine(currPos + prevCoord, currPos + currCoord, color);
			}
		}

        // unsigned int maxGenomesAlive = cores.genomesPerCore;

		// for (unsigned int ii = 0; ii < cores.maxStateHistory; ++ii)
		// {
		// 	unsigned int index = (ii + cores.currHistoryIndex) % cores.maxStateHistory;

		// 	const auto& stateHistory = cores.statesHistory[index];

		// 	stateHistory;

		// }

		// const unsigned int	maxStateHistory = 60;
		// unsigned int		currHistoryIndex = 0;
		// t_statesData		statesData;
		// t_statesHistory		statesHistory;

		stackRenderer.flush();
	}
	//*/

	glEnable(GL_DEPTH_TEST);
}
