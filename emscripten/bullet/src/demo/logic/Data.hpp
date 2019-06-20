
#pragma once

#include "thirdparty/OpenGLES.hpp"
#include "thirdparty/GLMath.hpp"

#include "graphic/StackRenderer.hpp"
#include "graphic/TextRenderer.hpp"

#include "graphic/wrappers/Geometry.hpp"
#include "graphic/wrappers/UniformBufferObject.hpp"
#include "graphic/wrappers/Texture.hpp"

#include "demo/simulation/AbstactSimulation.hpp"

#include <string>
#include <list>
#include <map>

enum e_uboTypes
{
	eMatrices = 0,
	eColors,
	eBlending,
	eAnimation,

	eCount
};

class Shader;

class Data
{

	//
	//
	// singleton

private:
	static Data* _instance;

	Data();
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
		UniformBufferObject	ubo;

		struct t_cameraData
		{
			glm::vec2	viewportSize = { 800.0f, 600.0f };

			glm::vec2	rotations = { 0.5f, 0.95f };
			glm::vec3	center = { 0.0f, 0.0f, 0.0f };
			float		distance = 0.0f;
		}
		camera;

		struct t_shaders
		{
			Shader*	basic = nullptr;
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

			struct t_basic
			{
				Geometry	stackRenderer;
			}
			basic;

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
		TextRenderer	textRenderer;
	}
	graphic;

	//

	struct t_logic
	{
		AbstactSimulation*	simulation = nullptr;

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
			std::map<unsigned int, unsigned int>	lookupMap;
			std::vector<std::vector<glm::vec3>>		allData;

			unsigned int	currentIndex = 0;
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
