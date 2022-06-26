
#pragma once

#include "helpers/GLMath.hpp"

#include <vector>
#include <unordered_map>

class Shader;

struct TextRenderer
{
public:
    struct LetterOffset
    {
        glm::vec3    position;
        glm::vec2    texCoord;
        glm::vec3    color;
        float        scale;
    };
    using LettersOffsets = std::vector<LetterOffset>;

public:
    LettersOffsets _lettersOffsetColored;
    LettersOffsets _lettersOffsetBackground;
    std::unordered_map<char, glm::vec2> _lettersTexCoordMap;

public:
    TextRenderer() = default;

public:
    void initialise();

public:
    void push(const glm::vec2& position,
              const std::string& message,
              const glm::vec3& color,
              float scale = 1.0f);

    void clear();

    void render();
};
