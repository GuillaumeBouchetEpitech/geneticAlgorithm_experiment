

#include "RendererSFML.hpp"


#include "../Simulation/utils/utils.hpp"
#include "../Simulation/utils/TraceLogger.hpp"


#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>


namespace
{

	// void drawLine(sf::RenderWindow& rwindow, const t_line& line, const sf::Color& color1, const sf::Color& color2)
	// {
	// 	const t_vec2f& p1 = line.p1;
	// 	const t_vec2f& p2 = line.p2;

	// 	// define a 100x100 square, red, with a 10x10 texture mapped on it
	// 	sf::Vertex vertices[] = {
	// 		sf::Vertex( sf::Vector2f(p1.x, p1.y), color1 ),
	// 		sf::Vertex( sf::Vector2f(p2.x, p2.y), color2 )
	// 	};

	// 	// draw it
	// 	rwindow.draw(vertices, 2, sf::Lines);
	// }

	void drawLine(sf::RenderWindow& rwindow, const t_line& line, const sf::Color& color, float thickness = 1.0f)
	{
		float length = sqrtf( (line.p1.x-line.p2.x)*(line.p1.x-line.p2.x) + (line.p1.y-line.p2.y)*(line.p1.y-line.p2.y) );

		sf::RectangleShape recline(sf::Vector2f(length, thickness));

		recline.setOrigin(0, thickness/2);
		recline.setPosition(line.p1.x, line.p1.y);

		recline.setFillColor(color);

		float angle = atan2f(line.p2.y - line.p1.y, line.p2.x - line.p1.x);

		recline.rotate(angle * 180.0f / 3.14f);
		rwindow.draw(recline);
	}

	void drawLines(sf::RenderWindow& rwindow, const t_lines& lines, const sf::Color& color1, const sf::Color& color2)
	{
		static std::vector<sf::Vertex>	s_vertices;

		s_vertices.clear();
		for (unsigned int i = 0; i < lines.size(); ++i)
		{
			s_vertices.push_back( sf::Vertex( sf::Vector2f(lines[i].p1.x, lines[i].p1.y), color1 ) );
			s_vertices.push_back( sf::Vertex( sf::Vector2f(lines[i].p2.x, lines[i].p2.y), color2 ) );
		}

		rwindow.draw(&(s_vertices[0]), s_vertices.size(), sf::Lines);
	}

	void drawPoint(sf::RenderWindow& rwindow, const t_vec2f& point, const sf::Color& color)
	{
		float size = 10.0f;
		sf::Vertex vertices[] = {
			sf::Vertex(sf::Vector2f(point.x - size, point.y - size), color),
			sf::Vertex(sf::Vector2f(point.x + size, point.y + size), color),
			sf::Vertex(sf::Vector2f(point.x + size, point.y - size), color),
			sf::Vertex(sf::Vector2f(point.x - size, point.y + size), color)
		};

		rwindow.draw(vertices, 4, sf::Lines);
	}

	void drawCar(sf::RenderWindow& rwindow, const Car& car, const sf::Color& color, bool render_sensors)
	{
		const t_vec2f& position = car.getPosition();
		float angle = car.getAngle();

		{
			float size_h = 25.0f;
			float size_v = 12.5f;

			float ratio = car.getLife();

			t_vec2f	positions[4] = {
				t_vec2f(position.x - size_h, position.y - size_v),
				t_vec2f(position.x - size_h + size_h * 2.0f * ratio, position.y - size_v),
				t_vec2f(position.x - size_h + size_h * 2.0f * ratio, position.y + size_v),
				t_vec2f(position.x - size_h, position.y + size_v)
			};

			for (int i = 0; i < 4; ++i)
				positions[i] = rotateVec2(positions[i], position, angle);

			// sf::Color col = (car.isAlive() ? color : sf::Color::Red);
			// sf::Color col = sf::Color::Green;
			sf::Color col = (car.isAlive() ? sf::Color::Green : sf::Color::Yellow);
			col.g *= ratio;

			sf::Vertex p1(sf::Vector2f(positions[0].x, positions[0].y), col);
			sf::Vertex p2(sf::Vector2f(positions[1].x, positions[1].y), col);
			sf::Vertex p3(sf::Vector2f(positions[2].x, positions[2].y), col);
			sf::Vertex p4(sf::Vector2f(positions[3].x, positions[3].y), col);

			sf::Vertex vertices[] = { p1,p2,p3, p1,p4,p3 };

			rwindow.draw(vertices, 6, sf::Triangles);
		}

		{
			float size_h = 25.0f;
			float size_v = 12.5f;

			t_vec2f	positions[4] = {
				t_vec2f(position.x - size_h, position.y - size_v),
				t_vec2f(position.x + size_h, position.y - size_v),
				t_vec2f(position.x + size_h, position.y + size_v),
				t_vec2f(position.x - size_h, position.y + size_v)
			};

			for (int i = 0; i < 4; ++i)
				positions[i] = rotateVec2(positions[i], position, angle);

			sf::Color col = (car.isAlive() ? color : sf::Color::Red);

			sf::Vertex p1(sf::Vector2f(positions[0].x, positions[0].y), col);
			sf::Vertex p2(sf::Vector2f(positions[1].x, positions[1].y), col);
			sf::Vertex p3(sf::Vector2f(positions[2].x, positions[2].y), col);
			sf::Vertex p4(sf::Vector2f(positions[3].x, positions[3].y), col);

			sf::Vertex vertices[] = { p1,p2, p2,p3, p3,p4, p4,p1 };

			glLineWidth(5.0f);

			rwindow.draw(vertices, 8, sf::Lines);

			glLineWidth(1.0f);
		}

		///

		if (!car.isAlive() || !render_sensors)
			return;

		const Car::t_sensors&	sensors = car.getSensors();

		for (Car::t_sensor sensor : sensors)
		{
			sf::Color	LightBlue(128,128,255);
			// drawLine(rwindow, sensor.m_line, LightBlue, LightBlue);

			t_vec2f	pos;
			pos.x = sensor.m_line.p1.x + (sensor.m_line.p2.x - sensor.m_line.p1.x) * sensor.m_value;
			pos.y = sensor.m_line.p1.y + (sensor.m_line.p2.y - sensor.m_line.p1.y) * sensor.m_value;

			t_line	tmp_line(sensor.m_line.p1, pos);
			t_line	tmp_line2(pos, sensor.m_line.p2);

			// drawLine(rwindow, tmp_line, LightBlue, LightBlue);
			drawLine(rwindow, tmp_line, LightBlue, 5.0f);
			drawLine(rwindow, tmp_line2, sf::Color::Red, 1.0f);

			drawPoint(rwindow, pos, sf::Color::Yellow);
		}
	}

}; // namespace
















