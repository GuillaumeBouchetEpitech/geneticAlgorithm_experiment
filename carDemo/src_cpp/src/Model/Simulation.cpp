
#include "Simulation.hpp"

#include <fstream>
#include <cmath>







//
//
//
// UTILS

bool CollisionDroiteSeg_BBox(const t_vec2f& A, const t_vec2f& B, const t_vec2f& O, const t_vec2f& P)
{
	t_vec2f minAB(std::min(A.x, B.x), std::min(A.y, B.y));
	t_vec2f minOP(std::min(O.x, P.x), std::min(O.y, P.y));
	t_vec2f maxAB(std::max(A.x, B.x), std::max(A.y, B.y));
	t_vec2f maxOP(std::max(O.x, P.x), std::max(O.y, P.y));

	return !(maxAB.x < minOP.x ||
			 minAB.x > maxOP.x ||
			 maxAB.y < minOP.y ||
			 minAB.y > maxOP.y);
}

bool CollisionDroiteSeg(const t_vec2f& A, const t_vec2f& B, const t_vec2f& O, const t_vec2f& P)
{
	t_vec2f AO,AP,AB;
	AB.x = B.x - A.x;
	AB.y = B.y - A.y;
	AP.x = P.x - A.x;
	AP.y = P.y - A.y;
	AO.x = O.x - A.x;
	AO.y = O.y - A.y;

	return ((AB.x * AP.y - AB.y * AP.x) * (AB.x * AO.y - AB.y * AO.x) < 0.0f);
}

float CollisionSegSeg_partial(const t_vec2f& A, const t_vec2f& B, const t_vec2f& O, const t_vec2f& P)
{
	// THIS IS SLOWER
	// if (!CollisionDroiteSeg_BBox(A,B,O,P))
	// 	return 1.0f;
	// /THIS IS SLOWER

	if (!CollisionDroiteSeg(A,B,O,P))
		return 1.0f;

	t_vec2f AB,OP;
	AB.x = B.x - A.x;
	AB.y = B.y - A.y;
	OP.x = P.x - O.x;
	OP.y = P.y - O.y;

	return -(A.x * OP.y - O.x * OP.y - OP.x * A.y + OP.x * O.y) / (AB.x * OP.y - AB.y * OP.x);
}

bool CollisionSegSeg(const t_vec2f& A, const t_vec2f& B, const t_vec2f& O, const t_vec2f& P)
{
	float k = CollisionSegSeg_partial(A, B, O, P);

	return (!(k < 0.0f || k > 1.0f));
}

/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////

bool	CollisionPointCercle(const t_vec2f& P, const t_vec2f& C, float radius)
{
	int d2 = (P.x - C.x) * (P.x - C.x) + (P.y - C.y) * (P.y - C.y);

	return (d2 <= radius * radius);
}

bool	CollisionDroiteCercle_BBox(const t_vec2f& A, const t_vec2f&  B, const t_vec2f& C, float radius)
{
	t_vec2f minAB(std::min(A.x, B.x), std::min(A.y, B.y));
	t_vec2f maxAB(std::max(A.x, B.x), std::max(A.y, B.y));
	t_vec2f minC(C.x - radius, C.y - radius);
	t_vec2f maxC(C.x + radius, C.y + radius);

	return !(maxAB.x < minC.x ||
			 minAB.x > maxC.x ||
			 maxAB.y < minC.y ||
			 minAB.y > maxC.y);
}

bool	CollisionDroiteCercle(const t_vec2f& A, const t_vec2f&  B, const t_vec2f& C, float radius)
{
	if (!CollisionDroiteCercle_BBox(A,B,C,radius))
		return false;

	t_vec2f	u(B.x - A.x, B.y - A.y);
	t_vec2f	AC(C.x - A.x, C.y - A.y);

	float numerateur = u.x * AC.y - u.y * AC.x;   // norme du vecteur v

	if (numerateur < 0)
		numerateur = -numerateur ;   // valeur absolue ; si c'est négatif, on prend l'opposé.

	float denominateur = sqrtf(u.x * u.x + u.y * u.y);  // norme de u
	float CI = numerateur / denominateur;

	return (CI < radius);
}

bool	CollisionSegmentCercle(const t_vec2f& A, const t_vec2f&  B, const t_vec2f& C, float radius)
{
	if (CollisionDroiteCercle(A, B, C, radius) == false)
		return false;  // si on ne touche pas la droite, on ne touchera jamais le segment

	t_vec2f AB, AC, BC;
	AB.x = B.x - A.x;
	AB.y = B.y - A.y;
	AC.x = C.x - A.x;
	AC.y = C.y - A.y;
	BC.x = C.x - B.x;
	BC.y = C.y - B.y;

	float pscal1 = AB.x * AC.x + AB.y * AC.y;  // produit scalaire
	float pscal2 = (-AB.x) * BC.x + (-AB.y) * BC.y;  // produit scalaire

	if (pscal1 >= 0 && pscal2 >= 0)
		return true;   // I entre A et B, ok.

	// dernière possibilité, A ou B dans le cercle
	if (CollisionPointCercle(A, C, radius) ||
		CollisionPointCercle(B, C, radius))
		return true;

	return false;
}

