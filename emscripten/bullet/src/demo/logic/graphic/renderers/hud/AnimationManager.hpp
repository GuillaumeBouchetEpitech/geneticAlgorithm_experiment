
#pragma once

#include "framework/containers/weak_ref_data_pool.hpp"

#include <functional>

class AnimationManager
{
public:
  using Callback = std::function<void(float)>;

private:
  struct Animation
  {
    float delayLeft;
    float timeLeft;
    float duration;
    Callback callback;

    Animation(
      float inDelayLeft,
      float inTimeLeft,
      float inDuration,
      const Callback& inCallback);

    Animation(const Animation& other) = delete;
    Animation(Animation&& other);
    ~Animation() = default;
    Animation& operator=(const Animation& other) = delete;
    Animation& operator=(Animation&& other);

  };

  using AnimationsPool = weak_ref_data_pool<Animation, Animation, 128, false>;

public:
  using AnimationRef = AnimationsPool::weak_ref;

private:
  AnimationsPool _animationsPool;

public:
  AnimationManager() = default;
  ~AnimationManager() = default;

public:
  AnimationRef push(float delay, float duration, const Callback& callback);
  void remove(AnimationRef animRef);
  AnimationRef push(AnimationRef animRef, float delay, float duration, const Callback& callback);

public:
  void update(float elaspedTime);
  void render() const;
};
