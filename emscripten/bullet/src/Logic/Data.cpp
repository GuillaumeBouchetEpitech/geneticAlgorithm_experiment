

#include "Data.hpp"

#include "Utility/TraceLogger.hpp"

//
//
// singleton

Data*	Data::m_pInstance = nullptr;

Data::Data()
{

	//
	//
	// experimental

#define D_PUSH_VERTEX(container, v1,v2,v3, c1,c2,c3)	\
	(container).push_back(v1);	\
	(container).push_back(v2);	\
	(container).push_back(v3);	\
	(container).push_back(c1);	\
	(container).push_back(c2);	\
	(container).push_back(c3);	\

	m_ShaderColor.initialise();
	GeometryColor::initialise(&m_ShaderColor);

	m_StackRenderer.create();

	{
		m_graphic.m_InstShader.initialise();
		m_graphic.m_InstGeometry_chassis.initialise(&m_graphic.m_InstShader);
		m_graphic.m_InstGeometry_wheel.initialise(&m_graphic.m_InstShader);


		std::vector<float>  arr_buffer;

		{
			std::vector<float>	buffer_chassis;
			buffer_chassis.reserve(400);

	        float side_x = 1;
	        float side_y = 2;
	        float side_z = 0.5;

	        float vertices[] = {
	             side_x,  side_y,  side_z, // 0
	            -side_x,  side_y,  side_z,
	             side_x, -side_y,  side_z,
	            -side_x, -side_y,  side_z,

	             side_x,  side_y, -side_z, // 4
	            -side_x,  side_y, -side_z,
	             side_x, -side_y, -side_z,
	            -side_x, -side_y, -side_z
	        };

	        //

	        int indices[24] = {
	            0,1,  1,3,  3,2,  2,0,
	            4,5,  5,7,  7,6,  6,4,
	            0,4,  1,5,  3,7,  2,6
	        };

	        //

	        for (int i = 0; i < 24; ++i)
	        {
	            int curr_index = indices[i] * 3;

	            D_PUSH_VERTEX(buffer_chassis, vertices[curr_index+0],vertices[curr_index+1],vertices[curr_index+2], 1,0,0);
	        }

			m_graphic.m_InstGeometry_chassis.update(&buffer_chassis[0], buffer_chassis.size() * sizeof(float));
		}


		{
			std::vector<float>	buffer_wheel;
			buffer_wheel.reserve(400);

			int quality = 16;
			float radius = 0.5f;
			float length = 0.5f;

			std::vector<float>	vertices;

	        for (int i = 0; i < quality; ++i)
	        {
	            float ratio = static_cast<float>(i) / quality;
	            vertices.push_back(-length / 2);
	            vertices.push_back(radius * cosf(M_PI * 2 * ratio));
	            vertices.push_back(radius * sinf(M_PI * 2 * ratio));
	        }

	        for (int i = 0; i < quality; ++i)
	        {
	            float ratio = static_cast<float>(i) / quality;
	        	vertices.push_back(+length/2);
	            vertices.push_back(radius * cosf(M_PI * 2 * ratio));
	        	vertices.push_back(radius * sinf(M_PI * 2 * ratio));
	        }

	        vertices.push_back(0);
	        vertices.push_back(-length/2);
	        vertices.push_back(0);

	        vertices.push_back(0);
	        vertices.push_back(+length/2);
	        vertices.push_back(0);

	        //

			std::vector<int>	indices;

	        // wheel side 1
	        for (int i = 0; i < quality; ++i)
	        {
	            indices.push_back(i);
	            indices.push_back((i + 1) % quality);
	        }

	        // wheel side 2
	        for (int i = 0; i < quality; ++i)
	        {
	            indices.push_back(quality + i);
	            indices.push_back(quality + (i + 1) % quality);
	        }

	        // inter wheels
	        // for (int i = 0; i < quality; ++i)
	        // {
	        //     indices.push_back(i);
	        //     indices.push_back(quality+i);
	        // }
	        indices.push_back(0);
	        indices.push_back(quality);

	        //

	        for (int i = 0; i < indices.size(); ++i)
	        {
	            int curr_index = indices[i] * 3;

	            D_PUSH_VERTEX(buffer_wheel, vertices[curr_index+0],vertices[curr_index+1],vertices[curr_index+2], 1,1,0);
	        }

			// m_GeometryColor_wheel.initialise();
			// m_GeometryColor_wheel.update(&buffer_wheel[0], buffer_wheel.size() * sizeof(float));
			m_graphic.m_InstGeometry_wheel.update(&buffer_wheel[0], buffer_wheel.size() * sizeof(float));
		}
	}

#undef D_PUSH_VERTEX

	// /experimental
	//
	//

	//
	//
	//
	// world

	// m_DebugDraw.setDebugMode(
	// 	BulletDebugDraw::DBG_DrawWireframe |
	// 	BulletDebugDraw::DBG_DrawContactPoints |
	// 	BulletDebugDraw::DBG_NoHelpText |
	// 	BulletDebugDraw::DBG_DrawConstraints |
	// 	BulletDebugDraw::DBG_DrawConstraintLimits |
	// 	BulletDebugDraw::DBG_FastWireframe
	// );
	// m_PhysicWorld.setDebugDrawer(&m_DebugDraw);

	//
	//
	// simulation

	m_pPhysicWrapper = new PhysicWrapper();

	m_pSimulation = new Simulation(m_pPhysicWrapper);
}

