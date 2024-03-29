
#pragma once

#include "framework/graphic/Geometry.hpp"
#include "framework/graphic/ShaderProgram.hpp"
#include "framework/graphic/Texture.hpp"

#include "framework/graphic/camera/Camera.hpp"

#include "framework/helpers/GLMath.hpp"

#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

class Shader;

struct TextRenderer {
  // public:
  //   enum class TextAllign {
  //     left = 0,
  //     center,
  //     right,
  //   };

private:
  struct LetterOffset {
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec4 color;
    float scale;
  };
  using LettersOffsets = std::vector<LetterOffset>;

private:
  LettersOffsets _lettersOffsetColored;
  LettersOffsets _lettersOffsetBackground;
  std::unordered_map<char, glm::vec2> _lettersTexCoordMap;

  std::shared_ptr<ShaderProgram> _shader;

  std::shared_ptr<Texture> _texture;

  Geometry _geometry;

  Camera::MatricesData _matricesData;

public:
  TextRenderer() = default;

public:
  void initialise();
  void setMatricesData(const Camera::MatricesData& matricesData);

public:
  void push(
    const glm::vec2& position, std::string_view message, const glm::vec4& color,
    float scale = 1.0f, float zDepth = 0.0f,
    const glm::vec3& inBackColor = {0, 0, 0}
    // TextAllign allign = TextAllign::left
  );

  struct Rectangle {
    glm::vec2 pos;
    glm::vec2 size;
  };

  void getSizes(
    std::vector<Rectangle>& outRectangles, const glm::vec2& position,
    std::string_view message, float scale = 1.0f);

  void clear();

  void render();
};
