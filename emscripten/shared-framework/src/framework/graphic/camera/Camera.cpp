
#include "Camera.hpp"

#include "sceneToScreen.hpp"

void Camera::lookAt(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up)
{
  if (_eye != eye || _target != target)
  {
    const glm::vec3 diff = _target - _eye;

    _forwardAxis = diff;

    const float magnitude = glm::length(diff);
    if (magnitude > 0.0f)
      _forwardAxis /= magnitude;
  }

  _eye = eye;
  _target = target;
  _up = up;
}

void Camera::computeMatrices()
{
  { // scene

    if (_dirtyProjectionMatrices)
    {
      _dirtyProjectionMatrices = false;

      if (_projectionType == ProjectionType::perspective)
      {
        const float aspectRatio = _viewportSize.x / _viewportSize.y;
        auto& proj = _projectionData.perspective;
        _matricesData.projection = glm::perspective(
          glm::radians(proj.fovy),
          aspectRatio,
          proj.near,
          proj.far);
      }
      else
      {
        auto& proj = _projectionData.orthographic;
        _matricesData.projection = glm::ortho(
          proj.left,
          proj.right,
          proj.bottom,
          proj.top,
          proj.near,
          proj.far);
      }
    }

    _matricesData.view = glm::lookAt(_eye, _target, _up);
    _matricesData.composed = _matricesData.projection * _matricesData.view;

    _matricesData.invComposed = glm::inverse(_matricesData.composed);

    _frustumCulling.calculateFrustum(_matricesData.projection, _matricesData.view);

  } // scene

}

const IFrustumCulling& Camera::getFrustumCulling() const
{
  return _frustumCulling;
}

bool Camera::sceneToHudCoord(const glm::vec3& scenePos, glm::vec3& hudPos) const
{
  const glm::vec2 hudOrigin(0, 0); // TODO: hardcoded

  return sceneToScreen(
      scenePos,
      _matricesData.view,
      _matricesData.projection,
      hudOrigin, _viewportSize,
      hudPos);
}

void Camera::hudToSceneCoord(const glm::vec2& hudPos, glm::vec3& from, glm::vec3& to)
{
  glm::vec2 actualPos = (glm::vec2(hudPos.x, hudPos.y) / _viewportSize) * 2.0f - 1.0f;
  //origin is top-left and +y mouse is down
  actualPos.y = -actualPos.y;

  glm::vec4 fromV4 = _matricesData.invComposed * glm::vec4(actualPos, -1.0f, 1.0f);
  glm::vec4 toV4 = _matricesData.invComposed * glm::vec4(actualPos, 1.0f, 1.0f);

  // perspective divide ("normalize" homogeneous coordinates)
  fromV4 /= fromV4.w;
  toV4 /= toV4.w;

  from = fromV4;
  to = toV4;
}

//

const glm::vec3& Camera::getEye() const { return _eye; }
const glm::vec3& Camera::getTarget() const { return _target; }
const glm::vec3& Camera::getUp() const { return _up; }
const glm::vec3& Camera::getForwardAxis() const { return _forwardAxis; }

void Camera::setSize(int width, int height)
{
  _viewportSize.x = float(width);
  _viewportSize.y = float(height);
  _dirtyProjectionMatrices = true;
}

const glm::vec2& Camera::getSize() const { return _viewportSize; }

void Camera::setPerspective(
  float fovy,
  float near,
  float far)
{
  _projectionType = ProjectionType::perspective;
  _projectionData.perspective.fovy = fovy;
  _projectionData.perspective.near = near;
  _projectionData.perspective.far = far;
  _dirtyProjectionMatrices = true;
}

void Camera::setOrthographic(
  float left,
  float right,
  float bottom,
  float top,
  float near,
  float far)
{
  _projectionType = ProjectionType::orthographic;
  _projectionData.orthographic.left = left;
  _projectionData.orthographic.right = right;
  _projectionData.orthographic.bottom = bottom;
  _projectionData.orthographic.top = top;
  _projectionData.orthographic.near = near;
  _projectionData.orthographic.far = far;
  _dirtyProjectionMatrices = true;
}

Camera::ProjectionType Camera::getProjectionType() const
{
  return _projectionType;
}

//

const Camera::MatricesData& Camera::getMatricesData() const
{
  return _matricesData;
}
