
#include "TextRenderer.hpp"

#include "demo/logic/Context.hpp"
#include "demo/logic/graphicIds.hpp"

#include "framework/system/ErrorHandler.hpp"
#include "framework/system/asValue.hpp"

void
TextRenderer::initialise() {

  auto& resourceManager = Context::get().graphic.resourceManager;

  _shader = resourceManager.getShader(asValue(ShaderIds::hudText));

  _texture = resourceManager.getTexture(0);

  const glm::vec2 gridSize = {16, 16};
  const glm::vec2 letterSize = glm::vec2(_texture->getSize()) / gridSize;
  const glm::vec2 texCoord = letterSize / glm::vec2(_texture->getSize());

  {
    struct Vertex {
      glm::vec2 position;
      glm::vec2 texCoord;
    };

    std::array<Vertex, 4> vertices{
      {{{+letterSize.x, 0}, {texCoord.x, texCoord.y}},
       {{0, 0}, {0, texCoord.y}},
       {{+letterSize.x, +letterSize.y}, {texCoord.x, 0}},
       {{0, +letterSize.y}, {0, 0}}}};

    std::array<int, 6> indices{{1, 0, 2, 1, 3, 2}};

    std::vector<Vertex> letterVertices;
    letterVertices.reserve(indices.size()); // pre-allocate

    for (int index : indices)
      letterVertices.push_back(vertices.at(index));

    auto geoDef =
      resourceManager.getGeometryDefinition(asValue(GeometryIds::hudText));
    _geometry.initialise(*_shader, geoDef);
    _geometry.updateBuffer(0, letterVertices);
    _geometry.setPrimitiveCount(letterVertices.size());
  }

  constexpr std::size_t preAllocatedSize = 1024;

  _lettersOffsetColored.reserve(preAllocatedSize);        // <= pre-allocate
  _lettersOffsetBackground.reserve(preAllocatedSize * 8); // <= pre-allocate

  _lettersTexCoordMap = {
    {' ', {0 * texCoord.x, 0 * texCoord.y}},
    {'!', {1 * texCoord.x, 0 * texCoord.y}},
    {'\"', {2 * texCoord.x, 0 * texCoord.y}},
    {'#', {3 * texCoord.x, 0 * texCoord.y}},
    {'$', {4 * texCoord.x, 0 * texCoord.y}},
    {'%', {5 * texCoord.x, 0 * texCoord.y}},
    {'&', {6 * texCoord.x, 0 * texCoord.y}},
    {'\'', {7 * texCoord.x, 0 * texCoord.y}},
    {'(', {8 * texCoord.x, 0 * texCoord.y}},
    {')', {9 * texCoord.x, 0 * texCoord.y}},
    {'*', {10 * texCoord.x, 0 * texCoord.y}},
    {'+', {11 * texCoord.x, 0 * texCoord.y}},
    {',', {12 * texCoord.x, 0 * texCoord.y}},
    {'-', {13 * texCoord.x, 0 * texCoord.y}},
    {'.', {14 * texCoord.x, 0 * texCoord.y}},
    {'/', {15 * texCoord.x, 0 * texCoord.y}},

    {'0', {0 * texCoord.x, 1 * texCoord.y}},
    {'1', {1 * texCoord.x, 1 * texCoord.y}},
    {'2', {2 * texCoord.x, 1 * texCoord.y}},
    {'3', {3 * texCoord.x, 1 * texCoord.y}},
    {'4', {4 * texCoord.x, 1 * texCoord.y}},
    {'5', {5 * texCoord.x, 1 * texCoord.y}},
    {'6', {6 * texCoord.x, 1 * texCoord.y}},
    {'7', {7 * texCoord.x, 1 * texCoord.y}},
    {'8', {8 * texCoord.x, 1 * texCoord.y}},
    {'9', {9 * texCoord.x, 1 * texCoord.y}},
    {':', {10 * texCoord.x, 1 * texCoord.y}},
    {';', {11 * texCoord.x, 1 * texCoord.y}},
    {'<', {12 * texCoord.x, 1 * texCoord.y}},
    {'=', {13 * texCoord.x, 1 * texCoord.y}},
    {'>', {14 * texCoord.x, 1 * texCoord.y}},
    {'?', {15 * texCoord.x, 1 * texCoord.y}},

    {'@', {0 * texCoord.x, 2 * texCoord.y}},
    {'A', {1 * texCoord.x, 2 * texCoord.y}},
    {'B', {2 * texCoord.x, 2 * texCoord.y}},
    {'C', {3 * texCoord.x, 2 * texCoord.y}},
    {'D', {4 * texCoord.x, 2 * texCoord.y}},
    {'E', {5 * texCoord.x, 2 * texCoord.y}},
    {'F', {6 * texCoord.x, 2 * texCoord.y}},
    {'G', {7 * texCoord.x, 2 * texCoord.y}},
    {'H', {8 * texCoord.x, 2 * texCoord.y}},
    {'I', {9 * texCoord.x, 2 * texCoord.y}},
    {'J', {10 * texCoord.x, 2 * texCoord.y}},
    {'K', {11 * texCoord.x, 2 * texCoord.y}},
    {'L', {12 * texCoord.x, 2 * texCoord.y}},
    {'M', {13 * texCoord.x, 2 * texCoord.y}},
    {'N', {14 * texCoord.x, 2 * texCoord.y}},
    {'O', {15 * texCoord.x, 2 * texCoord.y}},

    {'P', {0 * texCoord.x, 3 * texCoord.y}},
    {'Q', {1 * texCoord.x, 3 * texCoord.y}},
    {'R', {2 * texCoord.x, 3 * texCoord.y}},
    {'S', {3 * texCoord.x, 3 * texCoord.y}},
    {'T', {4 * texCoord.x, 3 * texCoord.y}},
    {'U', {5 * texCoord.x, 3 * texCoord.y}},
    {'V', {6 * texCoord.x, 3 * texCoord.y}},
    {'W', {7 * texCoord.x, 3 * texCoord.y}},
    {'X', {8 * texCoord.x, 3 * texCoord.y}},
    {'Y', {9 * texCoord.x, 3 * texCoord.y}},
    {'Z', {10 * texCoord.x, 3 * texCoord.y}},
    {'[', {11 * texCoord.x, 3 * texCoord.y}},
    {'\\', {12 * texCoord.x, 3 * texCoord.y}},
    {']', {13 * texCoord.x, 3 * texCoord.y}},
    {'^', {14 * texCoord.x, 3 * texCoord.y}},
    {'_', {15 * texCoord.x, 3 * texCoord.y}},

    {'`', {0 * texCoord.x, 4 * texCoord.y}},
    {'a', {1 * texCoord.x, 4 * texCoord.y}},
    {'b', {2 * texCoord.x, 4 * texCoord.y}},
    {'c', {3 * texCoord.x, 4 * texCoord.y}},
    {'d', {4 * texCoord.x, 4 * texCoord.y}},
    {'e', {5 * texCoord.x, 4 * texCoord.y}},
    {'f', {6 * texCoord.x, 4 * texCoord.y}},
    {'g', {7 * texCoord.x, 4 * texCoord.y}},
    {'h', {8 * texCoord.x, 4 * texCoord.y}},
    {'i', {9 * texCoord.x, 4 * texCoord.y}},
    {'j', {10 * texCoord.x, 4 * texCoord.y}},
    {'k', {11 * texCoord.x, 4 * texCoord.y}},
    {'l', {12 * texCoord.x, 4 * texCoord.y}},
    {'m', {13 * texCoord.x, 4 * texCoord.y}},
    {'n', {14 * texCoord.x, 4 * texCoord.y}},
    {'o', {15 * texCoord.x, 4 * texCoord.y}},

    {'p', {0 * texCoord.x, 5 * texCoord.y}},
    {'q', {1 * texCoord.x, 5 * texCoord.y}},
    {'r', {2 * texCoord.x, 5 * texCoord.y}},
    {'s', {3 * texCoord.x, 5 * texCoord.y}},
    {'t', {4 * texCoord.x, 5 * texCoord.y}},
    {'u', {5 * texCoord.x, 5 * texCoord.y}},
    {'v', {6 * texCoord.x, 5 * texCoord.y}},
    {'w', {7 * texCoord.x, 5 * texCoord.y}},
    {'x', {8 * texCoord.x, 5 * texCoord.y}},
    {'y', {9 * texCoord.x, 5 * texCoord.y}},
    {'z', {10 * texCoord.x, 5 * texCoord.y}},
    {'{', {11 * texCoord.x, 5 * texCoord.y}},
    {'|', {12 * texCoord.x, 5 * texCoord.y}},
    {'}', {13 * texCoord.x, 5 * texCoord.y}},
    {'~', {14 * texCoord.x, 5 * texCoord.y}},

  };
}