Data::~Data()
{
	m_StackRenderer.destroy();
}

void	Data::initialise()
{
    std::vector<float>   buffer_skeleton;
    buffer_skeleton.reserve(400);

	std::vector<float>	buffer_ground;
	buffer_ground.reserve(2048);

	std::vector<float>	buffer_walls;
	buffer_walls.reserve(2048*2);

    int physic_index = 1;

    const std::string   str_filename_circuit = "assets/circuits/default.txt";

	m_pSimulation->initialise(
        str_filename_circuit,
        [&](const std::vector<float>& buf, const std::vector<int>& indices) -> void
        {
            for (int ii = 0; ii < indices.size(); ++ii)
            {
                int index = indices[ii] * 3;

                buffer_skeleton.push_back( buf[ index+0 ] );
                buffer_skeleton.push_back( buf[ index+1 ] );
                buffer_skeleton.push_back( buf[ index+2 ] );
                buffer_skeleton.push_back(0.6f);
                buffer_skeleton.push_back(0.6f);
                buffer_skeleton.push_back(0.6f);
            }
        },
        [&](const std::vector<float>& buf, const std::vector<int>& indices) -> void
        {
            m_pPhysicWrapper->createGround(buf, indices, physic_index++);

            for (int ii = 0; ii < indices.size(); ++ii)
            {
                int index = indices[ii] * 3;

                buffer_ground.push_back( buf[ index+0 ] );
                buffer_ground.push_back( buf[ index+1 ] );
                buffer_ground.push_back( buf[ index+2 ] );

                if (index == 0 ||
                    index == 3)
                {
                    buffer_ground.push_back(1.0f);
                    buffer_ground.push_back(1.0f);
                    buffer_ground.push_back(1.0f);
                }
                else
                {
                    buffer_ground.push_back(0.0f);
                    buffer_ground.push_back(0.0f);
                    buffer_ground.push_back(1.0f);
                }
            }
        },
        [&](const std::vector<float>& buf, const std::vector<int>& indices) -> void
        {
            m_pPhysicWrapper->createGhostWall(buf, indices);

            for (int ii = 0; ii < indices.size(); ++ii)
            {
                int index = indices[ii] * 3;

                buffer_walls.push_back( buf[ index+0 ] );
                buffer_walls.push_back( buf[ index+1 ] );
                buffer_walls.push_back( buf[ index+2 ] );
                buffer_walls.push_back(0.6f);
                buffer_walls.push_back(0.6f);
                buffer_walls.push_back(0.6f);
            }
        }

	);

    m_GeometryColor_circuit_skelton.initialise();
    m_GeometryColor_circuit_skelton.update(&buffer_skeleton[0], buffer_skeleton.size() * sizeof(float));

	m_GeometryColor_circuit_ground.initialise();
	m_GeometryColor_circuit_ground.update(&buffer_ground[0], buffer_ground.size() * sizeof(float));

	m_GeometryColor_circuit_walls.initialise();
	m_GeometryColor_circuit_walls.update(&buffer_walls[0], buffer_walls.size() * sizeof(float));
}

//

void	Data::create()
{
	if (!m_pInstance)
	{
		m_pInstance = new Data();
		m_pInstance->initialise();
	}
}

void	Data::destroy()
{
	delete m_pInstance, m_pInstance = nullptr;
}

Data*	Data::get()
{
	return m_pInstance;
}

// singleton
//
//
