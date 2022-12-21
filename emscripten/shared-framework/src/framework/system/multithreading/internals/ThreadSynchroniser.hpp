
#pragma once

#include "framework/system/NonCopyable.hpp"

#include <condition_variable>
#include <mutex>

namespace multithreading {

// this class handle all locking and conditional variable interactions
class ThreadSynchroniser : public NonCopyable {
private:
  std::mutex _mutex;
  std::condition_variable _condVar;
  bool _notified = false;

public:
  // this class act like a scoped lock but notify before unlocking
  class ScopedLockedNotifier {
    // friendship since we need access to the _mutex
    friend ThreadSynchroniser;

  private:
    ThreadSynchroniser& _synchroniser;

  private:
    ScopedLockedNotifier(ThreadSynchroniser& synchroniser);

  public:
    ~ScopedLockedNotifier();
  };

public:
  ThreadSynchroniser() = default;

public:
  bool waitUntilNotified(std::unique_lock<std::mutex>& lock,
                         float seconds = 0.0f);
  void notify();

public:
  std::unique_lock<std::mutex> makeScopedLock();
  ScopedLockedNotifier makeScopedLockNotifier();
  bool isNotified() const;
};

} // namespace multithreading
