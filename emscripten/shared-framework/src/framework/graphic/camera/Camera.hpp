
#pragma once

#include "FrustumCulling.hpp"

#include "framework/helpers/GLMath.hpp"

class Camera {
public:
  struct MatricesData {
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 composed;
    glm::mat4 invComposed;
  };

public:
  enum class ProjectionType {
    perspective,
    orthographic,
  };

  struct Perspective {
    float fovy;
    float near;
    float far;
  };

  struct Orthographic {
    float left;
    float right;
    float bottom;
    float top;
    float near;
    float far;
  };

public:
  Camera() = default;
  ~Camera() = default;

public:
  void lookAt(const glm::vec3& eye, const glm::vec3& target,
              const glm::vec3& up);
  void computeMatrices();

  const IFrustumCulling& getFrustumCulling() const;

  bool sceneToHudCoord(const glm::vec3& scenePos, glm::vec3& hudPos) const;
  void hudToSceneCoord(const glm::vec2& pos2d, glm::vec3& from, glm::vec3& to);

public:
  const glm::vec3& getEye() const;
  const glm::vec3& getTarget() const;
  const glm::vec3& getUp() const;
  const glm::vec3& getForwardAxis() const;

  void setSize(uint32_t width, uint32_t height);
  const glm::vec2& getSize() const;

  void setPerspective(float fovy, float near, float far);
  void setOrthographic(float left, float right, float bottom, float top,
                       float near, float far);
  ProjectionType getProjectionType() const;

public:
  const MatricesData& getMatricesData() const;

private:
  glm::vec2 _viewportSize = {800.0f, 600.0f};

  glm::vec3 _eye = {0.0f, 0.0f, 0.0f};
  glm::vec3 _target = {1.0f, 0.0f, 0.0f};
  glm::vec3 _up = {0.0f, 0.0f, 1.0f};

  glm::vec3 _forwardAxis = {1.0f, 0.0f, 0.0f};

  bool _dirtyProjectionMatrices = true;

  MatricesData _matricesData;

  ProjectionType _projectionType;

  union ProjectionData {
    Perspective perspective;
    Orthographic orthographic;
  } _projectionData;

  FrustumCulling _frustumCulling;
};
