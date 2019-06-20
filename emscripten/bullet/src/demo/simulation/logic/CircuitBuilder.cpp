

#include "CircuitBuilder.hpp"

#include "demo/utilities/ErrorHandler.hpp"
#include "demo/utilities/TraceLogger.hpp"
#include "demo/utilities/math/BSpline.hpp"
#include "demo/utilities/string/trim.hpp"

#include <stdexcept> // <= std::invalid_argument / runtime_error
#include <algorithm>
#include <fstream>
#include <sstream>

#include <cmath>

//
//

void	CircuitBuilder::load(const std::string& filename)
{
	//
	//
	// open/read file

	std::ifstream fileStream(filename);

	if (!fileStream.is_open())
		D_THROW(std::runtime_error, "fail to read a file, name=" << filename);

	//
	//
	// parse file and extract skeleton

	t_checkpoints	rawCheckpoints;
	glm::vec3		currentColor = { 0.0f, 0.0f, 0.0f };

	std::string textLine;
	while (std::getline(fileStream, textLine))
	{
		// trim extracted line
		StringUtils::trim(textLine);

		 // skip empty line(s) and comment(s)
		if (textLine.empty() || textLine[0] == '#')
			continue;

		std::istringstream	isstr(textLine);

		std::string type;
		if (!(isstr >> type))
		{
			D_THROW(std::runtime_error, "failure to extract the line type");
		}

		if (type == "CHECKPOINT")
		{
			glm::vec3	left;
			glm::vec3	right;

			if (!(isstr >> left.x >> left.y >> left.z >> right.x >> right.y >> right.z))
				D_THROW(std::runtime_error, "failure to extract a line, type=" << type);

			for (int ii = 0; ii < 3; ++ii)
			{
				if (glm::isnan(left[ii]) || glm::isnan(right[ii]))
					D_THROW(std::runtime_error, "extracted invalid value (NaN), type=" << type);

				if (glm::isinf(left[ii]) || glm::isinf(right[ii]))
					D_THROW(std::runtime_error, "extracted invalid value (inf), type=" << type);
			}

			rawCheckpoints.push_back({ left, right, currentColor });
		}
		else if (type == "COLOR")
		{
			glm::vec3	color;

			if (!(isstr >> color.x >> color.y >> color.z))
				D_THROW(std::runtime_error, "failure to extract a line, type=" << type);

			for (int ii = 0; ii < 3; ++ii)
			{
				if (glm::isnan(color[ii]))
					D_THROW(std::runtime_error, "extracted invalid value (NaN), type=" << type);

				if (glm::isinf(color[ii]))
					D_THROW(std::runtime_error, "extracted invalid value (inf), type=" << type);
			}

			currentColor = color;
		}
		else
		{
			D_THROW(std::runtime_error, "unknown line type, type=" << type);
		}
	}

	//
	//
	// concatenate skeleton

	for (unsigned int ii = 0; ii < rawCheckpoints.size(); ++ii)
	{
		auto checkpoint = rawCheckpoints[ii];

		if (ii > 0)
		{
			const auto& prevCheckpoint = _checkpoints[ii - 1];

			// concatenate the checkpoint
			checkpoint.left += prevCheckpoint.left;
			checkpoint.right += prevCheckpoint.right;
		}

		_checkpoints.push_back(checkpoint);
	}
}

//
//

void	CircuitBuilder::generateSkeleton(t_callbackNoNormals onSkeletonPatch)
{
	if (!onSkeletonPatch)
		D_THROW(std::invalid_argument, "no callback provided");

	if (_checkpoints.empty())
		D_THROW(std::runtime_error, "not initialised");

	t_vertices	vertices;
	t_colors	colors;
	t_indices	indices;

	vertices.reserve(_checkpoints.size() * 4);
	indices.reserve(_checkpoints.size() * 8 + 8);

	for (unsigned int ii = 0; ii < _checkpoints.size(); ++ii)
	{
		const auto& checkpoint = _checkpoints[ii];

		vertices.push_back(checkpoint.left);
		vertices.push_back(checkpoint.right);
		vertices.push_back({ checkpoint.left.x, checkpoint.left.y, 0.0f });
		vertices.push_back({ checkpoint.right.x, checkpoint.right.y, 0.0f });

		const int currIndex = ii * 4;

		indices.push_back(currIndex + 0); // upper 0 left-right
		indices.push_back(currIndex + 1);

		indices.push_back(currIndex + 2); // lower 0 left-right
		indices.push_back(currIndex + 3);

		indices.push_back(currIndex + 0); // lower-upper 0 left
		indices.push_back(currIndex + 2);

		indices.push_back(currIndex + 1); // lower-upper 0 right
		indices.push_back(currIndex + 3);

		if (ii == 0)
			continue;

		const int prevIndex = (ii - 1) * 4;

		indices.push_back(currIndex + 0); // upper 0-1 left
		indices.push_back(prevIndex + 0);

		indices.push_back(currIndex + 1); // upper 0-1 right
		indices.push_back(prevIndex + 1);

		indices.push_back(currIndex + 2); // lower 0-1 left
		indices.push_back(prevIndex + 2);

		indices.push_back(currIndex + 3); // lower 0-1 right
		indices.push_back(prevIndex + 3);
	}

#undef D_PUSH_VERTEX

	onSkeletonPatch(vertices, indices);
}

