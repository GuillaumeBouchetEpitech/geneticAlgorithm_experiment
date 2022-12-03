
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <string>
#include <vector>
#include <functional>

class CircuitBuilder
{
public:
    using Vec3Array = std::vector<glm::vec3>;
    using Indices = std::vector<int>;

    struct StartTransform
    {
        glm::vec3 position;
        glm::vec4 quaternion;
    };

    struct Knot
    {
        glm::vec3 left;
        glm::vec3 right;
        float size;
        glm::vec3 color;
    };
    using Knots = std::vector<Knot>;
    using CircuitVertex = Knot;

public:
    using CallbackNoNormals = std::function<void(const Vec3Array& vertices,
                                                 const Indices& indices)>;

    using CallbackNormals = std::function<void(const Vec3Array& vertices,
                                               const Vec3Array& colors,
                                               const Vec3Array& normals,
                                               const Indices& indices)>;

private:
    StartTransform _startTransform;
    Knots _knots;

public:
    void parse(const std::string_view& filename);
    void load(const StartTransform& startTransform, const Knots& knots);

public:
    void generateWireframeSkeleton(CallbackNoNormals onSkeletonPatch);
    void generateCircuitGeometry(CallbackNormals onNewGroundPatch, CallbackNormals onNewWallPatch);

private:
    void generateSmoothedKnotsData(Knots& smoothedVertices);

public:
    const StartTransform& getStartTransform() const;
    const Knots& getKnots() const;

};
