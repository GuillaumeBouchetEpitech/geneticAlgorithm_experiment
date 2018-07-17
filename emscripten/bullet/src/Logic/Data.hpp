

#pragma once


#include "OpenGLES.hpp"


#include "Graphic/BasicGeometry.hpp"
#include "Graphic/InstancedGeometry.hpp"
#include "Graphic/StackRenderer.hpp"

#include "Simulation/Simulation.hpp"


#include <string>
#include <list>
#include <map>


#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/mat4x4.hpp> // glm::mat4


class Shader;

class Data
{

	//
	//
	// singleton

private:
	static Data*	m_pInstance;

	Data();
	~Data();
private:
	void	initialise();

public:
	static void	create();
	static void	destroy();
	static Data*	get();

	// singleton
	//
	//

public:

	struct t_graphic
	{
		struct t_cameraData
		{
			glm::vec2		viewportSize = { 800.0f, 600.0f };

			glm::vec2	rotations = { 0.5f, 0.95f };
			glm::vec3	center = { 0.0f, 0.0f, 0.0f };

			glm::mat4	composedMatrix;
		}
		camera;

		struct t_shaders
		{
			Shader*	pBasic = nullptr;
			Shader*	pInstancing = nullptr;
		}
		shaders;

		struct t_geometries
		{
			struct t_instanced
			{
				InstancedGeometry	chassis;
				InstancedGeometry	wheels;
			}
			instanced;

			struct t_basic
			{
				BasicGeometry	circuitSkelton;
				BasicGeometry	circuitGround;
				BasicGeometry	circuitWalls;
			}
			basic;
		}
		geometries;

		StackRenderer	stackRenderer;
	}
	graphic;

	//

	struct t_logic
	{
		Simulation*	pSimulation = nullptr;

		struct t_leaderCarData
		{
			int	timeout = 0;
			int	index = -1;
		}
		leaderCar;

		struct t_carsTrails
		{
			std::map< unsigned int, unsigned int >	lookupMap;
			std::vector< std::vector<float> >		allData;
			std::list< std::vector<float> >			bestData;
		}
		carsTrails;
	}
	logic;

	struct t_input
	{
		std::map<int, bool>	keys;
	}
	input;
};