RendererSFML::RendererSFML(Simulation& simulation)
	: m_Simulation(simulation)
{
}




void	RendererSFML::run(std::function<void()> simulation_callback)
{
	const t_lines&	checkpoints = m_Simulation.getCircuit().getCheckpoints();
	const t_lines&	walls = m_Simulation.getCircuit().getWalls();

	// Create the main window
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
	window.setFramerateLimit(60);

	int	index_target_car = -1;
	int	frames_until_new_target = 30;
	sf::Vector2f	camera_center;

	// Start the game loop
	while (window.isOpen())
	{
		//
		//
		//
		// Process events

		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close window: exit
			if (event.type == sf::Event::Closed)
				window.close();

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
				window.close();

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::P))
			{
				D_MYLOG("getCurrentGeneration=" << m_Simulation.getCurrentGeneration());
				D_MYLOG("getBestFitness=" << m_Simulation.getBestFitness());

				for (unsigned int i = 0; i < m_Simulation.getCars().size(); ++i)
				{
					const Car& c = m_Simulation.getCars()[i];

					const t_vec2f& pos = c.getPosition();
					float angle = c.getAngle();

					D_MYLOG("[" << i << "] pos=" << pos.x << "/" << pos.y << ", angle=" << angle << ", alive=" << c.isAlive() << ", id=" << m_Simulation.getGenomes()[i].m_id);
				}
			}
		}


		int iteration_nbr = 3;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			iteration_nbr = 290;
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			iteration_nbr = 1;



		//
		//
		//
		// update

		for (int i = 0; i < iteration_nbr; ++i)
			simulation_callback();
			// m_Simulation.update(0.25f);


		//
		//
		//
		// render

		{ // camera

			sf::View	view;
			view.reset(sf::FloatRect(0, 0, 800, 600));

			t_vec2f	target;
			

			auto&& cars = m_Simulation.getCars();

			--frames_until_new_target;
			if (frames_until_new_target <= 0 ||
				index_target_car < 0 ||
				!cars[index_target_car].isAlive())
			{
				frames_until_new_target = 60;

		        float	curr_fitness = -1;

		        for (unsigned int ii = 0; ii < cars.size(); ++ii)
		        {
		        	auto& car = cars[ii];

		            if (!car.isAlive())
		                continue;

		            if (curr_fitness > car.getFitness())
		                continue;

		            curr_fitness = car.getFitness();

		            index_target_car = ii;
		        }
			}

			if (index_target_car >= 0)
			{
				target = cars[index_target_car].getPosition();
			}


			// unsigned int index = 0;
			// for (; index < m_Simulation.getCars().size(); ++index)
			// 	if (m_Simulation.getCars()[index].isAlive())
			// 		break;

			// if (index < m_Simulation.getCars().size())
			// {
			// 	index_target_car = index;
			// 	target = m_Simulation.getCars()[index_target_car].getPosition();
			// }
			// else
			// 	index_target_car = -1;




			sf::Vector2f	diff(target.x-camera_center.x-200, target.y-camera_center.y);

            camera_center.x += diff.x * 0.1;
            camera_center.y += diff.y * 0.1;

			// view.setCenter(400,300);
			if (iteration_nbr <= 3)
				view.setCenter(camera_center);
			else
			{
				view.zoom(7.0f);
				view.setCenter(sf::Vector2f(0, -1700));
				// view.reset(sf::FloatRect(-800, -600, 1600, 1200));
			}

			window.setView(view);

		} // camera

		// Clear screen
		window.clear();

		// // render starting point
		// drawPoint(window, m_Simulation.getCircuit().getStartingPositon(), sf::Color::Blue);

		// render circuit
		drawLines(window, checkpoints, sf::Color(255,255,255), sf::Color(128,128,128));
		drawLines(window, walls, sf::Color::Blue, sf::Color(128,128,128));

		{ // render cars

			// render cars (except targetted car)
			auto arr_cars = m_Simulation.getCars();
			for (int index = 0; index < static_cast<int>(arr_cars.size()); ++index)
				if (index != index_target_car)
					drawCar(window, arr_cars[index], sf::Color::Green, false);

			// render targetted car
			if (index_target_car != -1)
				drawCar(window, arr_cars[index_target_car], sf::Color::Blue, true);

		} // render cars

		{ // render trails

			const std::vector< std::vector<t_line> >&	trails = m_Simulation.getTrails();
			if (!trails.empty())
			{
				for (unsigned int i = 0; i < trails.size() - 1; ++i)
				{
					sf::Color	color = sf::Color::Black;
					color.g = 128 + ((float)i / trails.size()) * 128;
					drawLines(window, trails[i], color, color);
				}

				drawLines(window, trails.back(), sf::Color::White, sf::Color::White);
			}

		} // render trails

		/**/
		{ // hud

			// window.setView(window.getDefaultView());

			sf::View	view;
			view.reset(sf::FloatRect(0, 0, 800, 600));
			view.move(-100,-100);
			window.setView(view);

			if (index_target_car != -1)
			{
				const GeneticAlgorithm::t_genome& genome = m_Simulation.getGenomes()[index_target_car];

				const auto& ann_topology = m_Simulation.getNNTopology();

				//
				// concatenate the weights in a usable array
				//

				std::vector<unsigned int> arr_topology;
				arr_topology.reserve(2 + ann_topology.getHiddens().size());

				arr_topology.push_back(ann_topology.getInput());
				for (auto& elem : ann_topology.getHiddens())
					arr_topology.push_back(elem);
				arr_topology.push_back(ann_topology.getOutput());

				unsigned int arr_topology_max = arr_topology[0];
				for (unsigned int index = 1; index < arr_topology.size(); ++index)
					arr_topology_max = std::max(arr_topology_max, arr_topology[index]);

				//
				// show weights
				//

				unsigned int	windex = 0;
				for (unsigned int index = 1; index < arr_topology.size(); ++index)
				{
					int prev_layer = arr_topology[index-1];
					int curr_layer = arr_topology[index];

					float prev_dec_y = (1.0f - ((float)prev_layer / arr_topology_max)) * 185;
					float curr_dec_y = (1.0f - ((float)curr_layer / arr_topology_max)) * 185;

					for (int pindex = 0; pindex < prev_layer; ++pindex)
						for (int cindex = 0; cindex < curr_layer; ++cindex)
						{
							sf::Vector2f	p1;
							p1.x = (index-1) * 75;
							p1.y = prev_dec_y + pindex * 75;

							sf::Vector2f	p2;
							p2.x = (index) * 75;
							p2.y = curr_dec_y + cindex * 75;

							float wvalue = genome.m_weights[windex++];

							float ratio = wvalue * 10.0f;
							if (ratio < 0)  ratio = -ratio;
							if (ratio < 1)  ratio = 1;

							t_line line(p1.x, p1.y, p2.x, p2.y);


							if (wvalue > 0)
								drawLine(window, line, sf::Color::Red, ratio);
							else
								drawLine(window, line, sf::Color::Blue, ratio);
						}
				}

				//
				// show neurons
				//

				for (unsigned int index = 0; index < arr_topology.size(); ++index)
				{
					int curr_layer = arr_topology[index];

					float curr_dec_y = (1.0f - ((float)curr_layer / arr_topology_max)) * 185;

					for (int cindex = 0; cindex < curr_layer; ++cindex)
					{
						int curr_x = (index) * 75;

						sf::CircleShape circle;
						circle.setRadius(10);
						circle.setOrigin(10,10);
						circle.setOutlineColor(sf::Color::Yellow);
						circle.setOutlineThickness(3);
						circle.setPosition(curr_x, curr_dec_y + cindex * 75);
						window.draw(circle);
					}
				}

			}

		} // hud
		//*/

		// Update the window
		window.display();
	}
}


