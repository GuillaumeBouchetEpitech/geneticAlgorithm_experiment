
#pragma once

#include "thirdparty/GLMath.hpp"

#include <vector>
#include <unordered_map>

class Shader;

struct TextRenderer
{
public:
    struct LetterOffset
    {
        glm::vec2    position;
        glm::vec2    texCoord;
        float        scale;

        LetterOffset(const glm::vec2& position, const glm::vec2& texCoord, float scale)
            : position(position)
            , texCoord(texCoord)
            , scale(scale)
        {}
    };
    using LettersOffsets = std::vector<LetterOffset>;

public:
    LettersOffsets _lettersOffset;
    std::unordered_map<char, glm::vec2> _lettersTexCoordMap;

public:
    TextRenderer() = default;

public:
    void initialise();

public:
    void push(const glm::vec2& position,
              const std::string& message,
              float scale = 1.0f);

    void clear();

    void render();
};
