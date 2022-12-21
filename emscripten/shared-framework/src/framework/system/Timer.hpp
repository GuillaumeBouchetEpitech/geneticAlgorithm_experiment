
#pragma once

class Timer {
private:
  float _delayLeft = 0.0f;
  float _timeLeft = 0.0f;
  float _duration = 0.0f;

public:
  Timer() = default;
  ~Timer() = default;

public:
  void start(float timeLeft);
  void start(float delayLeft, float timeLeft);
  void stop();

  void update(float elapsedTime);

  bool isDelayDone() const;
  bool isDone() const;
  bool isTimerDone() const;

  float getDelayLeft() const;
  float getTimeLeft() const;
  float getDuration() const;
  float getCoefLeft() const;
  float getCoefElapsed() const;
};
