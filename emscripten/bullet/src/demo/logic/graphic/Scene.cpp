
#include "Scene.hpp"

#include "demo/logic/Data.hpp"
#include "demo/logic/graphic/wrappers/Shader.hpp"

#include "demo/utilities/TraceLogger.hpp"

#include "thirdparty/GLMath.hpp"

#include <memory> // <= make_unique
#include <cstring> // <= std::memcpy

#include <iomanip>
#include <sstream>

void	Scene::renderSimple()
{
	auto& graphic = Data::get()->graphic;
	GLint baseAlignment = sizeof(float);
	int rawMatrixSize = baseAlignment * 16;

	Scene::setUniforms();

	Scene::clear();

	{ // scene

		graphic.ubo.bindRange(e_uboTypes::eMatrices, rawMatrixSize * 0, rawMatrixSize);

		Scene::renderCircuitSkeleton();
	}

	{ // HUD

		graphic.ubo.bindRange(e_uboTypes::eMatrices, rawMatrixSize * 1, rawMatrixSize);

		Scene::renderHUD();
	}

	Shader::unbind();
}

void	Scene::renderAll()
{
	auto& graphic = Data::get()->graphic;
	GLint baseAlignment = sizeof(float);
	int rawMatrixSize = baseAlignment * 16;

	Scene::setUniforms();

	Scene::clear();

	{ // scene

		graphic.ubo.bindRange(e_uboTypes::eMatrices, rawMatrixSize * 0, rawMatrixSize);

		if (!Data::get()->logic.isAccelerated)
			Scene::renderLeadingCarSensors();

		Scene::renderCars();
		Scene::renderCircuitSkeleton();
		Scene::renderBestCarsTrails();
		Scene::renderAnimatedCircuit();
	}

	{ // HUD

		graphic.ubo.bindRange(e_uboTypes::eMatrices, rawMatrixSize * 1, rawMatrixSize);

		Scene::renderHUD();
	}

	Shader::unbind();
}

void	Scene::setUniforms()
{
	const auto&	data = *Data::get();
	const auto&	animation = data.logic.circuitAnimation;
	const auto&	graphic = data.graphic;
	const auto&	camera = graphic.camera;
	const auto&	ubo = graphic.ubo;

	GLint offsetValue = UniformBufferObject::getOffsetAlignment();
	GLint baseAlignment = sizeof(float);

	//
	// matrices
	//

	{

		int rawMatrixSize = baseAlignment * 16;
		int rawBufSize = rawMatrixSize * 2;
		auto scopedBuffer = std::make_unique<char[]>(rawBufSize);

		char* rawBufferPtr = scopedBuffer.get();

		std::memset(rawBufferPtr, 0, rawBufSize);

		{
			const float fovy = glm::radians(70.0f);
			const float aspectRatio = float(camera.viewportSize.x) / camera.viewportSize.y;

			glm::mat4	projectionMatrix = glm::perspective(fovy, aspectRatio, 1.0f, 1000.f);
			glm::mat4	viewMatrix = glm::mat4(1.0f); // <= identity matrix
			glm::mat4	modelMatrix = glm::mat4(1.0f); // <= identity matrix

			viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -camera.distance));
			viewMatrix = glm::rotate(viewMatrix, camera.rotations.y, glm::vec3(-1.0f, 0.0f, 0.0f));
			viewMatrix = glm::rotate(viewMatrix, camera.rotations.x, glm::vec3(0.0f, 0.0f, 1.0f));

			modelMatrix = glm::translate(modelMatrix, camera.center);

			glm::mat4 sceneMatrix = projectionMatrix * viewMatrix * modelMatrix;

			std::memcpy(rawBufferPtr + 0 * rawMatrixSize, glm::value_ptr(sceneMatrix), rawMatrixSize);
		}

		{
			const auto& vSize = camera.viewportSize;
			glm::mat4	projectionMatrix = glm::ortho(0.0f, vSize.x, 0.0f, vSize.y, -10.0f, 10.0f);

			glm::vec3	eye = { 0, 0, 1 };
			glm::vec3	center = { 0, 0, 0 };
			glm::vec3	upAxis = { 0, 1, 0 };
			glm::mat4	viewMatrix = glm::lookAt(eye, center, upAxis);

			glm::mat4 hudMatrix = projectionMatrix * viewMatrix;

			std::memcpy(rawBufferPtr + 1 * rawMatrixSize, glm::value_ptr(hudMatrix), rawMatrixSize);
		}

		ubo.updateBuffer(e_uboTypes::eMatrices, rawBufferPtr, rawBufSize, true);
	}

	//
	// animation
	//

	{
		int rawBufSize = offsetValue * baseAlignment;
		auto scopedBuffer = std::make_unique<char[]>(rawBufSize);

		char* rawBufferPtr = scopedBuffer.get();

		std::memset(rawBufferPtr, 0, rawBufSize);

		std::memcpy(rawBufferPtr + 0 * baseAlignment, &animation.lowerValue, sizeof(animation.lowerValue));
		std::memcpy(rawBufferPtr + 1 * baseAlignment, &animation.upperValue, sizeof(animation.upperValue));

		ubo.updateBuffer(e_uboTypes::eAnimation, rawBufferPtr, rawBufSize, true);
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
	const auto&	leaderCar = data.logic.leaderCar;
	const auto&	simulation = data.logic.simulation;
	auto&		stackRenderer = data.graphic.stackRenderer;
	const auto&	shader = *data.graphic.shaders.basic;

	// valid leading car?
	if (leaderCar.index < 0)
		return;

	const auto& carData = simulation->getCarResult(leaderCar.index);

	// leading car alive?
	if (!carData.isAlive)
		return;

	shader.bind();

	const glm::vec3	greenColor(0.0f, 1.0f, 0.0f);
	const glm::vec3	redColor(1.0f, 0.0f, 0.0f);

	for (const auto& sensor : carData.eyeSensors)
	{
		stackRenderer.pushLine(sensor.near, sensor.far, greenColor);
		stackRenderer.pushCross(sensor.far, greenColor, 1.0f);
	}

	const auto& groundSensor = carData.groundSensor;
	stackRenderer.pushLine(groundSensor.near, groundSensor.far, redColor);
	stackRenderer.pushCross(groundSensor.far, redColor, 1.0f);

	stackRenderer.flush();
}

