
#pragma once

#include "thirdparty/GLMath.hpp"

#include <vector>
#include <map>

struct TextRenderer
{
public:
    struct t_offset
    {
        glm::vec2	position;
        glm::vec2	texCoord;
        float	    scale;
    };
	typedef std::vector<t_offset> t_offsets;

public:
    t_offsets                   _offsets;
    std::map<char, glm::vec2>   _lettersMap;

public:
	TextRenderer();

public:
	void	push(const glm::vec2& position, const std::string& message,
                 float scale = 1.0f);

	void	clear();

	void	render();
};
