
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
      const float aspectRatio = float(_viewportSize.x) / _viewportSize.y;

      _matrices.scene.projection = glm::perspective(
        glm::radians(_perspective.fovy),
        aspectRatio,
        _perspective.near,
        _perspective.far);
    }

    _matrices.scene.view = glm::lookAt(_eye, _target, _up);
    _matrices.scene.composed = _matrices.scene.projection * _matrices.scene.view;

    _matrices.scene.invComposed = glm::inverse(_matrices.scene.composed);

    _frustumCulling.calculateFrustum(_matrices.scene.projection, _matrices.scene.view);

  } // scene

  { // hud

    if (_dirtyProjectionMatrices)
    {
      _matrices.hud.projection = glm::ortho(0.0f,_viewportSize.x, 0.0f,_viewportSize.y, -2.0f, 2.0f);

      const glm::vec3 eye = { 0.0f, 0.0f, 1.0f };
      const glm::vec3 center = { 0.0f, 0.0f, 0.0f };
      const glm::vec3 upAxis = { 0.0f, 1.0f, 0.0f };
      _matrices.hud.view = glm::lookAt(eye, center, upAxis);

      _matrices.hud.composed = _matrices.hud.projection * _matrices.hud.view;
    }

  } // hud

  _dirtyProjectionMatrices = false;
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
      _matrices.scene.view,
      _matrices.scene.projection,
      hudOrigin, _viewportSize,
      hudPos);
}

void Camera::hudToSceneCoord(const glm::vec2& hudPos, glm::vec3& from, glm::vec3& to)
{
  glm::vec2 actualPos = (glm::vec2(hudPos.x, hudPos.y) / _viewportSize) * 2.0f - 1.0f;
  //origin is top-left and +y mouse is down
  actualPos.y = -actualPos.y;

  glm::vec4 fromV4 = _matrices.scene.invComposed * glm::vec4(actualPos, -1.0f, 1.0f);
  glm::vec4 toV4 = _matrices.scene.invComposed * glm::vec4(actualPos, 1.0f, 1.0f);

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

void Camera::setPerspective(const Perspective& perspective) { _perspective = perspective; }
const Camera::Perspective& Camera::getPerspective() const { return _perspective; }

//

const Camera::MatricesData& Camera::getSceneMatricesData() const
{
  return _matrices.scene;
}

const Camera::MatricesData& Camera::getHudMatricesData() const
{
  return _matrices.hud;
}

