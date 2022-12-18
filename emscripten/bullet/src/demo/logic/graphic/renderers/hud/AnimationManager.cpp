
#include "AnimationManager.hpp"

#include "framework/math/clamp.hpp"

AnimationManager::Animation::Animation(float inDelayLeft, float inTimeLeft,
                                       float inDuration,
                                       const Callback& inCallback) {
  delayLeft = inDelayLeft;
  timeLeft = inTimeLeft;
  duration = inDuration;
  callback = inCallback;
}

AnimationManager::Animation::Animation(Animation&& other) {
  std::swap(delayLeft, other.delayLeft);
  std::swap(timeLeft, other.timeLeft);
  std::swap(duration, other.duration);
  std::swap(callback, other.callback);
}

AnimationManager::Animation&
AnimationManager::Animation::operator=(Animation&& other) {
  if (&other == this)
    return *this;

  std::swap(delayLeft, other.delayLeft);
  std::swap(timeLeft, other.timeLeft);
  std::swap(duration, other.duration);
  std::swap(callback, other.callback);

  return *this;
}

AnimationManager::AnimationRef
AnimationManager::push(float delay, float duration, const Callback& callback) {
  return _animationsPool.acquire(delay, duration, duration, callback);
}

void AnimationManager::remove(AnimationRef animRef) {
  if (animRef)
    _animationsPool.release(animRef);
}

AnimationManager::AnimationRef
AnimationManager::push(AnimationRef animRef, float delay, float duration,
                       const Callback& callback) {
  remove(animRef);
  return push(delay, duration, callback);
}

void AnimationManager::update(float elaspedTime) {
  _animationsPool.filter([elaspedTime](Animation& currAnim) -> bool {
    if (currAnim.delayLeft > 0.0f) {
      currAnim.delayLeft -= elaspedTime;
      if (currAnim.delayLeft < 0.0f)
        currAnim.delayLeft = 0.0f;
    } else {
      currAnim.timeLeft -= elaspedTime;
      if (currAnim.timeLeft < 0.0f)
        currAnim.timeLeft = 0.0f;
    }

    return currAnim.timeLeft > 0.0f;
  });
}

void AnimationManager::render() const {
  _animationsPool.for_each([](const Animation& currAnim) -> void {
    if (currAnim.delayLeft > 0.0f)
      return;

    const float currTime =
      (currAnim.duration - currAnim.timeLeft) / currAnim.duration;
    currAnim.callback(clamp(currTime, 0.0f, 1.0f));
  });
}
