
#include "Data.hpp"

#include "graphic/wrappers/Shader.hpp"

#include "demo/defines.hpp"

void	Data::initialiseGeometries()
{
	{
		auto& shader = *graphic.shaders.instanced;

		Geometry::t_def::t_vbo vboGeometry;
		vboGeometry.attrs = { { "a_position", Geometry::e_attrType::eVec3f } };

		Geometry::t_def::t_vbo vboInstance;
		vboInstance.attrs = { { "a_transform", Geometry::e_attrType::eMat4f } };
		vboInstance.instanced = true;

		Geometry::t_def geomDef = { { vboGeometry, vboInstance }, GL_LINES };

		auto& instanced = graphic.geometries.instanced;

		{ // chassis geometry (instanced)

			glm::vec3 side{ 1.0f, 2.0f, 0.5f };

			std::array<glm::vec3, 24> vertices{{
				{ +side.x, +side.y, +side.z }, // 0 (top)
				{ -side.x, +side.y, +side.z },
				{ +side.x, -side.y, +side.z },
				{ -side.x, -side.y, +side.z },
				{ +side.x, +side.y, -side.z }, // 4 (bottom)
				{ -side.x, +side.y, -side.z },
				{ +side.x, -side.y, -side.z },
				{ -side.x, -side.y, -side.z }
			}};

			//

			std::array<int, 24> indices{{
				0,1,  1,3,  3,2,  2,0,
				4,5,  5,7,  7,6,  6,4,
				0,4,  1,5,  3,7,  2,6
			}};

			//

			std::vector<glm::vec3> chassisVertices;
			chassisVertices.reserve(24);

			for (int index : indices)
				chassisVertices.push_back(vertices[index]);

			instanced.chassis.initialise(shader, geomDef);
			instanced.chassis.updateBuffer(0, chassisVertices);
			instanced.chassis.setPrimitiveCount(chassisVertices.size());
		}

		{ // wheel geometry (instanced)

			const int	wheelQuality = 16;
			const float	wheelRadius = 0.5f;
			const float	wheelHalfWidth = 0.25f;

			std::vector<glm::vec3> vertices;
			vertices.reserve(wheelQuality * 2 + 2);

			for (int ii = 0; ii < wheelQuality; ++ii)
			{
				float coef = float(ii) / wheelQuality;
				vertices.push_back({
					-wheelHalfWidth,
					wheelRadius * cosf(M_PI * 2.0f * coef),
					wheelRadius * sinf(M_PI * 2.0f * coef)
				});
			}

			for (int ii = 0; ii < wheelQuality; ++ii)
			{
				float coef = float(ii) / wheelQuality;
				vertices.push_back({
					+wheelHalfWidth,
					wheelRadius * cosf(M_PI * 2.0f * coef),
					wheelRadius * sinf(M_PI * 2.0f * coef)
				});
			}

			vertices.push_back(vertices[0]);
			vertices.push_back(vertices[wheelQuality]);

			//

			std::vector<int> indices;
			indices.reserve(wheelQuality * 2 + 2);

			// wheel side 1
			for (int ii = 0; ii < wheelQuality; ++ii)
			{
				indices.push_back(ii);
				indices.push_back((ii + 1) % wheelQuality);
			}

			// wheel side 2
			for (int ii = 0; ii < wheelQuality; ++ii)
			{
				indices.push_back(wheelQuality + ii);
				indices.push_back(wheelQuality + (ii + 1) % wheelQuality);
			}

			// wheel bridge
			indices.push_back(0);
			indices.push_back(wheelQuality);

			//

			std::vector<glm::vec3> wheelVertices;
			wheelVertices.reserve(indices.size());

			for (int index : indices)
				wheelVertices.push_back(vertices[index]);

			instanced.wheels.initialise(shader, geomDef);
			instanced.wheels.updateBuffer(0, wheelVertices);
			instanced.wheels.setPrimitiveCount(wheelVertices.size());
		}
	}

	{
		auto& shader = *graphic.shaders.basic;
		auto& geometry = graphic.geometries.basic.stackRenderer;

		Geometry::t_def::t_vbo vboGeometry;
		vboGeometry.attrs.push_back({ "a_position", Geometry::e_attrType::eVec3f, 0 });
		vboGeometry.attrs.push_back({ "a_color", Geometry::e_attrType::eVec3f, 3 });

		Geometry::t_def geomDef = { { vboGeometry }, GL_LINES };

		geometry.initialise(shader, geomDef);
		geometry.setPrimitiveCount(0);
	}

	{
		auto& shader = *graphic.shaders.hudText;
		auto& geometry = graphic.geometries.hudText.letters;

		Geometry::t_def::t_vbo vboGeometry;
		vboGeometry.attrs = {
			{ "a_position", Geometry::e_attrType::eVec2f, 0 },
			{ "a_texCoord", Geometry::e_attrType::eVec2f, 2 }
		};

		Geometry::t_def::t_vbo vboInstance;
		vboInstance.attrs = {
			{ "a_offsetPosition", Geometry::e_attrType::eVec2f, 0 },
			{ "a_offsetTexCoord", Geometry::e_attrType::eVec2f, 2 },
			{ "a_offsetScale", Geometry::e_attrType::eFloat, 4 }
		};
		vboInstance.instanced = true;

		Geometry::t_def geomDef = { { vboGeometry, vboInstance }, GL_TRIANGLES };

		struct t_vertex
		{
			glm::vec2	position;
			glm::vec2	texCoord;
		};

		struct t_offset
		{
			glm::vec2	position;
			glm::vec2	texCoord;
			glm::vec2	scale;
		};

		const glm::vec2 textureSize = { 256, 96 };
		const glm::vec2 gridSize = { 16, 6 };
		const glm::vec2 letterSize = textureSize / gridSize;
		// const glm::vec2 letterHsize = letterSize * 0.5f;
		const glm::vec2 texCoord = letterSize / textureSize;

		std::array<t_vertex, 4> vertices{{
			// { { +letterHsize.x, -letterHsize.y }, { texCoord.x, texCoord.y } },
			// { { -letterHsize.x, -letterHsize.y }, {          0, texCoord.y } },
			// { { +letterHsize.x, +letterHsize.y }, { texCoord.x,          0 } },
			// { { -letterHsize.x, +letterHsize.y }, {          0,          0 } }
			{ { +letterSize.x,             0 }, { texCoord.x, texCoord.y } },
			{ {             0,             0 }, {          0, texCoord.y } },
			{ { +letterSize.x, +letterSize.y }, { texCoord.x,          0 } },
			{ {             0, +letterSize.y }, {          0,          0 } }
		}};

		std::array<int, 6> indices{{ 1,0,2,  1,3,2 }};

		std::vector<t_vertex> letterVertices;
		letterVertices.reserve(indices.size());

		for (int index : indices)
			letterVertices.push_back(vertices[index]);

		geometry.initialise(shader, geomDef);
		geometry.updateBuffer(0, letterVertices);
		geometry.setPrimitiveCount(letterVertices.size());
	}

}