t_vec2f	rotateVec2(const t_vec2f& point, const t_vec2f& center, float angle)
{
	t_vec2f	newPos;

	float cos_a = cosf(angle);
	float sin_a = sinf(angle);

	newPos.x = center.x + (point.x-center.x) * cos_a + (point.y-center.y)*sin_a;
	newPos.y = center.y + (point.x-center.x) * sin_a - (point.y-center.y)*cos_a;

	return newPos;
}

t_vec2f	lerp(const t_vec2f& p1, const t_vec2f& p2, float ratio)
{
	t_vec2f ret_val;
	ret_val.x = p1.x + (p2.x - p1.x) * ratio;
	ret_val.y = p1.y + (p2.y - p1.y) * ratio;
	return ret_val;
}

// UTILS
//
//
//













//
//
//
// CIRCUIT

Circuit::Circuit()
	:	m_valid(false)
{}

Circuit::~Circuit()
{}

bool Circuit::loadMap(const std::string& filename)
{
	// D_MYLOG("filename=" << filename);

	// open

	std::ifstream ifs;
	ifs.open(filename, std::ifstream::in);

	if (!ifs.is_open())
	{
		// D_MYLOG("fail to open");
		return false;
	}

	// loop

	std::string line;
	std::string label;

	while (ifs.good())
	{
		std::getline(ifs, line);

		if (line.empty())
		{
			// D_MYLOG("[empty line]");
			continue;
		}

		// D_MYLOG("line=" << line);

		std::stringstream sstr(line);
		sstr >> label;

		//
		// checkpoints

		if (label == "checkpoint")
		{
			// D_MYLOG("processing checkpoint");

			std::vector<float> vals;
			float val;
			while (sstr >> val)
			{
				if (std::isnan(val))
				{
					// D_MYLOG("invalid value -> Not a Number");
					return false;
				}

				vals.push_back(val);
			}

			if (vals.size() != 4)
			{
				// D_MYLOG("invalid number of values");
				return false;
			}

			// D_MYLOG("processed checkpoint");
			// D_MYLOG("vals: " << vals[0] << "," << vals[1] << "  " << vals[2] << "," << vals[3]);

			t_vec2f p1(vals[0],vals[1]);
			t_vec2f p2(vals[2],vals[3]);
			m_checkpoints.push_back( t_line(p1, p2) );
		}

		// checkpoints
		//

		else
		{
			// D_MYLOG("unidentified line");
		}
	}

	// close

	ifs.close();

	// final check

	// TODO : more complex check required here
	// -> need to use m_valid
	if (m_checkpoints.size() < 3)
		return false;

	// process walls

	for (unsigned int i = 1; i < m_checkpoints.size(); ++i)
	{
		const t_line& prev = m_checkpoints[i - 1];
		const t_line& curr = m_checkpoints[i];

		m_walls.push_back( t_line(prev.p1, curr.p1) );
		m_walls.push_back( t_line(prev.p2, curr.p2) );
	}	
	// m_walls.push_back( t_line(m_checkpoints.front().p1, m_checkpoints.back().p1) );
	// m_walls.push_back( t_line(m_checkpoints.front().p2, m_checkpoints.back().p2) );

	// expand the walls (fix collisions bugs)

	for (unsigned int i = 0; i < m_walls.size(); ++i)
	{
		m_walls[i].p1 = lerp(m_walls[i].p1, m_walls[i].p2, -0.01f);
		m_walls[i].p2 = lerp(m_walls[i].p1, m_walls[i].p2, 1.01f);
	}

	// duplicate the checkpoints

	t_lines	tmp_checkpoints;
	tmp_checkpoints.push_back(m_checkpoints[0]);
	for (unsigned int i = 1; i < m_checkpoints.size(); ++i)
	{
		const t_line& l1 = m_checkpoints[i - 1];
		const t_line& l2 = m_checkpoints[i];

		t_line	tmp_line;
		for (float f = 0.25f; f < 1.0f; f += 0.25f)
		{
			tmp_line.p1 = lerp(l1.p1, l2.p1, f);
			tmp_line.p2 = lerp(l1.p2, l2.p2, f);
			tmp_checkpoints.push_back(tmp_line);
		}

		tmp_checkpoints.push_back(l2);
	}
	// {
	// 	const t_line& l1 = m_checkpoints.front();
	// 	const t_line& l2 = m_checkpoints.back();

	// 	t_line	tmp_line;
	// 	tmp_line.p1 = lerp(l1.p1, l2.p1, 0.5f);
	// 	tmp_line.p2 = lerp(l1.p2, l2.p2, 0.5f);
	// 	tmp_checkpoints.push_back(tmp_line);

	// 	tmp_checkpoints.push_back(l2);
	// }
	m_checkpoints = tmp_checkpoints;

	// print checkpoints

	// D_MYLOG("print checkpoints");
	// for (unsigned int i = 0; i < m_checkpoints.size(); ++i)
	// {
	// 	const t_line& l = m_checkpoints[i];
	// 	D_MYLOG("[" << i << "]: " << l.p1.x << "," << l.p1.y << "  " << l.p2.x << "," << l.p2.y);
	// }



	// print walls

	// D_MYLOG("print walls");
	// for (unsigned int i = 0; i < m_walls.size(); ++i)
	// {
	// 	const t_line& l = m_walls[i];
	// 	D_MYLOG("[" << i << "]: " << l.p1.x << "," << l.p1.y << "  " << l.p2.x << "," << l.p2.y);
	// }

	// process starting position

	{
		t_line& l1 = m_checkpoints.front();
		m_start_position.x = l1.p1.x + (l1.p2.x - l1.p1.x) * 0.5f;
		m_start_position.y = l1.p1.y + (l1.p2.y - l1.p1.y) * 0.5f;

		t_vec2f pos;
		t_line& l2 = m_checkpoints[1];
		pos.x = l2.p1.x + (l2.p2.x - l2.p1.x) * 0.5f;
		pos.y = l2.p1.y + (l2.p2.y - l2.p1.y) * 0.5f;

		// D_MYLOG("m_start_position.x=" << m_start_position.x);
		// D_MYLOG("m_start_position.y=" << m_start_position.y);
		// D_MYLOG("pos.x=" << pos.x);
		// D_MYLOG("pos.y=" << pos.y);

		m_start_angle = atan2f(pos.y - m_start_position.y, pos.x - m_start_position.x);

		// D_MYLOG("m_start_angle=" << m_start_angle);
	}


	return true;
}

