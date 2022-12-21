
#include "ThreadSynchroniser.hpp"

#include <chrono>
#include <cstdint>

namespace multithreading {

//
//
// ScopedLockedNotifier

ThreadSynchroniser::ScopedLockedNotifier::ScopedLockedNotifier(
  ThreadSynchroniser& synchroniser)
  : _synchroniser(synchroniser) {
  _synchroniser._mutex.lock(); // scoped lock part
}

ThreadSynchroniser::ScopedLockedNotifier::~ScopedLockedNotifier() {
  // added value compared to a simple scoped lock
  // -> we notify before unlocking the mutex
  _synchroniser.notify();

  _synchroniser._mutex.unlock(); // scoped lock part
}

// ScopedLockedNotifier
//
//

bool ThreadSynchroniser::waitUntilNotified(std::unique_lock<std::mutex>& lock,
                                           float seconds /*= 0.0f*/) {
  _notified = false;

  // no need to wait for a timeout
  if (seconds <= 0.0f) {
    while (!_notified) // loop to avoid spurious wakeups
      _condVar.wait(lock);

    return true;
  }

  // we need to wait for a timeout
  const int64_t millisecondsToWait = int64_t(seconds * 1000.0f);
  auto timeoutPoint = std::chrono::system_clock::now();
  timeoutPoint += std::chrono::milliseconds(millisecondsToWait);

  while (!_notified) // loop to avoid spurious wakeups
    if (_condVar.wait_until(lock, timeoutPoint) == std::cv_status::timeout)
      return false; // we did time out

  return true; // we did not time out
}

void ThreadSynchroniser::notify() {
  _notified = true;
  _condVar.notify_one();
}

std::unique_lock<std::mutex> ThreadSynchroniser::makeScopedLock() {
  return std::unique_lock<std::mutex>(_mutex);
}

ThreadSynchroniser::ScopedLockedNotifier
ThreadSynchroniser::makeScopedLockNotifier() {
  return ScopedLockedNotifier(*this);
}

bool ThreadSynchroniser::isNotified() const { return _notified; }

} // namespace multithreading
