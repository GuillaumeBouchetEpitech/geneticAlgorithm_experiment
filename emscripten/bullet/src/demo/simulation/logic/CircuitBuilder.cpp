

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

void CircuitBuilder::load(const std::string& filename)
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

    t_knots rawKnots;
    glm::vec3 currentColor = { 0.0f, 0.0f, 0.0f };

    std::string textLine;
    while (std::getline(fileStream, textLine))
    {
        // trim extracted line
        StringUtils::trim(textLine);

         // skip empty line(s) and comment(s)
        if (textLine.empty() || textLine[0] == '#')
            continue;

        std::istringstream isstr(textLine);

        std::string type;
        if (!(isstr >> type))
            D_THROW(std::runtime_error, "failure to extract the line type");

        if (type == "START_TRANSFORM_POSITION")
        {
            auto& value = _startTransform.position;
            if (!(isstr >> value.x >> value.y >> value.z))
                D_THROW(std::runtime_error, "failure to extract a line, type=" << type);

            for (int ii = 0; ii < 3; ++ii)
            {
                if (glm::isnan(value[ii]))
                    D_THROW(std::runtime_error, "invalid value (NaN), type=" << type);

                if (glm::isinf(value[ii]))
                    D_THROW(std::runtime_error, "invalid value (inf), type=" << type);
            }
        }
        else if (type == "START_TRANSFORM_QUATERNION")
        {
            auto& value = _startTransform.quaternion;
            if (!(isstr >> value.x >> value.y >> value.z >> value.w))
                D_THROW(std::runtime_error, "failure to extract a line, type=" << type);

            for (int ii = 0; ii < 4; ++ii)
            {
                if (glm::isnan(value[ii]))
                    D_THROW(std::runtime_error, "invalid value (NaN), type=" << type);

                if (glm::isinf(value[ii]))
                    D_THROW(std::runtime_error, "invalid value (inf), type=" << type);
            }
        }
        else if (type == "KNOTS_COLOR")
        {
            glm::vec3 color;

            if (!(isstr >> color.x >> color.y >> color.z))
                D_THROW(std::runtime_error, "failure to extract a line, type=" << type);

            for (int ii = 0; ii < 3; ++ii)
            {
                if (glm::isnan(color[ii]))
                    D_THROW(std::runtime_error, "invalid value (NaN), type=" << type);

                if (glm::isinf(color[ii]))
                    D_THROW(std::runtime_error, "invalid value (inf), type=" << type);
            }

            currentColor = color;
        }
        else if (type == "KNOTS_DUAL")
        {
            glm::vec3 left;
            glm::vec3 right;

            if (!(isstr >> left.x >> left.y >> left.z >> right.x >> right.y >> right.z))
                D_THROW(std::runtime_error, "failure to extract a line, type=" << type);

            for (int ii = 0; ii < 3; ++ii)
            {
                if (glm::isnan(left[ii]) || glm::isnan(right[ii]))
                    D_THROW(std::runtime_error, "invalid value (NaN), type=" << type);

                if (glm::isinf(left[ii]) || glm::isinf(right[ii]))
                    D_THROW(std::runtime_error, "invalid value (inf), type=" << type);
            }

            rawKnots.push_back({ left, right, currentColor });
        }
        else
        {
            D_THROW(std::runtime_error, "unknown line type, type=" << type);
        }
    }

    //
    //
    // concatenate skeleton

    for (unsigned int ii = 0; ii < rawKnots.size(); ++ii)
    {
        auto knot = rawKnots[ii];

        if (ii > 0)
        {
            const auto& prevKnot = _knots[ii - 1];

            // concatenate the knot
            knot.left += prevKnot.left;
            knot.right += prevKnot.right;
        }

        _knots.push_back(knot);
    }
}

//
//

