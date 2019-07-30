
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

	struct t_startTransform
	{
		glm::vec3 position;
		glm::vec4 quaternion;
	};

public:
	typedef std::function<void(const t_vertices& vertices,
							   const t_indices& indices)> t_callbackNoNormals;

	typedef std::function<void(const t_vertices& vertices,
							   const t_colors& colors,
							   const t_normals& normals,
							   const t_indices& indices)> t_callbackNormals;

private:

	struct t_knot
	{
		glm::vec3 left;
		glm::vec3 right;
		glm::vec3 color;
	};
	typedef std::vector<t_knot>	t_knots;
	typedef t_knot				t_circuitVertex;

private:
	t_startTransform	_startTransform;
	t_knots				_knots;

public:
	void	load(const std::string& filename);

public:
	void	generateSkeleton(t_callbackNoNormals onSkeletonPatch);
	void	generate(t_callbackNormals onNewGroundPatch, t_callbackNormals onNewWallPatch);

public:
	const t_startTransform&	getStartTransform() const;

};
