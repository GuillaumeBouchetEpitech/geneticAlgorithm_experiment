
#pragma once

#include "framework/graphic/Geometry.hpp"
#include "framework/graphic/ShaderProgram.hpp"

#include "framework/graphic/camera/Camera.hpp"

#include "framework/helpers/GLMath.hpp"

#include <memory>
#include <vector>

struct WireframesStackRenderer {
public:
  struct Vertex {
    glm::vec3 position;
    glm::vec4 color;

    Vertex(const glm::vec3& position, const glm::vec4& color)
      : position(position), color(color) {}
  };
  using Vertices = std::vector<Vertex>;

public:
  WireframesStackRenderer() = default;
  ~WireframesStackRenderer() = default;

public:
  void initialise();
  void setMatricesData(const Camera::MatricesData& matricesData);

public:
  void push(const Vertex& vertex);
  void pushLine(const glm::vec3& posA, const glm::vec3& posB,
                const glm::vec4& color);
  void pushLine(const glm::vec3& posA, const glm::vec3& posB,
                const glm::vec3& color);
  void pushCross(const glm::vec3& pos, const glm::vec3& color,
                 float halfExtent);

  // 2d
  void pushLine(const glm::vec2& posA, const glm::vec2& posB,
                const glm::vec4& color);
  void pushLine(const glm::vec2& posA, const glm::vec2& posB,
                const glm::vec3& color);
  void pushRectangle(const glm::vec2& pos, const glm::vec2& size,
                     const glm::vec3& color);
  // 2d

public:
  void flush();

  std::size_t getLinesCount() const;

private:
  std::shared_ptr<ShaderProgram> _shader;
  Geometry _geometry;
  Vertices _vertices;

  Camera::MatricesData _matricesData;
};
