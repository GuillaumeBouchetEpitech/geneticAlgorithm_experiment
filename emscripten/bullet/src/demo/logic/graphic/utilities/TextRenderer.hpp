
#pragma once

#include "thirdparty/GLMath.hpp"

#include <vector>
#include <map>

class Shader;

struct TextRenderer
{
public:
    struct t_offset
    {
        glm::vec2    position;
        glm::vec2    texCoord;
        float        scale;
    };
    typedef std::vector<t_offset> t_lettersOffset;

public:
    t_lettersOffset             _lettersOffset;
    std::map<char, glm::vec2>   _lettersTexCoordMap;

public:
    TextRenderer() = default;

public:
    void    initialise();

public:
    void    push(const glm::vec2& position,
                 const std::string& message,
                 float scale = 1.0f);

    void    clear();

    void    render();
};
