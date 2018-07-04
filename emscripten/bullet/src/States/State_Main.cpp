

#include "State_Main.hpp"

#include "StateManager.hpp"

#include "Logic/Data.hpp"
#include "Logic/Graphic/Shader.hpp"

#include "Utility/TraceLogger.hpp"


// #include <chrono>
#include <ctime>

#include <iostream>


State_Main::State_Main()
{
}

State_Main::~State_Main()
{
}

//

void	State_Main::handleEvent(const SDL_Event& event)
{
	auto&	keys = Data::get()->m_input.keys;

    switch (event.type)
    {
    case SDL_KEYDOWN:
        keys[event.key.keysym.sym] = true;
        break;

    case SDL_KEYUP:
        keys[event.key.keysym.sym] = false;
        break;

	// case SDL_MOUSEBUTTONDOWN:
	// 	{
	// 		int tmp_X, tmp_Y;
	// 		SDL_GetMouseState(&tmp_X, &tmp_Y);
	// 		D_MYLOG("click, x=" << tmp_X << ", y=" << tmp_Y);

	// 		if (tmp_X > 100 && tmp_X < 200 &&
	// 			tmp_Y > 400 && tmp_Y < 500)
	// 		{
	// 			D_MYLOG("TOUCHED!");

	// 			Data::get()->m_pSimulation->getBestCar().reset();

	// 			StateManager::get()->changeState(StateManager::e_States::eRunBest);
	// 		}
	// 	}
	//     break;

    default:
        break;
    }
}






#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm:value_ptr


