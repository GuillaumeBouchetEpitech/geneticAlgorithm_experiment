
#include "TextRenderer.hpp"

#include "demo/logic/Data.hpp"

#include "demo/utilities/ErrorHandler.hpp"

void TextRenderer::initialise()
{
    const auto& hudText = Data::get().graphic.hudText;
    const glm::vec2 letterSize = hudText.textureSize / hudText.gridSize;
    const glm::vec2 texCoord = letterSize / hudText.textureSize;

    _lettersOffsetColored.reserve(1024); // <= pre-allocate
    _lettersOffsetBackground.reserve(1024); // <= pre-allocate

    _lettersTexCoordMap = {

        { ' ',  {  0 * texCoord.x, 0 * texCoord.y } },
        { '!',  {  1 * texCoord.x, 0 * texCoord.y } },
        { '\"', {  2 * texCoord.x, 0 * texCoord.y } },
        { '#',  {  3 * texCoord.x, 0 * texCoord.y } },
        { '$',  {  4 * texCoord.x, 0 * texCoord.y } },
        { '%',  {  5 * texCoord.x, 0 * texCoord.y } },
        { '&',  {  6 * texCoord.x, 0 * texCoord.y } },
        { '\'', {  7 * texCoord.x, 0 * texCoord.y } },
        { '(',  {  8 * texCoord.x, 0 * texCoord.y } },
        { ')',  {  9 * texCoord.x, 0 * texCoord.y } },
        { '*',  { 10 * texCoord.x, 0 * texCoord.y } },
        { '+',  { 11 * texCoord.x, 0 * texCoord.y } },
        { ',',  { 12 * texCoord.x, 0 * texCoord.y } },
        { '-',  { 13 * texCoord.x, 0 * texCoord.y } },
        { '.',  { 14 * texCoord.x, 0 * texCoord.y } },
        { '/',  { 15 * texCoord.x, 0 * texCoord.y } },

        { '0',  {  0 * texCoord.x, 1 * texCoord.y } },
        { '1',  {  1 * texCoord.x, 1 * texCoord.y } },
        { '2',  {  2 * texCoord.x, 1 * texCoord.y } },
        { '3',  {  3 * texCoord.x, 1 * texCoord.y } },
        { '4',  {  4 * texCoord.x, 1 * texCoord.y } },
        { '5',  {  5 * texCoord.x, 1 * texCoord.y } },
        { '6',  {  6 * texCoord.x, 1 * texCoord.y } },
        { '7',  {  7 * texCoord.x, 1 * texCoord.y } },
        { '8',  {  8 * texCoord.x, 1 * texCoord.y } },
        { '9',  {  9 * texCoord.x, 1 * texCoord.y } },
        { ':',  { 10 * texCoord.x, 1 * texCoord.y } },
        { ';',  { 11 * texCoord.x, 1 * texCoord.y } },
        { '<',  { 12 * texCoord.x, 1 * texCoord.y } },
        { '=',  { 13 * texCoord.x, 1 * texCoord.y } },
        { '>',  { 14 * texCoord.x, 1 * texCoord.y } },
        { '?',  { 15 * texCoord.x, 1 * texCoord.y } },

        { '@',  {  0 * texCoord.x, 2 * texCoord.y } },
        { 'A',  {  1 * texCoord.x, 2 * texCoord.y } },
        { 'B',  {  2 * texCoord.x, 2 * texCoord.y } },
        { 'C',  {  3 * texCoord.x, 2 * texCoord.y } },
        { 'D',  {  4 * texCoord.x, 2 * texCoord.y } },
        { 'E',  {  5 * texCoord.x, 2 * texCoord.y } },
        { 'F',  {  6 * texCoord.x, 2 * texCoord.y } },
        { 'G',  {  7 * texCoord.x, 2 * texCoord.y } },
        { 'H',  {  8 * texCoord.x, 2 * texCoord.y } },
        { 'I',  {  9 * texCoord.x, 2 * texCoord.y } },
        { 'J',  { 10 * texCoord.x, 2 * texCoord.y } },
        { 'K',  { 11 * texCoord.x, 2 * texCoord.y } },
        { 'L',  { 12 * texCoord.x, 2 * texCoord.y } },
        { 'M',  { 13 * texCoord.x, 2 * texCoord.y } },
        { 'N',  { 14 * texCoord.x, 2 * texCoord.y } },
        { 'O',  { 15 * texCoord.x, 2 * texCoord.y } },

        { 'P',  {  0 * texCoord.x, 3 * texCoord.y } },
        { 'Q',  {  1 * texCoord.x, 3 * texCoord.y } },
        { 'R',  {  2 * texCoord.x, 3 * texCoord.y } },
        { 'S',  {  3 * texCoord.x, 3 * texCoord.y } },
        { 'T',  {  4 * texCoord.x, 3 * texCoord.y } },
        { 'U',  {  5 * texCoord.x, 3 * texCoord.y } },
        { 'V',  {  6 * texCoord.x, 3 * texCoord.y } },
        { 'W',  {  7 * texCoord.x, 3 * texCoord.y } },
        { 'X',  {  8 * texCoord.x, 3 * texCoord.y } },
        { 'Y',  {  9 * texCoord.x, 3 * texCoord.y } },
        { 'Z',  { 10 * texCoord.x, 3 * texCoord.y } },
        { '[',  { 11 * texCoord.x, 3 * texCoord.y } },
        { '\\', { 12 * texCoord.x, 3 * texCoord.y } },
        { ']',  { 13 * texCoord.x, 3 * texCoord.y } },
        { '^',  { 14 * texCoord.x, 3 * texCoord.y } },
        { '_',  { 15 * texCoord.x, 3 * texCoord.y } },

        { '`',  {  0 * texCoord.x, 4 * texCoord.y } },
        { 'a',  {  1 * texCoord.x, 4 * texCoord.y } },
        { 'b',  {  2 * texCoord.x, 4 * texCoord.y } },
        { 'c',  {  3 * texCoord.x, 4 * texCoord.y } },
        { 'd',  {  4 * texCoord.x, 4 * texCoord.y } },
        { 'e',  {  5 * texCoord.x, 4 * texCoord.y } },
        { 'f',  {  6 * texCoord.x, 4 * texCoord.y } },
        { 'g',  {  7 * texCoord.x, 4 * texCoord.y } },
        { 'h',  {  8 * texCoord.x, 4 * texCoord.y } },
        { 'i',  {  9 * texCoord.x, 4 * texCoord.y } },
        { 'j',  { 10 * texCoord.x, 4 * texCoord.y } },
        { 'k',  { 11 * texCoord.x, 4 * texCoord.y } },
        { 'l',  { 12 * texCoord.x, 4 * texCoord.y } },
        { 'm',  { 13 * texCoord.x, 4 * texCoord.y } },
        { 'n',  { 14 * texCoord.x, 4 * texCoord.y } },
        { 'o',  { 15 * texCoord.x, 4 * texCoord.y } },

        { 'p',  {  0 * texCoord.x, 5 * texCoord.y } },
        { 'q',  {  1 * texCoord.x, 5 * texCoord.y } },
        { 'r',  {  2 * texCoord.x, 5 * texCoord.y } },
        { 's',  {  3 * texCoord.x, 5 * texCoord.y } },
        { 't',  {  4 * texCoord.x, 5 * texCoord.y } },
        { 'u',  {  5 * texCoord.x, 5 * texCoord.y } },
        { 'v',  {  6 * texCoord.x, 5 * texCoord.y } },
        { 'w',  {  7 * texCoord.x, 5 * texCoord.y } },
        { 'x',  {  8 * texCoord.x, 5 * texCoord.y } },
        { 'y',  {  9 * texCoord.x, 5 * texCoord.y } },
        { 'z',  { 10 * texCoord.x, 5 * texCoord.y } },
        { '{',  { 11 * texCoord.x, 5 * texCoord.y } },
        { '|',  { 12 * texCoord.x, 5 * texCoord.y } },
        { '}',  { 13 * texCoord.x, 5 * texCoord.y } },
        { '~',  { 14 * texCoord.x, 5 * texCoord.y } },

    };
}