// CIRCUIT
//
//
//













//
//
//
// CAR

Car::Car()
	:	m_angle(0.0f),
		m_fitness(0.0f),
		m_alive(true),
		m_total_updates(0)
{
	updateSensors();
}

void	Car::update(float step, const Circuit& circuit, const NeuralNetwork& in_NN)
{
	if (std::isnan(m_position.x) ||
		std::isnan(m_position.y) ||
		std::isnan(m_angle))
		m_alive = false;

	if (!m_alive)
		return;

	//

	// m_fitness += 0.01f;

	this->updateSensors();
	this->collideSensors(circuit);
	this->collideCheckpoints();
	this->collideWalls( circuit.getWalls() );

	// return;

	//


	std::vector<float>	input;
	std::vector<float>	output;

	for (t_sensor& sensor : m_sensors)
		input.push_back( sensor.m_value );

	in_NN.process(input, output);

	// float leftTheta		= M_PI/32.0f * output[0];
	// float rightTheta	= M_PI/32.0f * output[1];
	float leftTheta		= output[0];
	float rightTheta	= output[1];

	leftTheta = std::min<float>(M_PI/32.0f, std::max<float>(-M_PI/32.0f, leftTheta));
	rightTheta = std::min<float>(M_PI/32.0f, std::max<float>(-M_PI/32.0f, rightTheta));

	m_angle += (leftTheta - rightTheta) * step;

	// m_position.x += (k_speed * cosf(m_angle)) * step;
	// m_position.y += (k_speed * sinf(m_angle)) * step;

	float speed = ((fabs(leftTheta + rightTheta)) / 2) * 160.0f;

	speed = std::min(15.0f, std::max(10.0f, speed));


	t_vec2f	prev_pos = m_position;

	m_position.x += (speed * cosf(m_angle)) * step;
	m_position.y += (speed * sinf(m_angle)) * step;


	m_trail.push_back( t_line(prev_pos, m_position) );

	// m_fitness += speed * 0.01f;
	++m_total_updates;
}

