

// #include "State_RunBest.hpp"

// #include "StateManager.hpp"

// #include "Logic/Data.hpp"
// #include "Logic/Graphic/Shader.hpp"

// #include "Utility/TraceLogger.hpp"


// // #include <chrono>
// #include <ctime>

// #include <iostream>


// State_RunBest::State_RunBest()
// {
// }

// State_RunBest::~State_RunBest()
// {
// }

// //

// void	State_RunBest::handleEvent(const SDL_Event& event)
// {
// 	auto&	keys = Data::get()->m_input.keys;

//     switch (event.type)
//     {
//     case SDL_KEYDOWN:
//         keys[event.key.keysym.sym] = true;
//         break;

//     case SDL_KEYUP:
//         keys[event.key.keysym.sym] = false;
//         break;

//     case SDL_MOUSEBUTTONDOWN:
// 		{
// 			int tmp_X, tmp_Y;
// 			SDL_GetMouseState(&tmp_X, &tmp_Y);
// 			D_MYLOG("click, x=" << tmp_X << ", y=" << tmp_Y);

// 			if (tmp_X > 100 && tmp_X < 200 &&
// 				tmp_Y > 400 && tmp_Y < 500)
// 			{
// 				D_MYLOG("TOUCHED!");

// 				auto&& cars = Data::get()->m_pSimulation->getCars();
// 				for (const Car& car : cars)
// 					car.reset();

// 				StateManager::get()->changeState(StateManager::e_States::eMain);
// 			}
// 		}
// 	    break;

//     default:
//         break;
//     }
// }





// #include <glm/vec2.hpp> // glm::vec2
// #include <glm/vec3.hpp> // glm::vec3
// #include <glm/vec4.hpp> // glm::vec4
// #include <glm/mat4x4.hpp> // glm::mat4
// #include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
// #include <glm/gtc/type_ptr.hpp> // glm:value_ptr


// namespace
// {
// 	glm::mat4 camera(const glm::vec3& in_Pos, float in_distance, const glm::vec2& in_Rotate)
// 	{
// 		glm::mat4 Projection = glm::perspective(glm::radians(70.0f), 4.0f / 3.0f, 1.0f, 1000.f);

// 		glm::mat4 View = glm::mat4(1.0f);

// 		View = glm::translate(View, glm::vec3(0.0f, 0.0f, -in_distance));
// 		View = glm::rotate(View, in_Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
// 		View = glm::rotate(View, in_Rotate.x, glm::vec3(0.0f, 0.0f, 1.0f));

// 		View = glm::translate(View, in_Pos);

// 		// glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
// 		glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
// 		return Projection * View * Model;
// 	}
// };




// void	State_RunBest::update(int delta)
// {
// 	float elapsed_time = static_cast<float>(delta) / 1000;

// 	auto&	rotations = Data::get()->m_vec2_rotations;

// 	{ // events

// 		auto&	keys = Data::get()->m_input.keys;

// 		Data::get()->m_simualtion_step = (keys[SDLK_SPACE] ? 60 : 1);
// 		// Data::get()->m_simualtion_step = (keys[SDLK_SPACE] ? 1 : 60);

// 		if (keys[SDLK_LEFT] ||
// 			keys[SDLK_a] ||
// 			keys[SDLK_q])
// 			rotations.x -= 2.0f * elapsed_time;
// 		else if (keys[SDLK_RIGHT] ||
// 				 keys[SDLK_d])
// 			rotations.x += 2.0f * elapsed_time;

// 		if (keys[SDLK_UP] ||
// 			keys[SDLK_w] ||
// 			keys[SDLK_z])
// 			rotations.y += 1.0f * elapsed_time;
// 		else if (keys[SDLK_DOWN] ||
// 				 keys[SDLK_s])
// 			rotations.y -= 1.0f * elapsed_time;
// 	}

// 	{ // camera

// 		glm::vec3	camera_pos = glm::vec3(0.0f);

// 		//
// 		//

// 		// auto&	index_targetedCar = Data::get()->m_index_targetedCar;
// 		int	index_targetedCar = Data::get()->m_pSimulation->getCars().size();

// 		auto&	timeout_camera = Data::get()->m_timeout_camera;
// 		// timeout_camera -= delta;

// 		if (Data::get()->m_simualtion_step == 1)
// 		{
// 			// if (timeout_camera <= 0)
// 			// {
// 			// 	timeout_camera = 500;

