

#include "Data.hpp"

#include "Graphic/Shader.hpp"

#include "Utility/TraceLogger.hpp"

#include "constants.hpp"

//
//
// singleton

Data*	Data::m_pInstance = nullptr;

Data::Data()
{
#define D_PUSH_VERTEX(container, v1,v2,v3, c1,c2,c3)	\
	(container).push_back(v1);	\
	(container).push_back(v2);	\
	(container).push_back(v3);	\
	(container).push_back(c1);	\
	(container).push_back(c2);	\
	(container).push_back(c3);	\

	{ // create the basic shader

		Shader::t_def	def;
	    def.filenames.vertex = "assets/shaders/color.glsl.vert.c";
	    def.filenames.fragment = "assets/shaders/color.glsl.frag.c";
		def.attributes.push_back("a_position");
		def.attributes.push_back("a_color");
		def.uniforms.push_back("u_composedMatrix");
		def.uniforms.push_back("u_alpha");

		graphic.shaders.pBasic = Shader::build(def);

		if (!graphic.shaders.pBasic)
			D_MYLOG("Failed to build the (basic) shader");
	}

	{ // create the instancing shader

		Shader::t_def	def;
		def.filenames.vertex = "assets/shaders/instancing.glsl.vert.c";
		def.filenames.fragment = "assets/shaders/instancing.glsl.frag.c";
		def.attributes.push_back("a_position");
		def.attributes.push_back("a_color");
		def.attributes.push_back("a_transform");
		def.uniforms.push_back("u_composedMatrix");

		graphic.shaders.pInstancing = Shader::build(def);

		if (!graphic.shaders.pInstancing)
			D_MYLOG("Failed to build the (instancing) shader");
	}

	graphic.stackRenderer.create();

	{
		auto& instanced = graphic.geometries.instanced;

		std::vector<float>  arr_buffer;

		{
			std::vector<float>	chassisBuffer;
			chassisBuffer.reserve(128);

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

	        for (int ii = 0; ii < 24; ++ii)
	        {
	            int	index = indices[ii] * 3;
	            float*	pVertex = &vertices[index];

	            D_PUSH_VERTEX(chassisBuffer, pVertex[0],pVertex[1],pVertex[2], 1,0,0);
	        }

			instanced.chassis.initialise();
			instanced.chassis.updateGeometry(chassisBuffer.data(), chassisBuffer.size());
		}

		{
			std::vector<float>	buffer_wheel;
			buffer_wheel.reserve(128);

			int quality = 16;
			float radius = 0.5f;
			float length = 0.5f;

			std::vector<float>	vertices;

	        for (int ii = 0; ii < quality; ++ii)
	        {
	            float ratio = static_cast<float>(ii) / quality;
	            vertices.push_back(-length / 2);
	            vertices.push_back(radius * cosf(M_PI * 2 * ratio));
	            vertices.push_back(radius * sinf(M_PI * 2 * ratio));
	        }

	        for (int ii = 0; ii < quality; ++ii)
	        {
	            float ratio = static_cast<float>(ii) / quality;
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
	        for (int ii = 0; ii < quality; ++ii)
	        {
	            indices.push_back(ii);
	            indices.push_back((ii + 1) % quality);
	        }

	        // wheel side 2
	        for (int ii = 0; ii < quality; ++ii)
	        {
	            indices.push_back(quality + ii);
	            indices.push_back(quality + (ii + 1) % quality);
	        }

	        indices.push_back(0);
	        indices.push_back(quality);

	        //

	        for (int ii = 0; ii < indices.size(); ++ii)
	        {
	            int index = indices[ii] * 3;
	            float*	pVertex = &vertices[index];

	            D_PUSH_VERTEX(buffer_wheel, pVertex[0],pVertex[1],pVertex[2], 1,1,0);
	        }

			instanced.wheels.initialise();
			instanced.wheels.updateGeometry(buffer_wheel.data(), buffer_wheel.size());
		}
	}

#undef D_PUSH_VERTEX

	logic.pSimulation = new Simulation();

	logic.carsTrails.allData.resize(D_CARS_NUMBER_TOTAL);
}

Data::~Data()
{
	graphic.stackRenderer.destroy();
}

void	Data::initialise()
{
    std::vector<float>   skeletonBuffer;
	std::vector<float>	groundBuffer;
	std::vector<float>	wallsBuffer;

    skeletonBuffer.reserve(512);
	groundBuffer.reserve(2048);
	wallsBuffer.reserve(2048*2);

	logic.pSimulation->initialise(
        D_CIRCUIT_FILENAME,
        [&](const std::vector<float>& vertices, const std::vector<int>& indices) -> void
        {
            for (int ii = 0; ii < indices.size(); ++ii)
            {
                int	index = indices[ii] * 3;
                const float*	pVertex = &vertices[index];

                skeletonBuffer.push_back(pVertex[0]);
                skeletonBuffer.push_back(pVertex[1]);
                skeletonBuffer.push_back(pVertex[2]);
                skeletonBuffer.push_back(0.6f);
                skeletonBuffer.push_back(0.6f);
                skeletonBuffer.push_back(0.6f);
            }
        },
        [&](const std::vector<float>& vertices, const std::vector<int>& indices) -> void
        {
            for (int ii = 0; ii < indices.size(); ++ii)
            {
                int	index = indices[ii] * 3;
                const float*	pVertex = &vertices[index];

                groundBuffer.push_back(pVertex[0]);
                groundBuffer.push_back(pVertex[1]);
                groundBuffer.push_back(pVertex[2]);

                // is it the first line? (for the white colored strip)
                if (index == 0 ||
                    index == 3)
                {
                    groundBuffer.push_back(1.0f);
                    groundBuffer.push_back(1.0f);
                    groundBuffer.push_back(1.0f);
                }
                else
                {
                    groundBuffer.push_back(0.0f);
                    groundBuffer.push_back(0.0f);
                    groundBuffer.push_back(1.0f);
                }
            }
        },
        [&](const std::vector<float>& vertices, const std::vector<int>& indices) -> void
        {
            for (int ii = 0; ii < indices.size(); ++ii)
            {
                int index = indices[ii] * 3;
                const float*	pVertex = &vertices[index];

                wallsBuffer.push_back(pVertex[0]);
                wallsBuffer.push_back(pVertex[1]);
                wallsBuffer.push_back(pVertex[2]);
                wallsBuffer.push_back(0.6f);
                wallsBuffer.push_back(0.6f);
                wallsBuffer.push_back(0.6f);
            }
        }

	);

	auto& basic = graphic.geometries.basic;

    basic.circuitSkelton.initialise();
	basic.circuitGround.initialise();
	basic.circuitWalls.initialise();

    basic.circuitSkelton.updateGeometry(skeletonBuffer.data(), skeletonBuffer.size());
	basic.circuitGround.updateGeometry(groundBuffer.data(), groundBuffer.size());
	basic.circuitWalls.updateGeometry(wallsBuffer.data(), wallsBuffer.size());
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