void	Car::updateSensors()
{
	// initialise

	static const float angles[5] = {
		-M_PI/2.0f,
		-M_PI/4.0f,
		0.0f,
		M_PI/4.0f,
		M_PI/2.0f
	};

	for (unsigned int i = 0; i < m_sensors.size(); ++i)
	{
		m_sensors[i].m_line.p1 = t_vec2f();
		m_sensors[i].m_line.p2.x = 100.0f * cosf(angles[i]);
		m_sensors[i].m_line.p2.y = 100.0f * sinf(angles[i]);

		m_sensors[i].m_value = 1.0f;
	}

	// translate/rotate

	for (t_sensor& sensor : m_sensors)
	{
		sensor.m_line.p1.x += m_position.x;
		sensor.m_line.p2.x += m_position.x;
		sensor.m_line.p1.y += m_position.y;
		sensor.m_line.p2.y += m_position.y;

		sensor.m_line.p1 = rotateVec2(sensor.m_line.p1, m_position, m_angle);
		sensor.m_line.p2 = rotateVec2(sensor.m_line.p2, m_position, m_angle);
	}
}

void	Car::collideSensors(const Circuit& circuit)
{
	const t_lines&	walls = circuit.getWalls();

	for (t_sensor& sensor : m_sensors)
		for (const t_line& wall : walls)
		{
			float v = CollisionSegSeg_partial(sensor.m_line.p1, sensor.m_line.p2, wall.p1, wall.p2);

			if (v >= 0.0f && v < 1.0f)
				sensor.m_value = v;
		}
}

void	Car::collideCheckpoints()
{
	t_lines::iterator it = m_current_checkpoints.begin();
	while (it != m_current_checkpoints.end())
	{
		if (CollisionSegmentCercle(it->p1, it->p2, m_position, 5.0f))
		{
			it = m_current_checkpoints.erase(it);
			++m_fitness;
		}
		else
		{
			++it;
		}
	}

	if (m_current_checkpoints.empty())
	{
		// this line reward a faster car once the circuit is completed
		m_fitness += (1000.0f / m_total_updates);

		m_alive = false;
	}
}

void	Car::collideWalls(const t_lines& walls)
{
	for (const t_line& l : walls)
		if (CollisionSegmentCercle(l.p1, l.p2, m_position, 10.0f))
		{
			m_alive = false;
			break;
		}
}

void	Car::revive()
{
	m_alive = true;
	m_fitness = 0;
	m_total_updates = 0;
	m_trail.clear();
}

// CAR
//
//
//













//
//
//
// SIMULATION

Simulation::Simulation(const std::string& filename)
	// :	m_pNNTopology(NULL)
{
	m_Circuit.loadMap(filename);

	// at that point the geneticAlgo class constructor
	// is already called -> so the genomes are usable

	m_Cars.resize( m_genAlgo.getGenomes().size() );

	for (unsigned int i = 0; i < m_Cars.size(); ++i)
	{
		Car& car = m_Cars[i];
		car.setCheckpoints( m_Circuit.getCheckpoints() );

		car.setPosition( m_Circuit.getStartingPositon() );
		car.setAngle( m_Circuit.getStartingAngle() );
	}
}

void	Simulation::update(float step)
{
	bool	someone_is_alive = false;

	#pragma omp parallel num_threads(3)
	{
		// This code will be executed by three threads.

		// Chunks of this loop will be divided amongst
		// the (three) threads of the current team.
		#pragma omp for
		for (unsigned int i = 0; i < m_Cars.size(); ++i)
		{
			if (!m_Cars[i].isAlive())
				continue;

			someone_is_alive = true;

			m_Cars[i].update(step, m_Circuit, m_genAlgo.getNNetworks()[i]);
		}
	}

	if (someone_is_alive)
		return;



	// // pick the best car
	// unsigned int bestIndex = 0;
	// for (unsigned int i = 1; i < m_Cars.size(); ++i)
	// 	if (m_Cars[i].getFitness() > m_Cars[bestIndex].getFitness())
	// 		bestIndex = i;

	// D_MYLOG("next generation, total updates => " << m_Cars[bestIndex].getTotalUpdates());


	// if (m_genAlgo.getCurrentGeneration() >= 1000)
	// 	exit(0);

	// D_MYLOG("next generation, current generation => " << m_genAlgo.getCurrentGeneration());
	// D_MYLOG("next generation, current fitness => " << m_genAlgo.getBestFitness());

	// rate genomes
	for (unsigned int i = 0; i < m_Cars.size(); ++i)
		m_genAlgo.rateGenome(i, m_Cars[i].getFitness());

	m_genAlgo.BreedPopulation();

	if (m_genAlgo.isAGreatGeneration())
	{
		Car& car = m_Cars[ m_genAlgo.getAlpha().m_index ];

		m_trails.push_back( car.getTrail() );

		while (m_trails.size() > 5)
			m_trails.erase( m_trails.begin() );
	}

	for (Car& car : m_Cars)
	{
		// restart the car
		car.setPosition( m_Circuit.getStartingPositon() );
		car.setAngle( m_Circuit.getStartingAngle() );
		car.setCheckpoints( m_Circuit.getCheckpoints() );
		car.revive();
	}
}

// SIMULATION
//
//
//








