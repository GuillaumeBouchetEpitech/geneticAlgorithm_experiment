
#pragma once

#include "thirdparty/OpenGLES.hpp"
#include "thirdparty/GLMath.hpp"

#include "graphic/utilities/StackRenderer.hpp"
#include "graphic/utilities/TextRenderer.hpp"
#include "graphic/wrappers/Geometry.hpp"
#include "graphic/wrappers/Texture.hpp"

#include "demo/simulation/AbstactSimulation.hpp"

#include <string>
#include <list>
#include <map>

class Shader;

class Data
{

	//
	//
	// singleton

private:
	static Data* _instance;

	Data() = default;
	~Data();
private:
	void	initialise();

public:
	static void		create();
	static void		destroy();
	static Data*	get();

	// singleton
	//
	//

private:
	void	initialiseShaders();
	void	initialiseGeometries();
	void	initialiseCircuit();
	void	initialiseStates();

public:

	struct t_graphic
	{
		struct t_cameraData
		{
			glm::vec2	viewportSize = { 800.0f, 600.0f };

			glm::vec2	rotations = { 0.5f, 0.95f };
			glm::vec3	center = { 0.0f, 0.0f, 0.0f };
			float		distance = 0.0f;

			glm::mat4	projectionMatrix;
			glm::mat4	modeviewMatrix;
			glm::mat4	sceneMatrix;
			glm::mat4	hudMatrix;
		}
		camera;

		struct t_shaders
		{
			Shader*	stackRenderer = nullptr;
			Shader*	instanced = nullptr;
			Shader*	monoColor = nullptr;
			Shader*	animatedCircuit = nullptr;
			Shader*	hudText = nullptr;
		}
		shaders;

		struct t_textures
		{
			Texture		textFont;
		}
		textures;

		struct t_geometries
		{
			struct t_instanced
			{
				Geometry	chassis;
				Geometry	wheels;
			}
			instanced;

			struct t_stackRenderer
			{
				Geometry	lines;
			}
			stackRenderer;

			struct t_monoColor
			{
				Geometry				circuitSkelton;
				std::vector<Geometry>	bestCarsTrails;
			}
			monoColor;

			struct t_animatedCircuit
			{
				Geometry	ground;
				Geometry	walls;
			}
			animatedCircuit;

			struct t_hudText
			{
				Geometry	letters;
			}
			hudText;
		}
		geometries;

		StackRenderer	stackRenderer;

		struct t_hudText
		{
			const glm::vec2 textureSize = { 256, 256 };
			const glm::vec2 gridSize = { 16, 16 };

			TextRenderer	renderer;
		}
		hudText;
	}
	graphic;

	//

	struct t_logic
	{
		struct t_metrics
		{
			unsigned int	updateTime = 0;
			unsigned int	renderTime = 0;
		}
		metrics;

		AbstactSimulation*	simulation = nullptr;

		struct t_cores
		{
			typedef std::vector<AbstactSimulation::t_coreState>	t_statesData;
			typedef std::vector<t_statesData>					t_statesHistory;

			const unsigned int	maxStateHistory = 60;
			unsigned int		currHistoryIndex = 0;
			t_statesData		statesData;
			t_statesHistory		statesHistory;

			unsigned int		genomesPerCore = 0;
			unsigned int		totalCores = 0;
			unsigned int		totalCars = 0;
		}
		cores;

		bool		isPaused = false;
		bool		isAccelerated = false;

		struct t_generationData
		{
			int generationNumber = -1;
			int bestFitness = -1;
		};

		std::vector<t_generationData>	generationDatas;
		t_generationData				bestGenerationDatas;

		struct t_leaderCarData
		{
			float	timeout = 0;
			int		index = -1;
		}
		leaderCar;

		struct t_carsTrails
		{
			std::map<unsigned int, unsigned int>	genomeIndexMap;
			std::vector<std::vector<glm::vec3>>		allTrailsData;

			unsigned int	currentTrailIndex = 0;
		}
		carsTrails;

		struct t_circuitAnimation
		{
			float	targetValue = 0.0f;
			float	lowerValue = 0.0f;
			float	upperValue = 0.0f;
			float	maxUpperValue = 0.0f;
			int		maxPrimitiveCount = 0;

			struct t_boundaries
			{
				glm::vec3	min;
				glm::vec3	max;
				glm::vec3	center;
			}
			boundaries;
		}
		circuitAnimation;

		struct t_hudText
		{
			std::string header;
			std::string pthreadWarning;
		}
		hudText;
	}
	logic;

	struct t_input
	{
		std::map<int, bool>	keys;

		struct t_mouse
		{
			glm::ivec2	position = {0, 0};
			glm::ivec2	delta = {0, 0};
			bool		tracking = false;
		}
		mouse;
	}
	inputs;
};
