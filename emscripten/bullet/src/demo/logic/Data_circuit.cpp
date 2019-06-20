
#include "Data.hpp"

#include "demo/utilities/math/RandomNumberGenerator.hpp"

#include <limits> // <= std::numeric_limits<T>::max()

struct t_animatedVertex
{
	typedef glm::vec3	t_postion;
	typedef glm::vec3	t_color;
	typedef glm::vec3	t_normal;

	t_postion	postion;
	t_color		color;
	t_normal	normal;

	float		limitId;

	t_animatedVertex(const t_postion& p, const t_color& c, const t_normal& n, float l)
		: postion(p)
		, color(c)
		, normal(n)
		, limitId(l)
	{}
};
typedef std::vector<t_animatedVertex>	t_animatedVertices;

void	Data::initialiseCircuit()
{
	std::vector<glm::vec3>				skeletonVertices;
	t_animatedVertices	groundVertices;
	t_animatedVertices	wallsVertices;

	skeletonVertices.reserve(128);
	groundVertices.reserve(2048);
	wallsVertices.reserve(groundVertices.capacity() * 2);

 	const glm::vec3	greyColor = { 0.6f, 0.6f, 0.6f };
	const glm::vec3	whiteColor = { 1.0f, 1.0f, 1.0f };

	const float maxFloat = std::numeric_limits<float>::max();
	auto& boundaries = logic.circuitAnimation.boundaries;
	boundaries.min = glm::vec3(+maxFloat, +maxFloat, +maxFloat);
	boundaries.max = glm::vec3(-maxFloat, -maxFloat, -maxFloat);

	auto onSkeletonPatch = [&](const CircuitBuilder::t_vertices& vertices,
							   const CircuitBuilder::t_indices& indices) -> void {

		for (int index : indices)
		{
			const glm::vec3& vertex = vertices[index];

			skeletonVertices.push_back({ vertex.x, vertex.y, vertex.z });

			if (vertex.x < boundaries.min.x)	boundaries.min.x = vertex.x;
			if (vertex.y < boundaries.min.y)	boundaries.min.y = vertex.y;
			if (vertex.z < boundaries.min.z)	boundaries.min.z = vertex.z;

			if (vertex.x > boundaries.max.x)	boundaries.max.x = vertex.x;
			if (vertex.y > boundaries.max.y)	boundaries.max.y = vertex.y;
			if (vertex.z > boundaries.max.z)	boundaries.max.z = vertex.z;
		}
	};

	float	latestSize = 0;

	auto onGroundPatch = [&](const CircuitBuilder::t_vertices& vertices,
							 const CircuitBuilder::t_colors& colors,
							 const CircuitBuilder::t_normals& normals,
							 const CircuitBuilder::t_indices& indices) -> void {

		latestSize = float(groundVertices.size());

        float	limitValue = latestSize / indices.size();
        float	limitStep = 1.0f / indices.size();

		for (int index : indices)
		{
            limitValue += limitStep;

			bool	firstLine = (index < 2);

			const auto&	color = (firstLine ? whiteColor : colors[index]);

			glm::vec3	deformation = {
				t_RNG::getRangedValue(-0.2f, 0.2f),
				t_RNG::getRangedValue(-0.2f, 0.2f),
				t_RNG::getRangedValue(-0.2f, 0.2f)
			};

			glm::vec3	normal = (normals[index] + deformation) * 4.0f;

			groundVertices.push_back({ vertices[index], color, normal, limitValue });
		}

		logic.circuitAnimation.maxUpperValue += 1.0f;
	};

	auto onWallPatch = [&](const CircuitBuilder::t_vertices& vertices,
						   const CircuitBuilder::t_colors& colors,
						   const CircuitBuilder::t_normals& normals,
						   const CircuitBuilder::t_indices& indices) -> void {

		static_cast<void>(colors); // <= unused

        float	limitValue = latestSize / indices.size();
        float	limitStep = 1.0f / indices.size();

		for (int index : indices)
		{
            limitValue += limitStep;

			bool	firstLine = (index < 2);

			const auto&	color = (firstLine ? whiteColor : greyColor);

			glm::vec3	deformation = {
				t_RNG::getRangedValue(-0.2f, 0.2f),
				t_RNG::getRangedValue(-0.2f, 0.2f),
				t_RNG::getRangedValue(-0.2f, 0.2f)
			};

			glm::vec3	normal = (normals[index] + deformation) * 4.0f;

			wallsVertices.push_back({ vertices[index], color, normal, limitValue });
		}
	};

	unsigned int layerInput = 15;
	std::vector<unsigned int> layerHidden = { 10, 5 };
	unsigned int layerOutput = 2;
	bool useBiasNeuron = true;

	NeuralNetworkTopology topology;
	topology.init(layerInput, layerHidden, layerOutput, useBiasNeuron);



	AbstactSimulation::t_def simulationDef;
	simulationDef.filename = D_CIRCUIT_FILENAME;
    simulationDef.genomesPerCore = 30;
    simulationDef.totalCores = 3;
	simulationDef.neuralNetworkTopology = topology;
	simulationDef.onSkeletonPatch = onSkeletonPatch;
	simulationDef.onNewGroundPatch = onGroundPatch;
	simulationDef.onNewWallPatch = onWallPatch;

	logic.simulation->initialise(simulationDef);



	boundaries.center = (boundaries.min - boundaries.max) * 0.5f;

	//

	{
		auto& monoColor = graphic.geometries.monoColor;
		auto& shader = *graphic.shaders.monoColor;

		{
			Geometry::t_def::t_vbo vboGeometry;
			vboGeometry.attrs.push_back({ "a_position", Geometry::e_attrType::eVec3f });

			Geometry::t_def geomDef = { { vboGeometry }, GL_LINES };

			//

			monoColor.circuitSkelton.initialise(shader, geomDef);
			monoColor.circuitSkelton.updateBuffer(0, skeletonVertices);
			monoColor.circuitSkelton.setPrimitiveCount(skeletonVertices.size());
		}

		//

		{
			Geometry::t_def::t_vbo vboGeometry;
			vboGeometry.attrs.push_back({ "a_position", Geometry::e_attrType::eVec3f });

			Geometry::t_def geomDef = { { vboGeometry }, GL_LINE_STRIP };

			auto& bestCarsTrails = graphic.geometries.monoColor.bestCarsTrails;

			bestCarsTrails.resize(5);
			for (auto& geometry : bestCarsTrails)
				geometry.initialise(shader, geomDef);
		}

	}

	//

	{
		auto& animatedCircuit = graphic.geometries.animatedCircuit;
		auto& shader = *graphic.shaders.animatedCircuit;

		Geometry::t_def::t_vbo vboGeometry;
		vboGeometry.attrs.push_back({ "a_position", Geometry::e_attrType::eVec3f, 0 });
		vboGeometry.attrs.push_back({ "a_color", Geometry::e_attrType::eVec3f, 3 });
		vboGeometry.attrs.push_back({ "a_normal", Geometry::e_attrType::eVec3f, 6 });
		vboGeometry.attrs.push_back({ "a_index", Geometry::e_attrType::eFloat, 9 });

		Geometry::t_def geomDef = { { vboGeometry }, GL_TRIANGLES };

		//

		animatedCircuit.ground.initialise(shader, geomDef);
		animatedCircuit.ground.updateBuffer(0, groundVertices);
		animatedCircuit.ground.setPrimitiveCount(groundVertices.size());

		animatedCircuit.walls.initialise(shader, geomDef);
		animatedCircuit.walls.updateBuffer(0, wallsVertices);
		animatedCircuit.walls.setPrimitiveCount(wallsVertices.size());

		logic.circuitAnimation.maxPrimitiveCount = groundVertices.size();
	}
}