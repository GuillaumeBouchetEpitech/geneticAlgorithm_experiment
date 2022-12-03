

#include "CircuitBuilder.hpp"

#include "framework/math/BSpline.hpp"

#include "framework/ErrorHandler.hpp"
#include "framework/TraceLogger.hpp"

#include "framework/string/trim.hpp"

#include "framework/asValue.hpp"

#include <stdexcept> // <= std::invalid_argument / runtime_error
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string_view>

#include <cmath>

//
//

namespace /*anonymous*/
{
    void validateFloat(float value, const std::string_view type)
    {
        if (glm::isnan(value))
            D_THROW(std::runtime_error, "invalid value (NaN), type=" << type);

        if (glm::isinf(value))
            D_THROW(std::runtime_error, "invalid value (inf), type=" << type);
    };
};

void CircuitBuilder::parse(const std::string_view& filename)
{
    //
    //
    // open/read file

    std::ifstream fileStream(filename.data());

    if (!fileStream.is_open())
        D_THROW(std::runtime_error, "fail to read a file, name=" << filename);

    //
    //
    // parse file and extract skeleton

    Knots rawKnots;

    float currentKnotsSize = 2.0f;
    glm::vec3 currentColor = { 0.0f, 0.0f, 0.0f };

    //
    //
    // build command handler (parser)

    std::unordered_map<std::string_view, std::function<void(std::istringstream&)>> commandsMap;

    const std::string_view cmd_start_transform_pos = "START_TRANSFORM_POSITION";
    commandsMap[cmd_start_transform_pos] = [&cmd_start_transform_pos, this](std::istringstream& isstr)
    {
        auto& value = _startTransform.position;
        if (!(isstr >> value.x >> value.y >> value.z))
            D_THROW(std::runtime_error, "failure to extract a line, type=" << cmd_start_transform_pos);

        for (int ii = 0; ii < 3; ++ii)
            validateFloat(value[ii], cmd_start_transform_pos);
    };

    const std::string_view cmd_start_transform_quat = "START_TRANSFORM_QUATERNION";
    commandsMap[cmd_start_transform_quat] = [&cmd_start_transform_quat, this](std::istringstream& isstr)
    {
        auto& value = _startTransform.quaternion;
        if (!(isstr >> value.x >> value.y >> value.z >> value.w))
            D_THROW(std::runtime_error, "failure to extract a line, type=" << cmd_start_transform_quat);

        for (int ii = 0; ii < 4; ++ii)
            validateFloat(value[ii], cmd_start_transform_quat);
    };

    const std::string_view cmd_knots_color = "KNOTS_COLOR";
    commandsMap[cmd_knots_color] = [&cmd_knots_color, &currentColor](std::istringstream& isstr)
    {
        glm::vec3 color;

        if (!(isstr >> color.x >> color.y >> color.z))
            D_THROW(std::runtime_error, "failure to extract a line, type=" << cmd_knots_color);

        for (int ii = 0; ii < 3; ++ii)
            validateFloat(color[ii], cmd_knots_color);

        if (color.x < 0 || color.x > 1 ||
            color.y < 0 || color.y > 1 ||
            color.z < 0 || color.z > 1)
            D_THROW(std::runtime_error, "color values must be [0..1], type=" << cmd_knots_color);

        currentColor = color;
    };

    const std::string_view cmd_knots_size = "KNOTS_SIZE";
    commandsMap[cmd_knots_size] = [&cmd_knots_size, &currentKnotsSize](std::istringstream& isstr)
    {
        float knotsSize;

        if (!(isstr >> knotsSize))
            D_THROW(std::runtime_error, "failure to extract a line, type=" << cmd_knots_size);

        validateFloat(knotsSize, cmd_knots_size);

        if (knotsSize <= 0)
            D_THROW(std::runtime_error, "knotsSize values must be > 0, type=" << cmd_knots_size);

        currentKnotsSize = knotsSize;
    };

    const std::string_view cmd_knots_dual = "KNOTS_DUAL";
    commandsMap[cmd_knots_dual] = [&cmd_knots_dual, &rawKnots, &currentKnotsSize, &currentColor](std::istringstream& isstr)
    {
        glm::vec3 left;
        glm::vec3 right;

        if (!(isstr >> left.x >> left.y >> left.z >> right.x >> right.y >> right.z))
            D_THROW(std::runtime_error, "failure to extract a line, type=" << cmd_knots_dual);

        for (int ii = 0; ii < 3; ++ii)
        {
            validateFloat(left[ii], cmd_knots_dual);
            validateFloat(right[ii], cmd_knots_dual);
        }

        rawKnots.push_back({ left, right, currentKnotsSize, currentColor });
    };

    //
    //
    // parse data

    std::string textLine;
    while (std::getline(fileStream, textLine))
    {
        // trim extracted line
        StringUtils::trim(textLine);

         // skip empty line(s) and comment(s)
        if (textLine.empty() || textLine.front() == '#')
            continue;

        std::istringstream isstr(textLine);

        // extract command type
        std::string cmdType;
        if (!(isstr >> cmdType))
            D_THROW(std::runtime_error, "failure to extract the line type");

        // retrieve command from type
        auto itCmd = commandsMap.find(cmdType);
        if (itCmd == commandsMap.end())
            D_THROW(std::runtime_error, "unknown line type, type=" << cmdType);

        // run command (parse)
        itCmd->second(isstr);
    }

    //
    //
    // concatenate skeleton

    for (CircuitBuilder::Knot& knot : rawKnots)
    {
        if (!_knots.empty())
        {
            const auto& prevKnot = _knots.back();

            // concatenate the knot with the previous one
            // => this is so the next knots can have relative coordinates
            // => it make the circuit text file easier to write
            knot.left += prevKnot.left;
            knot.right += prevKnot.right;
        }

        _knots.push_back(knot);
    }
}