//

void TextRenderer::push(const glm::vec2& position,
                        const std::string& message,
                        const glm::vec3& color,
                        float scale /* = 1.0f */)
{
    const auto& hudText = Data::get().graphic.hudText;
    const glm::vec2 letterSize = hudText.textureSize / hudText.gridSize;

    glm::vec2 currPos = position;

    for (char letter : message)
    {
        if (letter == '\n')
        {
            currPos.x = position.x;
            currPos.y -= letterSize.y * scale;
            continue;
        }

        auto it = _lettersTexCoordMap.find(letter);

        if (it == _lettersTexCoordMap.end())
            D_THROW(std::runtime_error, "fail to find a letter"
                    << ", letter=" << letter);

        const auto& texCoord = it->second;

        _lettersOffsetColored.push_back({ glm::vec3(currPos.x, currPos.y, 0.1f), texCoord, color, scale });

        constexpr int range = 1;
        for (int yy2 = -range; yy2 <= +range; yy2 += range)
        for (int xx2 = -range; xx2 <= +range; xx2 += range)
        {
            if (xx2 == 0 && yy2 == 0)
                continue;

            const glm::vec3 pos0 = glm::vec3(currPos.x + scale * xx2, currPos.y + scale * yy2, 0.0f);

            _lettersOffsetBackground.push_back({ pos0, texCoord, glm::vec3(0), scale });
        }

        currPos.x += letterSize.x * scale;
    }
}

void TextRenderer::clear()
{
    _lettersOffsetColored.clear();
    _lettersOffsetBackground.clear();
}

void TextRenderer::render()
{
    if (_lettersOffsetColored.empty())
        return;

    auto& geometry = Data::get().graphic.geometries.hudText.letters;

    geometry.updateBuffer(1, _lettersOffsetBackground);
    geometry.setInstancedCount(_lettersOffsetBackground.size());

    geometry.render();

    geometry.updateBuffer(1, _lettersOffsetColored);
    geometry.setInstancedCount(_lettersOffsetColored.size());

    geometry.render();
}