namespace
{
	glm::mat4 camera(const glm::vec3& in_Pos, float in_distance, const glm::vec2& in_Rotate)
	// glm::mat4 camera(const glm::vec3& in_Pos, float in_distance, const glm::vec2& in_Rotate, glm::mat4& proj, glm::mat4& view)
	{
		glm::mat4 Projection = glm::perspective(glm::radians(70.0f), 4.0f / 3.0f, 1.0f, 1000.f);


		glm::mat4 View = glm::mat4(1.0f);

		View = glm::translate(View, glm::vec3(0.0f, 0.0f, -in_distance));
		View = glm::rotate(View, in_Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
		View = glm::rotate(View, in_Rotate.x, glm::vec3(0.0f, 0.0f, 1.0f));

		View = glm::translate(View, in_Pos);

		// proj = Projection;
		// view = View;

		// glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
		glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
		return Projection * View * Model;
	}
};




void	State_Main::update(int delta)
{
	float elapsed_time = static_cast<float>(delta) / 1000;

	auto&	rotations = Data::get()->m_vec2_rotations;

	{ // events

		auto&	keys = Data::get()->m_input.keys;

		Data::get()->m_simualtion_step = (keys[SDLK_SPACE] ? 60 : 1);
		// Data::get()->m_simualtion_step = (keys[SDLK_SPACE] ? 1 : 60);

		if (keys[SDLK_LEFT] ||
			keys[SDLK_a] ||
			keys[SDLK_q])
			rotations.x -= 2.0f * elapsed_time;
		else if (keys[SDLK_RIGHT] ||
				 keys[SDLK_d])
			rotations.x += 2.0f * elapsed_time;

		if (keys[SDLK_UP] ||
			keys[SDLK_w] ||
			keys[SDLK_z])
			rotations.y += 1.0f * elapsed_time;
		else if (keys[SDLK_DOWN] ||
				 keys[SDLK_s])
			rotations.y -= 1.0f * elapsed_time;
	}

	{ // camera

		glm::vec3	camera_pos = {0.0f, 0.0f, 0.0f};

		//
		//

		auto&	index_targetedCar = Data::get()->m_index_targetedCar;

		auto&	timeout_camera = Data::get()->m_timeout_camera;
		timeout_camera -= delta;

		if (Data::get()->m_simualtion_step == 1)
		{
			if (timeout_camera <= 0)
			{
				timeout_camera = 500;

		        float	curr_fitness = -1;

		        float	mat4x4[16];

		        auto& Cars = Data::get()->m_pSimulation->getCars();

		        for (auto& car : Cars)
		        {
		            if (!car.isAlive())
		                continue;

		            if (curr_fitness > car.getFitness())
		                continue;

		            curr_fitness = car.getFitness();

					index_targetedCar = car.getIndex();

		        	Data::get()->m_pPhysicWrapper->vehicle_getOpenGLMatrix(index_targetedCar, mat4x4);
					glm::mat4	tmp_mat = glm::make_mat4((float*)mat4x4);
					glm::vec4	tmp_pos = tmp_mat * glm::vec4(0.0f,0.0f,-2.0f,-1.0f);
					camera_pos.x = tmp_pos.x;
					camera_pos.y = tmp_pos.y;
					camera_pos.z = tmp_pos.z;

					// camera_pos.x = mat4x4[12];
					// camera_pos.y = mat4x4[13];
					// camera_pos.z = mat4x4[14];
		        }
			}
			else if (index_targetedCar >= 0)
			{
		        float	mat4x4[16];

				Data::get()->m_pPhysicWrapper->vehicle_getOpenGLMatrix(index_targetedCar, mat4x4);
				glm::mat4	tmp_mat = glm::make_mat4((float*)mat4x4);
				glm::vec4	tmp_pos = tmp_mat * glm::vec4(0.0f,0.0f,-2.0f,-1.0f);
				camera_pos.x = tmp_pos.x;
				camera_pos.y = tmp_pos.y;
				camera_pos.z = tmp_pos.z;

				// camera_pos.x = mat4x4[12];
				// camera_pos.y = mat4x4[13];
				// camera_pos.z = mat4x4[14];
			}
			else
			{
				camera_pos.x = camera_pos.y = camera_pos.z = 0;
			}
		}
		else
		{
			camera_pos.x = camera_pos.y = camera_pos.z = 0;
		}

        //
        //

        auto&	center = Data::get()->m_vec3_center;
        auto&	previous_center = Data::get()->m_vec3_previous_center;

        previous_center = center; // <= to determine the lookAt of the second camera

        // glm::vec3	diff(camera_pos.x-center.x, camera_pos.y-center.y, camera_pos.z-center.z);
        glm::vec3	diff(center.x-camera_pos.x, center.y-camera_pos.y, center.z-camera_pos.z);

        float	lerp_ratio = 0.10;

        center.x -= diff.x * lerp_ratio;
        center.y -= diff.y * lerp_ratio;
        center.z -= diff.z * lerp_ratio;

		//
		//

		Data::get()->m_composedMatrix = camera(center, 50, rotations);
		// Data::get()->m_composedMatrix = camera(center, 50, rotations, Data::get()->m_graphic.m_proj, Data::get()->m_graphic.m_view);

		Data::get()->m_StackRenderer.setMatrix( glm::value_ptr(Data::get()->m_composedMatrix) );

	} // camera



	// for (int i = 0; i < Data::get()->m_simualtion_step; ++i)
	// 	Data::get()->m_pSimulation->update(1.0f / 60); // <= MUST be constant

	auto& myProducer = Data::get()->m_Producer;
	auto& myRunning = Data::get()->m_running;

	myProducer.update();

	if (myRunning == 0)
	{
		myRunning = 3;

		Data::get()->m_pSimulation->evolve();

		for (int world_index = 0; world_index < 3; ++world_index)
		{
			// D_MYLOG("world_index=" << world_index);

			myProducer.push([world_index]() {

				// D_MYLOG("world_index=" << world_index);

				for (int jj = 0; jj < Data::get()->m_simualtion_step; ++jj)
					Data::get()->m_pSimulation->update(world_index, 1.0f / 60); // <= MUST be constant

			}, [&]() {

				--myRunning;
			});
		}

		// while (myRunning > 0)
		// 	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}



}


void	State_Main::render(const SDL_Window& window)
{
	{
		auto&	window_size = Data::get()->m_graphic.window_size;

		glViewport(0, 0, window_size.x, window_size.y);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	{ // update instacing matrices

		auto&& arr_Cars = Data::get()->m_pSimulation->getCars();

		auto&	PhysicWorld = Data::get()->m_PhysicWorld;

		std::vector<float>	buffer_chassis;
		buffer_chassis.reserve(16*90);
		std::vector<float>	buffer_wheel;
		buffer_wheel.reserve(16*90*4);

		btScalar	mat4[16];

		for (unsigned int ii = 0; ii < arr_Cars.size(); ++ii)
		{
			if (!arr_Cars[ii].isAlive())
				continue;

			{
				Data::get()->m_pPhysicWrapper->vehicle_getOpenGLMatrix(ii, mat4);

				glm::mat4	tmp_mat = glm::make_mat4((float*)mat4);

				tmp_mat = glm::translate(tmp_mat, glm::vec3(0.0f, 0.0f, 1.0f));

				float*	pMatrix = glm::value_ptr(tmp_mat);

				for (int ii = 0; ii < 16; ++ii)
					buffer_chassis.push_back(pMatrix[ii]);
			}

			{
				for (int jj = 0; jj < Data::get()->m_pPhysicWrapper->vehicle_getWheelsNumber(ii); ++jj)
				{
					Data::get()->m_pPhysicWrapper->vehicle_getWheelsMatrix(ii, jj, mat4);

					for (int ii = 0; ii < 16; ++ii)
						buffer_wheel.push_back(mat4[ii]);
				}
			}
		}

		Data::get()->m_graphic.m_InstGeometry_chassis.update2(&buffer_chassis[0], buffer_chassis.size() * sizeof(float));
		Data::get()->m_graphic.m_InstGeometry_wheel.update2(&buffer_wheel[0], buffer_wheel.size() * sizeof(float));
	}

	{ // render raycast

		StackRenderer::t_color	sensors_color(0,1,0,1.0);
		StackRenderer::t_color	ground_sensor(1,0,0,1.0);

		auto&& arr_Cars = Data::get()->m_pSimulation->getCars();

		auto&	index_targetedCar = Data::get()->m_index_targetedCar;

		if (index_targetedCar >= 0)
		{
			auto&& car = arr_Cars[index_targetedCar];

			if (car.isAlive())
			{
				auto&& sensors = car.getSensors();

				for (auto&& sensor : sensors)
				{
					Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(sensor.near[0], sensor.near[1], sensor.near[2]), sensors_color);
					Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(sensor.far[0], sensor.far[1], sensor.far[2]), sensors_color);
				}

				auto&& sensor = car.getGroundSensor();
				Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(sensor.near[0], sensor.near[1], sensor.near[2]), ground_sensor);
				Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(sensor.far[0], sensor.far[1], sensor.far[2]), ground_sensor);
			}
		}