// 		 //        float	curr_fitness = -1;

// 		 //        float	mat4x4[16];

// 		 //        auto& Cars = Data::get()->m_pSimulation->getCars();

// 		 //        for (auto& car : Cars)
// 		 //        {
// 		 //            if (!car.isAlive())
// 		 //                continue;

// 		 //            if (curr_fitness > car.getFitness())
// 		 //                continue;

// 		 //            curr_fitness = car.getFitness();

// 			// 		index_targetedCar = car.getIndex();

// 		 //        	Data::get()->m_pPhysicWrapper->vehicle_getOpenGLMatrix(index_targetedCar, mat4x4);

// 			// 		camera_pos.x = -mat4x4[12];
// 			// 		camera_pos.y = -mat4x4[13];
// 			// 		camera_pos.z = -mat4x4[14];
// 		 //        }
// 			// }
// 			// else if (index_targetedCar >= 0)
// 			{
// 		        float	mat4x4[16];

// 				Data::get()->m_pPhysicWrapper->vehicle_getOpenGLMatrix(index_targetedCar, mat4x4);

// 				camera_pos.x = -mat4x4[12];
// 				camera_pos.y = -mat4x4[13];
// 				camera_pos.z = -mat4x4[14];
// 			}
// 			// else
// 			// {
// 			// 	camera_pos.x = camera_pos.y = camera_pos.z = 0;
// 			// }
// 		}
// 		else
// 		{
// 			camera_pos.x = camera_pos.y = camera_pos.z = 0;
// 		}

//         //
//         //

//         auto&	center = Data::get()->m_vec3_center;

//         glm::vec3	diff(camera_pos.x - center.x, camera_pos.y - center.y, camera_pos.z - center.z);

//         float	lerp_ratio = 0.15;

//         center.x += diff.x * lerp_ratio;
//         center.y += diff.y * lerp_ratio;
//         center.z += diff.z * lerp_ratio;

// 		//
// 		//

// 		Data::get()->m_composedMatrix = camera(center, 50, rotations);

// 		Data::get()->m_StackRenderer.setMatrix( glm::value_ptr(Data::get()->m_composedMatrix) );

// 	} // camera

// 	for (int i = 0; i < Data::get()->m_simualtion_step; ++i)
// 	{
// 		Data::get()->m_pSimulation->updateBest(1.0f / 60); // <= MUST be constant
// 	}
// }


// void	State_RunBest::render(const SDL_Window& window)
// {

// 	// glViewport(0, 0, window.w, window.h);

// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


// 	// Data::get()->m_PhysicWorld.debugDrawWorld();

// 	// Data::get()->m_StackRenderer.flush();


// 	{ // raycast

// 		StackRenderer::t_color	sensors_color(0,1,0,1.0);
// 		StackRenderer::t_color	ground_sensor(1,0,0,1.0);

// 		// auto&& arr_Cars = Data::get()->m_pSimulation->getCars();

// 		// auto&	index_targetedCar = Data::get()->m_index_targetedCar;

// 		// if (index_targetedCar >= 0)
// 		// for (auto&& car : arr_Cars)
// 		{
// 			// auto&& car = arr_Cars[index_targetedCar];

// 			auto&& car = Data::get()->m_pSimulation->getBestCar();

// 			// if (car.isAlive())
// 			{
// 				auto&& sensors = car.getSensors();

// 				for (auto&& sensor : sensors)
// 				{
// 					Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(sensor.near[0], sensor.near[1], sensor.near[2]), sensors_color);
// 					Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(sensor.far[0], sensor.far[1], sensor.far[2]), sensors_color);
// 				}

// 				auto&& sensor = car.getGroundSensor();
// 				Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(sensor.near[0], sensor.near[1], sensor.near[2]), ground_sensor);
// 				Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(sensor.far[0], sensor.far[1], sensor.far[2]), ground_sensor);
// 			}
// 		}


// 		Data::get()->m_StackRenderer.flush();

// 	} // raycast

// 	{ // trails

// 		auto&& car_trails = Data::get()->m_pSimulation->getCarTrails();

// 		StackRenderer::t_color	trails_color(1,1,1,1.0);

// 		for (auto&& buffer : car_trails)
// 		{
// 			for (unsigned int ii = 3; ii < buffer.size(); ii += 3)
// 			{
// 				Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(buffer[ii-3], buffer[ii-2], buffer[ii-1]), trails_color);
// 				Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(buffer[ii+0], buffer[ii+1], buffer[ii+2]), trails_color);
// 			}
// 		}
// 		Data::get()->m_StackRenderer.flush();

