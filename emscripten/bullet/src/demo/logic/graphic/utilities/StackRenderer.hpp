
#pragma once

#include "thirdparty/GLMath.hpp"

#include <vector>

struct StackRenderer
{
public:
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 color;

        Vertex(const glm::vec3& position, const glm::vec3& color)
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

    void pushTriangle(const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& posC, const glm::vec3& color);
    void pushQuad(const glm::vec2& center, const glm::vec2& size, const glm::vec3& color);

    // 2d
    void pushLine(const glm::vec2& posA, const glm::vec2& posB, const glm::vec3& color);
    void pushRectangle(const glm::vec2& pos, const glm::vec2& size, const glm::vec3& color);
    // 2d

    void flush();
    void flushLines();
    void flushTriangles();
};
