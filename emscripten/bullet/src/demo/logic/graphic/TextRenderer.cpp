
#include "TextRenderer.hpp"

#include "demo/logic/Data.hpp"

#include "demo/utilities/ErrorHandler.hpp"

namespace /*anonymous*/
{
    const glm::vec2 textureSize = { 256, 96 };
    const glm::vec2 gridSize = { 16, 6 };
    const glm::vec2 letterSize = textureSize / gridSize;
    const glm::vec2 letterCoord = letterSize / textureSize;
};

TextRenderer::TextRenderer()
{
    _offsets.reserve(256);

    _lettersMap = {

        { ' ',  {  0 * letterCoord.x, 0 * letterCoord.y } },
        { '!',  {  1 * letterCoord.x, 0 * letterCoord.y } },
        { '\"', {  2 * letterCoord.x, 0 * letterCoord.y } },
        { '#',  {  3 * letterCoord.x, 0 * letterCoord.y } },
        { '$',  {  4 * letterCoord.x, 0 * letterCoord.y } },
        { '%',  {  5 * letterCoord.x, 0 * letterCoord.y } },
        { '&',  {  6 * letterCoord.x, 0 * letterCoord.y } },
        { '\'', {  7 * letterCoord.x, 0 * letterCoord.y } },
        { '(',  {  8 * letterCoord.x, 0 * letterCoord.y } },
        { ')',  {  9 * letterCoord.x, 0 * letterCoord.y } },
        { '*',  { 10 * letterCoord.x, 0 * letterCoord.y } },
        { '+',  { 11 * letterCoord.x, 0 * letterCoord.y } },
        { ',',  { 12 * letterCoord.x, 0 * letterCoord.y } },
        { '-',  { 13 * letterCoord.x, 0 * letterCoord.y } },
        { '.',  { 14 * letterCoord.x, 0 * letterCoord.y } },
        { '/',  { 15 * letterCoord.x, 0 * letterCoord.y } },

        { '0',  {  0 * letterCoord.x, 1 * letterCoord.y } },
        { '1',  {  1 * letterCoord.x, 1 * letterCoord.y } },
        { '2',  {  2 * letterCoord.x, 1 * letterCoord.y } },
        { '3',  {  3 * letterCoord.x, 1 * letterCoord.y } },
        { '4',  {  4 * letterCoord.x, 1 * letterCoord.y } },
        { '5',  {  5 * letterCoord.x, 1 * letterCoord.y } },
        { '6',  {  6 * letterCoord.x, 1 * letterCoord.y } },
        { '7',  {  7 * letterCoord.x, 1 * letterCoord.y } },
        { '8',  {  8 * letterCoord.x, 1 * letterCoord.y } },
        { '9',  {  9 * letterCoord.x, 1 * letterCoord.y } },
        { ':',  { 10 * letterCoord.x, 1 * letterCoord.y } },
        { ';',  { 11 * letterCoord.x, 1 * letterCoord.y } },
        { '<',  { 12 * letterCoord.x, 1 * letterCoord.y } },
        { '=',  { 13 * letterCoord.x, 1 * letterCoord.y } },
        { '>',  { 14 * letterCoord.x, 1 * letterCoord.y } },
        { '?',  { 15 * letterCoord.x, 1 * letterCoord.y } },

        { '@',  {  0 * letterCoord.x, 2 * letterCoord.y } },
        { 'A',  {  1 * letterCoord.x, 2 * letterCoord.y } },
        { 'B',  {  2 * letterCoord.x, 2 * letterCoord.y } },
        { 'C',  {  3 * letterCoord.x, 2 * letterCoord.y } },
        { 'D',  {  4 * letterCoord.x, 2 * letterCoord.y } },
        { 'E',  {  5 * letterCoord.x, 2 * letterCoord.y } },
        { 'F',  {  6 * letterCoord.x, 2 * letterCoord.y } },
        { 'G',  {  7 * letterCoord.x, 2 * letterCoord.y } },
        { 'H',  {  8 * letterCoord.x, 2 * letterCoord.y } },
        { 'I',  {  9 * letterCoord.x, 2 * letterCoord.y } },
        { 'J',  { 10 * letterCoord.x, 2 * letterCoord.y } },
        { 'K',  { 11 * letterCoord.x, 2 * letterCoord.y } },
        { 'L',  { 12 * letterCoord.x, 2 * letterCoord.y } },
        { 'M',  { 13 * letterCoord.x, 2 * letterCoord.y } },
        { 'N',  { 14 * letterCoord.x, 2 * letterCoord.y } },
        { 'O',  { 15 * letterCoord.x, 2 * letterCoord.y } },

        { 'P',  {  0 * letterCoord.x, 3 * letterCoord.y } },
        { 'Q',  {  1 * letterCoord.x, 3 * letterCoord.y } },
        { 'R',  {  2 * letterCoord.x, 3 * letterCoord.y } },
        { 'S',  {  3 * letterCoord.x, 3 * letterCoord.y } },
        { 'T',  {  4 * letterCoord.x, 3 * letterCoord.y } },
        { 'U',  {  5 * letterCoord.x, 3 * letterCoord.y } },
        { 'V',  {  6 * letterCoord.x, 3 * letterCoord.y } },
        { 'W',  {  7 * letterCoord.x, 3 * letterCoord.y } },
        { 'X',  {  8 * letterCoord.x, 3 * letterCoord.y } },
        { 'Y',  {  9 * letterCoord.x, 3 * letterCoord.y } },
        { 'Z',  { 10 * letterCoord.x, 3 * letterCoord.y } },
        { '[',  { 11 * letterCoord.x, 3 * letterCoord.y } },
        { '\\', { 12 * letterCoord.x, 3 * letterCoord.y } },
        { ']',  { 13 * letterCoord.x, 3 * letterCoord.y } },
        { '^',  { 14 * letterCoord.x, 3 * letterCoord.y } },
        { '_',  { 15 * letterCoord.x, 3 * letterCoord.y } },

        { '`',  {  0 * letterCoord.x, 4 * letterCoord.y } },
        { 'a',  {  1 * letterCoord.x, 4 * letterCoord.y } },
        { 'b',  {  2 * letterCoord.x, 4 * letterCoord.y } },
        { 'c',  {  3 * letterCoord.x, 4 * letterCoord.y } },
        { 'd',  {  4 * letterCoord.x, 4 * letterCoord.y } },
        { 'e',  {  5 * letterCoord.x, 4 * letterCoord.y } },
        { 'f',  {  6 * letterCoord.x, 4 * letterCoord.y } },
        { 'g',  {  7 * letterCoord.x, 4 * letterCoord.y } },
        { 'h',  {  8 * letterCoord.x, 4 * letterCoord.y } },
        { 'i',  {  9 * letterCoord.x, 4 * letterCoord.y } },
        { 'j',  { 10 * letterCoord.x, 4 * letterCoord.y } },
        { 'k',  { 11 * letterCoord.x, 4 * letterCoord.y } },
        { 'l',  { 12 * letterCoord.x, 4 * letterCoord.y } },
        { 'm',  { 13 * letterCoord.x, 4 * letterCoord.y } },
        { 'n',  { 14 * letterCoord.x, 4 * letterCoord.y } },
        { 'o',  { 15 * letterCoord.x, 4 * letterCoord.y } },

        { 'p',  {  0 * letterCoord.x, 5 * letterCoord.y } },
        { 'q',  {  1 * letterCoord.x, 5 * letterCoord.y } },
        { 'r',  {  2 * letterCoord.x, 5 * letterCoord.y } },
        { 's',  {  3 * letterCoord.x, 5 * letterCoord.y } },
        { 't',  {  4 * letterCoord.x, 5 * letterCoord.y } },
        { 'u',  {  5 * letterCoord.x, 5 * letterCoord.y } },
        { 'v',  {  6 * letterCoord.x, 5 * letterCoord.y } },
        { 'w',  {  7 * letterCoord.x, 5 * letterCoord.y } },
        { 'x',  {  8 * letterCoord.x, 5 * letterCoord.y } },
        { 'y',  {  9 * letterCoord.x, 5 * letterCoord.y } },
        { 'z',  { 10 * letterCoord.x, 5 * letterCoord.y } },
        { '{',  { 11 * letterCoord.x, 5 * letterCoord.y } },
        { '|',  { 12 * letterCoord.x, 5 * letterCoord.y } },
        { '}',  { 13 * letterCoord.x, 5 * letterCoord.y } },
        { '~',  { 14 * letterCoord.x, 5 * letterCoord.y } },

    };
}

//

void	TextRenderer::push(const glm::vec2& position, const std::string& message,
                           float scale /*= 1.0f*/)
{
    glm::vec2 currPos = position;

    for (char letter : message)
    {
        if (letter == '\n')
        {
            currPos.x = position.x;
            currPos.y -= letterSize.y * scale;
            continue;
        }

        auto it = _lettersMap.find(letter);

        if (it == _lettersMap.end())
            D_THROW(std::runtime_error, "fail to find a letter"
                    << ", letter=" << letter);

        _offsets.push_back({ currPos, it->second, scale });

        currPos.x += letterSize.x * scale;
    }
}

void	TextRenderer::clear()
{
    _offsets.clear();
}

void	TextRenderer::render()
{
    if (_offsets.empty())
        return;

    auto& geometry = Data::get()->graphic.geometries.hudText.letters;

    geometry.updateBuffer(1, _offsets);
    geometry.setInstancedCount(_offsets.size());

    geometry.render();
}