void
TextRenderer::setMatricesData(const Camera::MatricesData& matricesData) {
  _matricesData = matricesData;
}

//

void
TextRenderer::push(
  const glm::vec2& inPosition, std::string_view inMessage,
  const glm::vec4& inColor, float inScale /* = 1.0f */, float zDepth, /*= 0.0f*/
  const glm::vec3& inBackColor /*= {0,0,0}*/
                               // TextAllign allign /* = TextAllign::left */
) {
  // TODO: support text align

  const glm::vec2 gridSize = {16, 16};
  const glm::vec2 letterSize = glm::vec2(_texture->getSize()) / gridSize;

  std::vector<float> allLinesWidth;
  allLinesWidth.reserve(64);
  allLinesWidth.push_back(0.0f);

  float currWidth = 0.0f;
  for (char letter : inMessage) {
    if (letter == '\n') {
      currWidth = 0.0f;
      allLinesWidth.push_back(0.0f);
      continue;
    }

    currWidth += letterSize.x * inScale;
    allLinesWidth.back() = std::min(allLinesWidth.back(), currWidth);
  }

  glm::vec2 currPos = inPosition;

  for (char letter : inMessage) {
    if (letter == '\n') {
      currPos.x = inPosition.x;
      currPos.y -= letterSize.y * inScale;
      continue;
    }

    auto it = _lettersTexCoordMap.find(letter);

    if (it == _lettersTexCoordMap.end())
      D_THROW(
        std::runtime_error, "fail to find a letter"
                              << ", letter=" << letter);

    const auto& texCoord = it->second;

    _lettersOffsetColored.push_back(
      {glm::vec3(currPos, zDepth), texCoord, inColor, inScale});

    const glm::vec4 blackColor = glm::vec4(inBackColor, inColor.a);
    constexpr int range = 1;
    glm::vec3 outlinePos;
    outlinePos.z = zDepth - 0.01f;
    for (int stepY = -range; stepY <= +range; stepY += range) {

      outlinePos.y = currPos.y + inScale * stepY;

      for (int stepX = -range; stepX <= +range; stepX += range) {
        if (stepX == 0 && stepY == 0)
          continue;

        outlinePos.x = currPos.x + inScale * stepX;

        _lettersOffsetBackground.push_back(
          {outlinePos, texCoord, blackColor, inScale});
      }
    }

    currPos.x += letterSize.x * inScale;
  }
}

void
TextRenderer::getSizes(
  std::vector<Rectangle>& outRectangles, const glm::vec2& inPosition,
  std::string_view inMessage, float inScale /*= 1.0f*/) {
  const glm::vec2 gridSize = {16, 16};
  const glm::vec2 letterSize = glm::vec2(_texture->getSize()) / gridSize;

  const float sizeY = letterSize.y * inScale;

  outRectangles.clear();
  outRectangles.reserve(64);
  outRectangles.push_back({{inPosition.x, inPosition.y}, {0.0f, sizeY}});

  for (char letter : inMessage) {
    if (letter == '\n') {
      const float lastY = outRectangles.back().pos.y;
      outRectangles.push_back({{inPosition.x, lastY - sizeY}, {0.0f, sizeY}});
      continue;
    }
    outRectangles.back().size.x += letterSize.x * inScale;
  }
}

void
TextRenderer::clear() {
  _lettersOffsetColored.clear();
  _lettersOffsetBackground.clear();
}

void
TextRenderer::render() {
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
