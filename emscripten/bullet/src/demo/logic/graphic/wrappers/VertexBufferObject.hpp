
#pragma once

#include "helpers/OpenGLES.hpp"

#include <vector>

class VertexBufferObject
{
private:
    std::vector<GLuint> _ids;

public:
    ~VertexBufferObject();

public:
    void allocate(unsigned int size = 1);
    void deallocate();

public:
    bool isAllocated() const;

public:
    void bind(unsigned int index = 0) const;

public:
    static void unbind();
};
