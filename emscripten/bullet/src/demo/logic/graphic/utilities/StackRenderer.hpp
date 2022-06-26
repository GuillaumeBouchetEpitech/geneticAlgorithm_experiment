
#pragma once

#include "helpers/GLMath.hpp"

#include <vector>

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
    Vertices _lineVertices;
    Vertices _triangleVertices;

public:
    StackRenderer();
    ~StackRenderer();

public:
    void push(const Vertex& vertex);
    void pushLine(const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& color);
    void pushCross(const glm::vec3& pos, const glm::vec3& color, float halfExtent);

    void pushTriangle(const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& posC, const glm::vec4& color);
    void pushTriangle(const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& posC,
                      const glm::vec3& colorA, const glm::vec3& colorB, const glm::vec3& colorC);

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

    // void pushThickTriangle3DLine(const glm::vec3& posA, const glm::vec3& posB,
    //                              float thicknessA, float thicknessB,
    //                              const glm::vec3& colorA, const glm::vec3& colorB);

    // 2d
    void pushLine(const glm::vec2& posA, const glm::vec2& posB, const glm::vec3& color);
    void pushRectangle(const glm::vec2& pos, const glm::vec2& size, const glm::vec3& color);
    // 2d

    void flush();
    void flushLines();
    void flushTriangles();
};