		Data::get()->m_StackRenderer.flush();
	}

	{ // trails

		auto&& car_trails = Data::get()->m_pSimulation->getCarTrails();

		StackRenderer::t_color	trails_color(1,1,1,1.0);

		for (auto&& buffer : car_trails)
		{
			for (unsigned int ii = 3; ii < buffer.size(); ii += 3)
			{
				Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(buffer[ii-3], buffer[ii-2], buffer[ii-1]), trails_color);
				Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(buffer[ii+0], buffer[ii+1], buffer[ii+2]), trails_color);
			}
		}
		Data::get()->m_StackRenderer.flush();
	}

	{ // instaced geometries

		float*	pMatrix = glm::value_ptr(Data::get()->m_composedMatrix);

		auto&& graphic = Data::get()->m_graphic;

		Shader::bind(&graphic.m_InstShader.getShader());

			graphic.m_InstGeometry_chassis.render(GL_LINES, pMatrix);
			graphic.m_InstGeometry_wheel.render(GL_LINES, pMatrix);

		Shader::bind(nullptr);
	}

	{ // static gemetries

		Shader::bind( Data::get()->m_ShaderColor.getShader() );

			float*	pMatrix = glm::value_ptr(Data::get()->m_composedMatrix);

			Data::get()->m_GeometryColor_circuit_skelton.render(GL_LINES, pMatrix);

			Data::get()->m_GeometryColor_circuit_ground.render(GL_TRIANGLES, pMatrix, 0.8f);

			Data::get()->m_GeometryColor_circuit_walls.render(GL_TRIANGLES, pMatrix, 0.2f);

		Shader::bind( nullptr );
	}

	{ // second camera

		auto&	window_size = Data::get()->m_graphic.window_size;

		glm::vec2	tmp_position = { window_size.x * 0.66f, 0.0f };
		glm::vec2	tmp_window_size = { window_size.x * 0.33f, window_size.y * 0.3f };
		float aspect_ratio = tmp_window_size.x / tmp_window_size.y;

		glViewport(tmp_position.x, tmp_position.y, tmp_window_size.x, tmp_window_size.y);
		glEnable(GL_SCISSOR_TEST);
		glScissor(tmp_position.x, tmp_position.y, tmp_window_size.x, tmp_window_size.y);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glScissor(0, 0, window_size.x, window_size.y);
		glDisable(GL_SCISSOR_TEST);

		//

        auto&	center = Data::get()->m_vec3_center;
        auto&	prev_center = Data::get()->m_vec3_previous_center;

		glm::mat4 Projection = glm::perspective(glm::radians(70.0f), aspect_ratio, 1.0f, 1000.f);
		glm::mat4 View = glm::lookAt(-prev_center, -center, {0.0f, 0.0f, 1.0f});
		glm::mat4 tmp_composedMatrix = Projection * View;

		//

		{
			Data::get()->m_StackRenderer.setMatrix( glm::value_ptr(tmp_composedMatrix) );
		}

		{ // render raycast

			StackRenderer::t_color	sensors_color(0,1,0,1.0);
			StackRenderer::t_color	ground_sensor(1,0,0,1.0);

			auto&& arr_Cars = Data::get()->m_pSimulation->getCars();

			auto&	index_targetedCar = Data::get()->m_index_targetedCar;

			if (index_targetedCar >= 0)
			{
				auto&& car = arr_Cars[index_targetedCar];

				if (car.isAlive())
				{
					auto&& sensors = car.getSensors();

					for (auto&& sensor : sensors)
					{
						Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(sensor.near[0], sensor.near[1], sensor.near[2]), sensors_color);
						Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(sensor.far[0], sensor.far[1], sensor.far[2]), sensors_color);
					}

					auto&& sensor = car.getGroundSensor();
					Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(sensor.near[0], sensor.near[1], sensor.near[2]), ground_sensor);
					Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(sensor.far[0], sensor.far[1], sensor.far[2]), ground_sensor);
				}
			}

			Data::get()->m_StackRenderer.flush();
		}

		{ // trails

			auto&& car_trails = Data::get()->m_pSimulation->getCarTrails();

			StackRenderer::t_color	trails_color(1,1,1,1.0);

			for (auto&& buffer : car_trails)
			{
				for (unsigned int ii = 3; ii < buffer.size(); ii += 3)
				{
					Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(buffer[ii-3], buffer[ii-2], buffer[ii-1]), trails_color);
					Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(buffer[ii+0], buffer[ii+1], buffer[ii+2]), trails_color);
				}
			}
			Data::get()->m_StackRenderer.flush();
		}

		{ // instaced geometries

			float*	pMatrix = glm::value_ptr(tmp_composedMatrix);

			auto&& graphic = Data::get()->m_graphic;

			Shader::bind(&graphic.m_InstShader.getShader());

				graphic.m_InstGeometry_chassis.render(GL_LINES, pMatrix);
				graphic.m_InstGeometry_wheel.render(GL_LINES, pMatrix);

			Shader::bind(nullptr);
		}

		{ // static gemetries

			Shader::bind( Data::get()->m_ShaderColor.getShader() );

				float*	pMatrix = glm::value_ptr(tmp_composedMatrix);

				Data::get()->m_GeometryColor_circuit_skelton.render(GL_LINES, pMatrix);

				Data::get()->m_GeometryColor_circuit_ground.render(GL_TRIANGLES, pMatrix, 0.8f);

				Data::get()->m_GeometryColor_circuit_walls.render(GL_TRIANGLES, pMatrix, 0.2f);

			Shader::bind( nullptr );
		}
	}

	{ // HUD

		// glClear(/*GL_COLOR_BUFFER_BIT |*/ GL_DEPTH_BUFFER_BIT);

		// {
		// 	// glm::mat4 Projection = glm::perspective(glm::radians(70.0f), 4.0f / 3.0f, 1.0f, 1000.f);
		// 	glm::mat4 Projection = glm::ortho(0.0f,800.0f, 0.0f,600.0f, -1.0f,+1.0f);

		// 	glm::mat4 View = glm::mat4(1.0f);
		// 	glm::mat4 composedMatrix = Projection * View;

		// 	Data::get()->m_StackRenderer.setMatrix( glm::value_ptr(composedMatrix) );
		// }


		// StackRenderer::t_color	trails_color(1,1,1,1.0);

		// { // render button

		// 	Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(100,100,0), trails_color);
		// 	Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(100,200, 0), trails_color);

		// 	Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(100,200, 0), trails_color);
		// 	Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(200,200, 0), trails_color);

		// 	Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(200,200, 0), trails_color);
		// 	Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(200,100, 0), trails_color);

		// 	Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(200,100, 0), trails_color);
		// 	Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(100,100, 0), trails_color);
		// }

		// Data::get()->m_StackRenderer.flush();
	}


	// SDL_GL_SwapBuffers();
}

void	State_Main::resize(int width, int height)
{
	Data::get()->m_graphic.window_size = { width, height };
}
