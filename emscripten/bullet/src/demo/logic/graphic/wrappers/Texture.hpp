
#pragma once

#include "thirdparty/OpenGLES.hpp"
#include "thirdparty/GLMath.hpp"

#include <string>

// forward declaration
class FrameBuffer;

class Texture
{
    friend FrameBuffer; // required by FrameBuffer::attachColorTexture()

private:
    GLuint      _textureId = 0;
    glm::ivec2  _size = { 0, 0 };

public:
    Texture() = default;
    ~Texture();

public:
    void load(const std::string& filename, bool pixelated = false, bool repeat = false);
    void allocateBlank(const glm::ivec2& size, bool pixelated = false, bool repeat = false,
                       const GLvoid* pixels = nullptr);

public:
    const glm::ivec2& getSize() const;

public:
    void bind() const;

public:
    static void unbind();

};
