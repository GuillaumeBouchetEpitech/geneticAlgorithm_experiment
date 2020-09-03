
#pragma once

#include "thirdparty/GLMath.hpp"

#include <vector>

struct StackRenderer
{
public:
    struct t_vertex
    {
        glm::vec3 position;
        glm::vec3 color;
    };
    typedef std::vector<t_vertex> t_vertices;

public:
    t_vertices _lineVertices;
    t_vertices _triangleVertices;

public:
    StackRenderer();
    ~StackRenderer();

public:
    void push(const t_vertex& vertex);
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
