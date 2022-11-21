
#pragma once

#include "framework/graphic/ShaderProgram.hpp"
#include "framework/graphic/Geometry.hpp"

#include "framework/graphic/camera/Camera.hpp"

#include "framework/helpers/GLMath.hpp"

#include <vector>
#include <memory>

struct StackRenderer
{
public:
    struct Vertex
    {
        glm::vec3 position;
        glm::vec4 color;

        Vertex(const glm::vec3& position, const glm::vec3& color)
            : position(position)
            , color(color, 1.0f)
        {}

        Vertex(const glm::vec3& position, const glm::vec4& color)
            : position(position)
            , color(color)
        {}
    };
    using Vertices = std::vector<Vertex>;

public:
    StackRenderer() = default;
    ~StackRenderer() = default;

public:
    void initialise();
    void setMatricesData(const Camera::MatricesData& matricesData);

public:
    void push(const Vertex& vertex);
    void pushLine(const glm::vec3& posA, const glm::vec3& posB, const glm::vec4& color);
    void pushLine(const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& color);
    void pushCross(const glm::vec3& pos, const glm::vec3& color, float halfExtent);

    void pushTriangle(const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& posC, const glm::vec4& color);
    void pushTriangle(const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& posC,
                      const glm::vec4& colorA, const glm::vec4& colorB, const glm::vec4& colorC);

    void pushQuad(const glm::vec2& center, const glm::vec2& size, const glm::vec4& color);
    void pushQuad(const glm::vec3& center, const glm::vec2& size, const glm::vec4& color);
    void pushCircle(const glm::vec2& center, float radius, const glm::vec4& color);
    void pushCircle(const glm::vec3& center, float radius, const glm::vec4& color);

    void pushThickTriangleLine(const glm::vec2& posA, const glm::vec2& posB, float thickness, const glm::vec4& color);
    // void pushThickTriangleLine(const glm::vec3& posA, const glm::vec3& posB, float thickness, const glm::vec4& color);
    void pushThickTriangleLine(const glm::vec2& posA, const glm::vec2& posB, float thicknessA, float thicknessB, const glm::vec4& color);
    void pushThickTriangleLine(
        const glm::vec2& posA,
        const glm::vec2& posB,
        float thicknessA,
        float thicknessB,
        const glm::vec4& color,
        float z);

    void pushThickTriangle3DLine(const glm::vec3& posA, const glm::vec3& posB,
                                 float thicknessA, float thicknessB,
                                 const glm::vec4& colorA, const glm::vec4& colorB);

    // 2d
    void pushLine(const glm::vec2& posA, const glm::vec2& posB, const glm::vec4& color);
    void pushLine(const glm::vec2& posA, const glm::vec2& posB, const glm::vec3& color);
    void pushRectangle(const glm::vec2& pos, const glm::vec2& size, const glm::vec3& color);
    // 2d

    void flush();
    void flushLines();
    void flushTriangles();

    std::size_t getLinesCount() const;
    std::size_t getTrianglesCount() const;

private:
    std::shared_ptr<ShaderProgram> _shader;

    struct Geometries
    {
        Geometry lines;
        Geometry triangles;
    }
    _geometries;

    Vertices _lineVertices;
    Vertices _triangleVertices;

    Camera::MatricesData _matricesData;

};
