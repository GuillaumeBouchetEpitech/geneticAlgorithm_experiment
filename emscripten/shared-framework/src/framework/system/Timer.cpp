
#include "Timer.hpp"

void Timer::start(float timeLeft) { start(0.0f, timeLeft); }

void Timer::start(float delayLeft, float timeLeft) {
  _delayLeft = delayLeft;
  _duration = _timeLeft = timeLeft;
}

void Timer::stop() { _delayLeft = _duration = _timeLeft = 0.0f; }

void Timer::update(float elapsedTime) {
  if (_delayLeft > 0.0f) {
    _delayLeft -= elapsedTime;
    if (_delayLeft < 0.0f)
      _delayLeft = 0.0f;
  } else if (_timeLeft > 0.0f) {
    _timeLeft -= elapsedTime;
    if (_timeLeft < 0.0f)
      _timeLeft = 0.0f;
  }
}

bool Timer::isDelayDone() const { return _delayLeft == 0.0f; }
bool Timer::isDone() const { return isDelayDone() && _timeLeft == 0.0f; }
bool Timer::isTimerDone() const { return isDone(); }

float Timer::getDelayLeft() const { return _delayLeft; }
float Timer::getTimeLeft() const { return _timeLeft; }
float Timer::getDuration() const { return _duration; }
float Timer::getCoefLeft() const { return _timeLeft / _duration; }
float Timer::getCoefElapsed() const { return 1.0f - getCoefLeft(); }