// 	} // trails

// 	{
// 		// auto&& arr_Cars = Data::get()->m_pSimulation->getCars();

// 		auto&	PhysicWorld = Data::get()->m_PhysicWorld;

// 		Shader::bind( Data::get()->m_ShaderColor.getShader() );

// 		btScalar	mat4[16];

// 		// for (unsigned int i = 0; i < PhysicWorld.getVehicleSize(); ++i)
// 		{
// 			// if (!arr_Cars[i].isAlive())
// 			// 	continue;

// 			// auto*	pVehicle = PhysicWorld.getVehicle(i);
// 			auto*	pVehicle = PhysicWorld.getVehicle(PhysicWorld.getVehicleSize() - 1);

// 			//

// 			{
// 				pVehicle->getOpenGLMatrix(mat4);

// 				glm::mat4	tmp_mat = Data::get()->m_composedMatrix * glm::make_mat4((float*)mat4);

// 				tmp_mat = glm::translate(tmp_mat, glm::vec3(0.0f, 0.0f, 1.0f));

// 				float*	pMatrix = glm::value_ptr(tmp_mat);

// 				// if (arr_Cars[i].isAlive())
// 					Data::get()->m_GeometryColor_chassis.render(GL_LINES, pMatrix);
// 				// else
// 				// 	Data::get()->m_GeometryColor_chassis_dead.render(GL_LINES, pMatrix);
// 			}

// 			//

// 			{
// 				for (int jj = 0; jj < pVehicle->getNumWheels(); ++jj)
// 				{
// 					pVehicle->getWheelMatrix(jj, mat4);

// 					glm::mat4	tmp_mat = Data::get()->m_composedMatrix * glm::make_mat4((float*)mat4);

// 					float*	pMatrix = glm::value_ptr(tmp_mat);

// 					Data::get()->m_GeometryColor_wheel.render(GL_LINES, pMatrix);
// 				}
// 			}
// 		}

// 		Shader::bind( nullptr );
// 	}


// 	Shader::bind( Data::get()->m_ShaderColor.getShader() );

// 		float*	pMatrix = glm::value_ptr(Data::get()->m_composedMatrix);

// 		// // Data::get()->m_GeometryColor.render(GL_TRIANGLES, pMatrix);

// 		// Data::get()->m_GeometryColor_chassis.render(GL_LINES, pMatrix);

// 		// Data::get()->m_GeometryColor_wheel.render(GL_LINES, pMatrix);

// 		Data::get()->m_GeometryColor_circuit_skelton.render(GL_LINES, pMatrix);

// 		Data::get()->m_GeometryColor_circuit_ground.render(GL_TRIANGLES, pMatrix, 1.0f);

// 		Data::get()->m_GeometryColor_circuit_walls.render(GL_TRIANGLES, pMatrix, 0.2f);

// 	Shader::bind( nullptr );


// 	{ // HUD

// 		glClear(/*GL_COLOR_BUFFER_BIT |*/ GL_DEPTH_BUFFER_BIT);

// 		{
// 			// glm::mat4 Projection = glm::perspective(glm::radians(70.0f), 4.0f / 3.0f, 1.0f, 1000.f);
// 			glm::mat4 Projection = glm::ortho(0.0f,800.0f, 0.0f,600.0f, -1.0f,+1.0f);

// 			glm::mat4 View = glm::mat4(1.0f);
// 			glm::mat4 composedMatrix = Projection * View;

// 			Data::get()->m_StackRenderer.setMatrix( glm::value_ptr(composedMatrix) );
// 		}


// 		StackRenderer::t_color	trails_color(1,1,1,1.0);

// 		{ // render button

// 			Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(100,100,0), trails_color);
// 			Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(100,200, 0), trails_color);

// 			Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(100,200, 0), trails_color);
// 			Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(200,200, 0), trails_color);

// 			Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(200,200, 0), trails_color);
// 			Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(200,100, 0), trails_color);

// 			Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(200,100, 0), trails_color);
// 			Data::get()->m_StackRenderer.push_vertex(StackRenderer::t_pos(100,100, 0), trails_color);
// 		}

// 		Data::get()->m_StackRenderer.flush();
// 	}


// 	// SDL_GL_SwapBuffers();
// }

