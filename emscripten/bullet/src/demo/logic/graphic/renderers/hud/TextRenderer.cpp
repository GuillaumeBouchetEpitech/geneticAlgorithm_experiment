
#include "TextRenderer.hpp"

#include "demo/logic/Data.hpp"

#include "demo/logic/graphicIds.hpp"

#include "framework/asValue.hpp"

#include "framework/graphic/ResourceManager.hpp"
#include "framework/graphic/GeometryBuilder.hpp"

#include "framework/ErrorHandler.hpp"

void TextRenderer::initialise()
{

    _shader = Data::get().graphic.resourceManager.getShader(asValue(Shaders::hudText));

    _texture = Data::get().graphic.resourceManager.getTexture(0);

    const glm::vec2 gridSize = { 16, 16 };
    const glm::vec2 letterSize = glm::vec2(_texture->getSize()) / gridSize;
    const glm::vec2 texCoord = letterSize / glm::vec2(_texture->getSize());

    {

        GeometryBuilder geometryBuilder;

        geometryBuilder
            .reset()
            .setShader(*_shader)
            .setPrimitiveType(Geometry::PrimitiveType::triangles)
            .addVbo()
            .addVboAttribute("a_position", Geometry::AttrType::Vec2f, 0)
            .addVboAttribute("a_texCoord", Geometry::AttrType::Vec2f, 2)
            .addVbo()
            .setVboAsInstanced()
            .addVboAttribute("a_offsetPosition", Geometry::AttrType::Vec3f, 0)
            .addVboAttribute("a_offsetTexCoord", Geometry::AttrType::Vec2f, 3)
            .addVboAttribute("a_offsetColor", Geometry::AttrType::Vec3f, 5)
            .addVboAttribute("a_offsetScale", Geometry::AttrType::Float, 8);

        geometryBuilder.build(_geometry);

        struct Vertex
        {
            glm::vec2 position;
            glm::vec2 texCoord;
        };

        std::array<Vertex, 4> vertices
        {{
            { { +letterSize.x,             0 }, { texCoord.x, texCoord.y } },
            { {             0,             0 }, {          0, texCoord.y } },
            { { +letterSize.x, +letterSize.y }, { texCoord.x,          0 } },
            { {             0, +letterSize.y }, {          0,          0 } }
        }};

        std::array<int, 6> indices{{ 1,0,2,  1,3,2 }};

        std::vector<Vertex> letterVertices;
        letterVertices.reserve(indices.size()); // pre-allocate

        for (int index : indices)
            letterVertices.push_back(vertices[index]);

        _geometry.updateBuffer(0, letterVertices);
        _geometry.setPrimitiveCount(letterVertices.size());

    }

    constexpr std::size_t preAllocatedSize = 1024;

    _lettersOffsetColored.reserve(preAllocatedSize); // <= pre-allocate
    _lettersOffsetBackground.reserve(preAllocatedSize * 8); // <= pre-allocate

    _lettersTexCoordMap =
    {
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

void TextRenderer::setMatricesData(const Camera::MatricesData& matricesData)
{
    _matricesData = matricesData;
}

//

void TextRenderer::push(
    const glm::vec2& position,
    std::string_view message,
    const glm::vec3& color,
    float scale /* = 1.0f */,
    TextAllign allign /* = TextAllign::left */)
{
    // TODO: support text align

    static_cast<void>(allign); // unused

    const glm::vec2 gridSize = { 16, 16 };
    const glm::vec2 letterSize = glm::vec2(_texture->getSize()) / gridSize;

    std::vector<float> allLinesWidth;
    allLinesWidth.reserve(64);
    allLinesWidth.push_back(0.0f);

    float currWidth = 0.0f;
    for (char letter : message)
    {
        if (letter == '\n')
        {
            currWidth = 0.0f;
            allLinesWidth.push_back(0.0f);
            continue;
        }

        currWidth += letterSize.x * scale;
        if (allLinesWidth.back() < currWidth)
            allLinesWidth.back() = currWidth;
    }

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
        for (int stepY = -range; stepY <= +range; stepY += range)
        for (int stepX = -range; stepX <= +range; stepX += range)
        {
            if (stepX == 0 && stepY == 0)
                continue;

            const glm::vec3 pos0 =
            {
                currPos.x + scale * stepX,
                currPos.y + scale * stepY,
                0.0f
            };

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

    if (!_shader)
        D_THROW(std::runtime_error, "shader not setup");

    _shader->bind();
    _shader->setUniform("u_composedMatrix", _matricesData.composed);

    _texture->bind();

    _geometry.updateBuffer(1, _lettersOffsetBackground);
    _geometry.setInstancedCount(_lettersOffsetBackground.size());

    _geometry.render();

    _geometry.updateBuffer(1, _lettersOffsetColored);
    _geometry.setInstancedCount(_lettersOffsetColored.size());

    _geometry.render();
}
