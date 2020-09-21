

#include "CircuitBuilder.hpp"

#include "demo/utilities/ErrorHandler.hpp"
#include "demo/utilities/TraceLogger.hpp"
#include "demo/utilities/math/BSpline.hpp"
#include "demo/utilities/string/trim.hpp"
#include "demo/utilities/types.hpp"

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

    float currentMinDistance = 2.0f;
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
        else if (type == "KNOTS_MINIMUM_DSTANCE")
        {
            float minimumDistance;

            if (!(isstr >> minimumDistance))
                D_THROW(std::runtime_error, "failure to extract a line, type=" << type);

            if (glm::isnan(minimumDistance))
                D_THROW(std::runtime_error, "invalid value (NaN), type=" << type);

            if (glm::isinf(minimumDistance))
                D_THROW(std::runtime_error, "invalid value (inf), type=" << type);

            currentMinDistance = minimumDistance;
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

            rawKnots.emplace_back(left, right, currentMinDistance, currentColor);
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

void CircuitBuilder::load(const CircuitBuilder::t_startTransform& startTransform,
                          const CircuitBuilder::t_knots& knots)
{
    _startTransform = startTransform;
    _knots = knots;
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

    vertices.reserve(_knots.size() * 4); // pre-allocate
    indices.reserve(_knots.size() * 8 + 8); // pre-allocate

    for (unsigned int ii = 0; ii < _knots.size(); ++ii)
    {
        const auto& knot = _knots[ii];

        // "real" value
        vertices.push_back(knot.left);
        vertices.push_back(knot.right);
        // "on the floor" value
        vertices.emplace_back(knot.left.x, knot.left.y, 0.0f);
        vertices.emplace_back(knot.right.x, knot.right.y, 0.0f);

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
    smoothedVertices.reserve(2048); // pre-allocate, ease the reallocation

    {
        enum class SplineType: unsigned int
        {
            leftX = 0,
            leftY,
            leftZ,
            rightX,
            rightY,
            rightZ,
            minDistance,
            colorR,
            colorG,
            colorB,

            count,
        };

        const unsigned int  dimension = asValue(SplineType::count);

        const unsigned int  degree = 3;
        const float*        knotsData = &_knots.front().left.x;
        const std::size_t   knotsLength = _knots.size() * dimension;

        BSpline smoother;
        smoother.initialise({ knotsData, knotsLength, dimension, degree });

        CircuitBuilder::t_circuitVertex vertex;

        const unsigned int maxIterations = 1000;
        const float step = 1.0f / maxIterations; // tiny steps

        for (float coef = 0.0f; coef <= 1.0f; coef += step)
        {
            vertex.left.x = smoother.calcAt(coef, asValue(SplineType::leftX));
            vertex.left.y = smoother.calcAt(coef, asValue(SplineType::leftY));
            vertex.left.z = smoother.calcAt(coef, asValue(SplineType::leftZ));
            vertex.right.x = smoother.calcAt(coef, asValue(SplineType::rightX));
            vertex.right.y = smoother.calcAt(coef, asValue(SplineType::rightY));
            vertex.right.z = smoother.calcAt(coef, asValue(SplineType::rightZ));

            float minDistance = smoother.calcAt(coef, asValue(SplineType::minDistance));

            if (!smoothedVertices.empty())
            {
                // both left and right vertices must be far enough to be included
                const auto& lastVertex = smoothedVertices.back();

                if (glm::distance(lastVertex.left, vertex.left) < minDistance ||
                    glm::distance(lastVertex.right, vertex.right) < minDistance)
                    continue;
            }

            // check for invalid values (it create graphic and physic artifacts)
            const float minLength = 0.001f;
            if (glm::length(vertex.left) < minLength ||
                glm::length(vertex.right) < minLength)
                continue; // TODO: fix it

            vertex.color.r = smoother.calcAt(coef, asValue(SplineType::colorR));
            vertex.color.g = smoother.calcAt(coef, asValue(SplineType::colorG));
            vertex.color.b = smoother.calcAt(coef, asValue(SplineType::colorB));

            smoothedVertices.push_back(vertex);
        }
    }

    //
    //
    // generate circuit

    const unsigned int patchesPerKnot = 6;

    glm::vec3 prevNormal;

    struct t_circuitPatchData
    {
        t_vec3Array vertices;
        t_vec3Array normals;

        t_circuitPatchData()
        {
            vertices.reserve(512); // pre-allocate
            normals.reserve(512); // pre-allocate
        }
    };

    t_indices indices;
    t_circuitPatchData ground;
    t_circuitPatchData leftWall;
    t_circuitPatchData rightWall;
    t_vec3Array circuitPatchColors;

    indices.reserve(512); // pre-allocate
    circuitPatchColors.reserve(512); // pre-allocate

    for (unsigned int index = 1; index < smoothedVertices.size(); index += patchesPerKnot)
    {
        indices.clear();
        ground.vertices.clear();
        leftWall.vertices.clear();
        rightWall.vertices.clear();
        circuitPatchColors.clear();

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
            indices.push_back(currIndex + 0);
            indices.push_back(currIndex + 1);
            indices.push_back(currIndex + 2);
            indices.push_back(currIndex + 0);
            indices.push_back(currIndex + 3);
            indices.push_back(currIndex + 2);

            const auto& prevKnot = smoothedVertices[stepIndex - 1];
            const auto& currKnot = smoothedVertices[stepIndex];

            const glm::vec3& prevLeft = prevKnot.left;
            const glm::vec3& prevRight = prevKnot.right;
            const glm::vec3& currLeft = currKnot.left;
            const glm::vec3& currRight = currKnot.right;

            const glm::vec3& prevColor = prevKnot.color;
            const glm::vec3& currColor = currKnot.color;

            glm::vec3 currNormal = glm::normalize(glm::cross(prevLeft - prevRight, prevRight - currRight));

            // for the first time
            if (stepIndex == 1)
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

            ground.normals.push_back(prevNormal);
            ground.normals.push_back(prevNormal);
            ground.normals.push_back(currNormal);
            ground.normals.push_back(currNormal);

            circuitPatchColors.push_back(prevColor);
            circuitPatchColors.push_back(prevColor);
            circuitPatchColors.push_back(currColor);
            circuitPatchColors.push_back(currColor);

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

const CircuitBuilder::t_knots& CircuitBuilder::getKnots() const
{
    return _knots;
}