void CircuitBuilder::load(const CircuitBuilder::StartTransform& startTransform,
                          const CircuitBuilder::Knots& knots)
{
    _startTransform = startTransform;
    _knots = knots;
}

//
//

void CircuitBuilder::generateWireframeSkeleton(CallbackNoNormals onSkeletonPatch)
{
    if (!onSkeletonPatch)
        D_THROW(std::invalid_argument, "no callback provided");

    if (_knots.empty())
        D_THROW(std::runtime_error, "not initialised");

    Vec3Array vertices;
    Indices indices;

    vertices.reserve(_knots.size() * 4); // pre-allocate
    indices.reserve(_knots.size() * 8 + 8); // pre-allocate

    // should be 0 but is not nice when shown with the chessboard floor
    constexpr float floorValue = 0.0f;

    for (std::size_t ii = 0; ii < _knots.size(); ++ii)
    {
        const auto& knot = _knots[ii];

        // "real" value
        vertices.push_back(knot.left);
        vertices.push_back(knot.right);
        // "on the floor" value
        vertices.emplace_back(knot.left.x, knot.left.y, floorValue);
        vertices.emplace_back(knot.right.x, knot.right.y, floorValue);

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

void CircuitBuilder::generateSmoothedKnotsData(Knots& smoothedKnotsData)
{
    smoothedKnotsData.clear();
    smoothedKnotsData.reserve(2048); // pre-allocate, ease the reallocation

    enum class SplineType: unsigned int
    {
        leftX = 0,
        leftY,
        leftZ,
        rightX,
        rightY,
        rightZ,
        size,
        colorR,
        colorG,
        colorB,

        count,
    };

    constexpr unsigned int  dimension = asValue(SplineType::count);
    const float*            knotsData = &_knots.front().left.x;
    const std::size_t       knotsLength = _knots.size() * dimension;
    constexpr unsigned int  splineDegree = 3;

    BSpline bsplineSmoother;
    bsplineSmoother.initialise({ knotsData, knotsLength, dimension, splineDegree });

    CircuitBuilder::CircuitVertex vertex;

    constexpr unsigned int maxIterations = 1000;
    constexpr float step = 1.0f / maxIterations; // tiny steps

    for (float coef = 0.0f; coef <= 1.0f; coef += step)
    {
        vertex.left.x = bsplineSmoother.calcAt(coef, asValue(SplineType::leftX));
        vertex.left.y = bsplineSmoother.calcAt(coef, asValue(SplineType::leftY));
        vertex.left.z = bsplineSmoother.calcAt(coef, asValue(SplineType::leftZ));
        vertex.right.x = bsplineSmoother.calcAt(coef, asValue(SplineType::rightX));
        vertex.right.y = bsplineSmoother.calcAt(coef, asValue(SplineType::rightY));
        vertex.right.z = bsplineSmoother.calcAt(coef, asValue(SplineType::rightZ));

        const float knotSize = bsplineSmoother.calcAt(coef, asValue(SplineType::size));

        if (!smoothedKnotsData.empty())
        {
            // both left and right vertices must be far enough to be included
            const auto& lastVertex = smoothedKnotsData.back();

            if (glm::distance(lastVertex.left, vertex.left) < knotSize ||
                glm::distance(lastVertex.right, vertex.right) < knotSize)
                continue;
        }

        // check for invalid values (it create graphic and physic artifacts)
        const float minLength = 0.001f;
        if (glm::length(vertex.left) < minLength ||
            glm::length(vertex.right) < minLength)
            continue; // TODO: fix it

        vertex.color.r = bsplineSmoother.calcAt(coef, asValue(SplineType::colorR));
        vertex.color.g = bsplineSmoother.calcAt(coef, asValue(SplineType::colorG));
        vertex.color.b = bsplineSmoother.calcAt(coef, asValue(SplineType::colorB));

        smoothedKnotsData.push_back(vertex);
    }
}

void CircuitBuilder::generateCircuitGeometry(
    CallbackNormals onNewGroundPatch,
    CallbackNormals onNewWallPatch)
{
    if (_knots.empty())
        D_THROW(std::runtime_error, "not initialised");

    if (!onNewGroundPatch && !onNewWallPatch)
        D_THROW(std::invalid_argument, "no callbacks provided");

    Knots smoothedKnotsData;
    generateSmoothedKnotsData(smoothedKnotsData);

    constexpr std::size_t patchesPerKnot = 6;

    glm::vec3 prevNormal;

    struct CircuitPatchData
    {
        Vec3Array vertices;
        Vec3Array normals;

        CircuitPatchData()
        {
            vertices.reserve(512); // pre-allocate
            normals.reserve(512); // pre-allocate
        }
    };

    Indices indices;
    CircuitPatchData ground;
    CircuitPatchData leftWall;
    CircuitPatchData rightWall;
    Vec3Array circuitPatchColors;

    indices.reserve(512); // pre-allocate
    circuitPatchColors.reserve(512); // pre-allocate

    const unsigned int startIndex = 1;
    for (std::size_t index = startIndex; index < smoothedKnotsData.size(); index += patchesPerKnot)
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

        for (std::size_t stepIndex = index; stepIndex < smoothedKnotsData.size() && stepIndex < index + patchesPerKnot; ++stepIndex)
        {
            const int currIndex = indicexIndex++ * 4;
            indices.push_back(currIndex + 0);
            indices.push_back(currIndex + 1);
            indices.push_back(currIndex + 2);
            indices.push_back(currIndex + 0);
            indices.push_back(currIndex + 3);
            indices.push_back(currIndex + 2);

            const auto& prevKnot = smoothedKnotsData[stepIndex - 1];
            const auto& currKnot = smoothedKnotsData[stepIndex];

            const glm::vec3& prevLeft = prevKnot.left;
            const glm::vec3& prevRight = prevKnot.right;
            const glm::vec3& currLeft = currKnot.left;
            const glm::vec3& currRight = currKnot.right;

            const glm::vec3& prevColor = prevKnot.color;
            const glm::vec3& currColor = currKnot.color;

            const glm::vec3 currNormal = glm::normalize(glm::cross(prevLeft - prevRight, prevRight - currRight));

            // for the first time
            if (stepIndex == startIndex)
                prevNormal = currNormal;

            const glm::vec3 prevNormalLeft(prevNormal.x, prevNormal.z, prevNormal.y);
            const glm::vec3 currNormalLeft(currNormal.x, currNormal.z, currNormal.y);
            const glm::vec3 prevNormalRight = -prevNormalLeft;
            const glm::vec3 currNormalRight = -currNormalLeft;

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

            const glm::vec3 prevHeight = prevNormal * 10.0f;
            const glm::vec3 currHeight = currNormal * 10.0f;

            //

            const glm::vec3 prevTopLeft = prevLeft + prevHeight;
            const glm::vec3 currTopLeft = currLeft + currHeight;

            leftWall.vertices.push_back(prevLeft);
            leftWall.vertices.push_back(prevTopLeft);
            leftWall.vertices.push_back(currTopLeft);
            leftWall.vertices.push_back(currLeft);

            leftWall.normals.push_back(prevNormalLeft);
            leftWall.normals.push_back(prevNormalLeft);
            leftWall.normals.push_back(currNormalLeft);
            leftWall.normals.push_back(currNormalLeft);

            //

            const glm::vec3 prevTopRight = prevRight + prevHeight;
            const glm::vec3 currTopRight = currRight + currHeight;

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

const CircuitBuilder::StartTransform& CircuitBuilder::getStartTransform() const
{
    return _startTransform;
}

const CircuitBuilder::Knots& CircuitBuilder::getKnots() const
{
    return _knots;
}