// void	CircuitBuilder::generate(t_callbackNormals2 onNewGroundPatch, t_callbackNormals onNewWallPatch)
void	CircuitBuilder::generate(t_callbackNormals onNewGroundPatch, t_callbackNormals onNewWallPatch)
{
	if (_checkpoints.empty())
		D_THROW(std::runtime_error, "not initialised");

	if (!onNewGroundPatch || !onNewWallPatch)
		D_THROW(std::invalid_argument, "no callbacks provided");

	//
	//
	// smooth the circuit

	t_checkpoints smoothedVertices;

	std::vector<float>	left_x;
	std::vector<float>	left_y;
	std::vector<float>	left_z;
	std::vector<float>	right_x;
	std::vector<float>	right_y;
	std::vector<float>	right_z;
	std::vector<float>	color_x;
	std::vector<float>	color_y;
	std::vector<float>	color_z;

	left_x.reserve(_checkpoints.size());
	left_y.reserve(_checkpoints.size());
	left_z.reserve(_checkpoints.size());
	right_x.reserve(_checkpoints.size());
	right_y.reserve(_checkpoints.size());
	right_z.reserve(_checkpoints.size());
	color_x.reserve(_checkpoints.size());
	color_y.reserve(_checkpoints.size());
	color_z.reserve(_checkpoints.size());

	for (const auto& checkpoint : _checkpoints)
	{
		left_x.push_back(checkpoint.left.x);
		left_y.push_back(checkpoint.left.y);
		left_z.push_back(checkpoint.left.z);
		right_x.push_back(checkpoint.right.x);
		right_y.push_back(checkpoint.right.y);
		right_z.push_back(checkpoint.right.z);
		color_x.push_back(checkpoint.color.x);
		color_y.push_back(checkpoint.color.y);
		color_z.push_back(checkpoint.color.z);
	}

	struct t_BSpline3
	{
		BSpline	x, y, z;

		t_BSpline3(const std::vector<float>& pointsX,
				   const std::vector<float>& pointsY,
				   const std::vector<float>& pointsZ)
			: x(pointsX, 1, 3)
			, y(pointsY, 1, 3)
			, z(pointsZ, 1, 3)
		{}

		void calcAt(float step, glm::vec3& position)
		{
			x.calcAt(step, &position.x);
			y.calcAt(step, &position.y);
			z.calcAt(step, &position.z);
		}
	}
	splineLeft(left_x, left_y, left_z),
	splineRight(right_x, right_y, right_z),
	splineColor(color_x, color_y, color_z);

	CircuitBuilder::t_checkpoint	checkpoint;

	for (float step = 0.0f; step <= 1.0f; step += 0.001f) // tiny steps
	{

		splineLeft.calcAt(step, checkpoint.left);
		splineRight.calcAt(step, checkpoint.right);

		if (!smoothedVertices.empty())
		{
			const auto& latestVertex = smoothedVertices.back();
			if (glm::length(checkpoint.left - latestVertex.left) < 2.0f ||
			 	glm::length(checkpoint.right - latestVertex.right) < 2.0f)
				continue;
		}

		// check for invalid values (it create graphic and physic artifacts)
		if (glm::length(checkpoint.left) < 0.001f ||
			glm::length(checkpoint.right) < 0.001f)
			continue; // TODO: fix it

		splineColor.calcAt(step, checkpoint.color);

		smoothedVertices.push_back(checkpoint);
	}

	//
	//
	// generate circuit

	const int patchesPerCheckpoint = 6;

	t_vertex	prevNormal;

	for (unsigned int index = 1; index < smoothedVertices.size(); index += patchesPerCheckpoint)
	{
		t_indices	indices;

		struct t_checkpointData
		{
			t_vertices	vertices;
			t_normals	normals;
		}
		ground,
		leftWall,
		rightWall;

		t_colors	checkpointColors;

		//
		//
		// prepare the vertices

		int indicexIndex = 0;

		for (unsigned int stepIndex = index;
			 stepIndex < smoothedVertices.size() &&
			 stepIndex < index + patchesPerCheckpoint;
			 ++stepIndex)
		{
			const int currIndex = indicexIndex++ * 4;
			indices.push_back(0 + currIndex);
			indices.push_back(1 + currIndex);
			indices.push_back(2 + currIndex);
			indices.push_back(0 + currIndex);
			indices.push_back(3 + currIndex);
			indices.push_back(2 + currIndex);

			const auto& prevCheckpoint = smoothedVertices[stepIndex - 1];
			const auto& currCheckpoint = smoothedVertices[stepIndex];

			const t_vertex&	prevLeft = prevCheckpoint.left;
			const t_vertex&	prevRight = prevCheckpoint.right;
			const t_vertex&	currLeft = currCheckpoint.left;
			const t_vertex&	currRight = currCheckpoint.right;

			const glm::vec3&	prevColor = prevCheckpoint.color;
			const glm::vec3&	currColor = currCheckpoint.color;

		    t_vertex currNormal = glm::normalize(glm::cross(prevLeft - prevRight, currRight - prevRight));

			if (stepIndex == 1) // <= for the first time
				prevNormal = currNormal;

			t_vertex	prevNormalLeft(prevNormal.x, prevNormal.z, prevNormal.y);
			t_vertex	currNormalLeft(currNormal.x, currNormal.z, currNormal.y);
			t_vertex	prevNormalRight = -prevNormalLeft;
			t_vertex	currNormalRight = -currNormalLeft;

			//

			ground.vertices.push_back(prevLeft);
			ground.vertices.push_back(prevRight);
			ground.vertices.push_back(currRight);
			ground.vertices.push_back(currLeft);

			checkpointColors.push_back(prevColor);
			checkpointColors.push_back(prevColor);
			checkpointColors.push_back(currColor);
			checkpointColors.push_back(currColor);

			ground.normals.push_back(prevNormal);
			ground.normals.push_back(prevNormal);
			ground.normals.push_back(currNormal);
			ground.normals.push_back(currNormal);

			//

			t_vertex	prevHeight = prevNormal * 10.0f;
			t_vertex	currHeight = currNormal * 10.0f;

			//

			t_vertex	prevTopLeft = prevLeft + prevHeight;
			t_vertex	currTopLeft = currLeft + currHeight;

			leftWall.vertices.push_back(prevLeft);
			leftWall.vertices.push_back(prevTopLeft);
			leftWall.vertices.push_back(currTopLeft);
			leftWall.vertices.push_back(currLeft);

			leftWall.normals.push_back(prevNormalLeft);
			leftWall.normals.push_back(prevNormalLeft);
			leftWall.normals.push_back(currNormalLeft);
			leftWall.normals.push_back(currNormalLeft);

			//

			t_vertex	prevTopRight = prevRight + prevHeight;
			t_vertex	currTopRight = currRight + currHeight;

			rightWall.vertices.push_back(prevRight);
			rightWall.vertices.push_back(prevTopRight);
			rightWall.vertices.push_back(currTopRight);
			rightWall.vertices.push_back(currRight);

			rightWall.normals.push_back(prevNormalRight);
			rightWall.normals.push_back(prevNormalRight);
			rightWall.normals.push_back(currNormalRight);
			rightWall.normals.push_back(currNormalRight);

			//

			prevNormal = currNormal;
		}

		if (onNewGroundPatch)
			onNewGroundPatch(ground.vertices, checkpointColors, ground.normals, indices);

		if (onNewWallPatch)
		{
			onNewWallPatch(leftWall.vertices, checkpointColors, leftWall.normals, indices);
			onNewWallPatch(rightWall.vertices, checkpointColors, rightWall.normals, indices);
		}
	}
}