void	Scene::renderCars()
{
	// instanced geometrie(s)

	const auto&	graphic = Data::get()->graphic;
	const auto&	shader = *graphic.shaders.instanced;
	const auto&	instanced = graphic.geometries.instanced;
	const auto&	ubo = graphic.ubo;

	GLint offsetValue = UniformBufferObject::getOffsetAlignment();
	GLint baseAlignment = sizeof(float);
	int bufPageSize = offsetValue * baseAlignment;

	shader.bind();

	ubo.bindRange(e_uboTypes::eColors, bufPageSize * 0, bufPageSize);

	instanced.chassis.render();

	ubo.bindRange(e_uboTypes::eColors, bufPageSize * 1, bufPageSize);

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
	const auto&	ubo = graphic.ubo;

	GLint offsetValue = UniformBufferObject::getOffsetAlignment();
	GLint baseAlignment = sizeof(float);
	int bufPageSize = offsetValue * baseAlignment;

	shader.bind();

	if (circuit)
	{
		ubo.bindRange(e_uboTypes::eColors, bufPageSize * 2, bufPageSize);

		monoColor.circuitSkelton.render();
	}

	if (trails)
	{
		ubo.bindRange(e_uboTypes::eColors, bufPageSize * 3, bufPageSize);

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
	const auto&	ubo = graphic.ubo;

	GLint offsetValue = UniformBufferObject::getOffsetAlignment();
	int bufPageSize = offsetValue;

	shader.bind();

	ubo.bindRange(e_uboTypes::eBlending, 0, bufPageSize);

	animatedCircuit.ground.render();

	glDisable(GL_DEPTH_TEST); // <= prevent "blending artifact"

	ubo.bindRange(e_uboTypes::eBlending, bufPageSize, bufPageSize);

	animatedCircuit.walls.render();

	glEnable(GL_DEPTH_TEST);
}

void	Scene::renderHUD()
{
	auto&		data = *Data::get();
	auto&		graphic = data.graphic;
	const auto&	shader = *graphic.shaders.hudText;
	const auto&	simulation = data.logic.simulation;
	const auto&	hudText = data.logic.hudText;

	shader.bind();

	glDisable(GL_DEPTH_TEST); // <= not useful for a HUD
	glEnable(GL_TEXTURE_2D);

	graphic.textures.textFont.bind();

	graphic.textRenderer.clear();
	graphic.textRenderer.push({ 8, 600 - 16 - 8 }, hudText.header, 1.0f);

	if (!hudText.pthreadWarning.empty())
		graphic.textRenderer.push({ 800 - 19 * 16, 2 * 16 }, hudText.pthreadWarning, 1.0f);

	{
		const unsigned int	totalCars = simulation->getTotalCars();
		unsigned int		carsLeft = 0;
		float				localBestFitness = 0.0f;
		for (unsigned int ii = 0; ii < totalCars; ++ii)
		{
			const auto& carData = simulation->getCarResult(ii);

			if (carData.isAlive)
				++carsLeft;

			if (localBestFitness < carData.fitness)
				localBestFitness = carData.fitness;
		}

		std::stringstream sstr;

		sstr
			<< "Generation: " << simulation->getGenerationNumber() << std::endl
			<< "Cars: " << std::setw(2) << carsLeft << "/" << totalCars << std::endl
			<< "fitness:" << std::endl
			<< "->current:" << std::setw(3) << localBestFitness << std::endl
			<< "->best:   " << std::setw(3) << simulation->getBestGenome().fitness;

		std::string str = sstr.str();

		graphic.textRenderer.push({ 8, 8 + 4 * 16 }, str, 1.0f);
	}

	if (data.logic.isPaused)
		graphic.textRenderer.push({ 400 - 3 * 16 * 5, 300 - 8 * 5 }, "PAUSED", 5.0f);

	graphic.textRenderer.render();

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
}
