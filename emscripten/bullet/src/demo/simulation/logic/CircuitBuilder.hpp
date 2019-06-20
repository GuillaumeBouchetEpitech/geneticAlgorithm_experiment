
#pragma once

#include "thirdparty/GLMath.hpp"

#include <string>
#include <vector>
#include <functional>

class CircuitBuilder
{
public:
	typedef glm::vec3				t_vertex;
	typedef std::vector<t_vertex>	t_vertices;
	typedef std::vector<glm::vec3>	t_colors;
	typedef std::vector<t_vertex>	t_normals;
	typedef std::vector<int>		t_indices;

public:
	typedef std::function<void(const t_vertices& vertices,
							   const t_indices& indices)> t_callbackNoNormals;

	typedef std::function<void(const t_vertices& vertices,
							   const t_colors& colors,
							   const t_normals& normals,
							   const t_indices& indices)> t_callbackNormals;

private:
	struct t_checkpoint
	{
		glm::vec3 left;
		glm::vec3 right;
		glm::vec3 color;
	};
	typedef std::vector<t_checkpoint>	t_checkpoints;

private:
	t_checkpoints	_checkpoints;

public:
	void	load(const std::string& filename);

public:
	void	generateSkeleton(t_callbackNoNormals onSkeletonPatch);
	void	generate(t_callbackNormals onNewGroundPatch, t_callbackNormals onNewWallPatch);

};