void CircuitBuilder::generateSkeleton(t_callbackNoNormals onSkeletonPatch)
{
    if (!onSkeletonPatch)
        D_THROW(std::invalid_argument, "no callback provided");

    if (_knots.empty())
        D_THROW(std::runtime_error, "not initialised");

    t_vec3Array vertices;
    t_vec3Array colors;
    t_indices indices;

    vertices.reserve(_knots.size() * 4);
    indices.reserve(_knots.size() * 8 + 8);

    for (unsigned int ii = 0; ii < _knots.size(); ++ii)
    {
        const auto& knot = _knots[ii];

        // "real" value
        vertices.push_back(knot.left);
        vertices.push_back(knot.right);
        // "on the floor" value
        vertices.push_back({ knot.left.x, knot.left.y, 0.0f });
        vertices.push_back({ knot.right.x, knot.right.y, 0.0f });

        // from "the floor" to the "real" value

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

        // from the "previous" to the "current" value

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

    onSkeletonPatch(vertices, indices);
}

void CircuitBuilder::generate(t_callbackNormals onNewGroundPatch,
                                 t_callbackNormals onNewWallPatch)
{
    if (_knots.empty())
        D_THROW(std::runtime_error, "not initialised");

    if (!onNewGroundPatch || !onNewWallPatch)
        D_THROW(std::invalid_argument, "no callbacks provided");

    //
    //
    // smooth the circuit

    t_knots smoothedVertices;
    smoothedVertices.reserve(512); // <= ease the reallocation

    {
        const unsigned int  dimension = 9; // <= 3 * vec3 = 9 (float)
        const unsigned int  degree = 3;
        const float*        knotsData = &_knots.front().left.x; // <= float*
        const std::size_t   knotsLength = _knots.size() * dimension; // <= float*

        BSpline smootherBSpline;
        smootherBSpline.initialise({ knotsData, knotsLength, dimension, degree });

        CircuitBuilder::t_circuitVertex vertex;

        const unsigned int maxIterations = 1000;
        const float step = 1.0f / maxIterations; // tiny steps
        const float minimumDistance = 2.0f;

        for (float coef = 0.0f; coef <= 1.0f; coef += step) // tiny steps
        {
            vertex.left.x = smootherBSpline.calcAt(coef, 0); // left x
            vertex.left.y = smootherBSpline.calcAt(coef, 1); // left y
            vertex.left.z = smootherBSpline.calcAt(coef, 2); // left z
            vertex.right.x = smootherBSpline.calcAt(coef, 3); // right x
            vertex.right.y = smootherBSpline.calcAt(coef, 4); // right y
            vertex.right.z = smootherBSpline.calcAt(coef, 5); // right z

            if (!smoothedVertices.empty())
            {
                const auto& lastVertex = smoothedVertices.back();
                if (glm::length(vertex.left - lastVertex.left) < minimumDistance ||
                    glm::length(vertex.right - lastVertex.right) < minimumDistance)
                    continue;
            }

            // check for invalid values (it create graphic and physic artifacts)
            if (glm::length(vertex.left) < 0.001f ||
                glm::length(vertex.right) < 0.001f)
                continue; // TODO: fix it

            vertex.color.x = smootherBSpline.calcAt(coef, 6); // r
            vertex.color.y = smootherBSpline.calcAt(coef, 7); // g
            vertex.color.z = smootherBSpline.calcAt(coef, 8); // b

            smoothedVertices.push_back(vertex);
        }
    }

    //
    //
    // generate circuit

    const int patchesPerKnot = 6;

    glm::vec3 prevNormal;

    for (unsigned int index = 1; index < smoothedVertices.size(); index += patchesPerKnot)
    {
        t_indices indices;

        struct t_circuitPatchData
        {
            t_vec3Array vertices;
            t_vec3Array normals;
        }
        ground,
        leftWall,
        rightWall;

        t_vec3Array circuitPatchColors;

        //
        //
        // prepare the vertices

        int indicexIndex = 0;

        for (unsigned int stepIndex = index;
             stepIndex < smoothedVertices.size() &&
             stepIndex < index + patchesPerKnot;
             ++stepIndex)
        {
            const int currIndex = indicexIndex++ * 4;
            indices.push_back(0 + currIndex);
            indices.push_back(1 + currIndex);
            indices.push_back(2 + currIndex);
            indices.push_back(0 + currIndex);
            indices.push_back(3 + currIndex);
            indices.push_back(2 + currIndex);

            const auto& prevKnot = smoothedVertices[stepIndex - 1];
            const auto& currKnot = smoothedVertices[stepIndex];

            const glm::vec3& prevLeft = prevKnot.left;
            const glm::vec3& prevRight = prevKnot.right;
            const glm::vec3& currLeft = currKnot.left;
            const glm::vec3& currRight = currKnot.right;

            const glm::vec3& prevColor = prevKnot.color;
            const glm::vec3& currColor = currKnot.color;

            glm::vec3 currNormal = glm::normalize(glm::cross(prevLeft - prevRight, prevRight - currRight));

            if (stepIndex == 1) // <= for the first time
                prevNormal = currNormal;

            glm::vec3 prevNormalLeft(prevNormal.x, prevNormal.z, prevNormal.y);
            glm::vec3 currNormalLeft(currNormal.x, currNormal.z, currNormal.y);
            glm::vec3 prevNormalRight = -prevNormalLeft;
            glm::vec3 currNormalRight = -currNormalLeft;

            //

            ground.vertices.push_back(prevLeft);
            ground.vertices.push_back(prevRight);
            ground.vertices.push_back(currRight);
            ground.vertices.push_back(currLeft);

            circuitPatchColors.push_back(prevColor);
            circuitPatchColors.push_back(prevColor);
            circuitPatchColors.push_back(currColor);
            circuitPatchColors.push_back(currColor);

            ground.normals.push_back(prevNormal);
            ground.normals.push_back(prevNormal);
            ground.normals.push_back(currNormal);
            ground.normals.push_back(currNormal);

            //

            glm::vec3 prevHeight = prevNormal * 10.0f;
            glm::vec3 currHeight = currNormal * 10.0f;

            //

            glm::vec3 prevTopLeft = prevLeft + prevHeight;
            glm::vec3 currTopLeft = currLeft + currHeight;

            leftWall.vertices.push_back(prevLeft);
            leftWall.vertices.push_back(prevTopLeft);
            leftWall.vertices.push_back(currTopLeft);
            leftWall.vertices.push_back(currLeft);

            leftWall.normals.push_back(prevNormalLeft);
            leftWall.normals.push_back(prevNormalLeft);
            leftWall.normals.push_back(currNormalLeft);
            leftWall.normals.push_back(currNormalLeft);

            //

            glm::vec3 prevTopRight = prevRight + prevHeight;
            glm::vec3 currTopRight = currRight + currHeight;

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
            onNewGroundPatch(ground.vertices, circuitPatchColors, ground.normals, indices);

        if (onNewWallPatch)
        {
            onNewWallPatch(leftWall.vertices, circuitPatchColors, leftWall.normals, indices);
            onNewWallPatch(rightWall.vertices, circuitPatchColors, rightWall.normals, indices);
        }
    }
}

const CircuitBuilder::t_startTransform& CircuitBuilder::getStartTransform() const
{
    return _startTransform;
}
