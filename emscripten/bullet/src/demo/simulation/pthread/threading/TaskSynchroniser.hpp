
#pragma once

#include <mutex>
#include <condition_variable>

// this class handle all locking and conditional variable interactions
class TaskSynchroniser
{
private:
    std::mutex              _mutex;
    std::condition_variable _condVar;
    bool                    _notified = false;

public:
    // this class act like a scoped lock but notify before unlocking
    class ScopedLockedNotifier
    {
    private:
        TaskSynchroniser& _data;

    public:
        ScopedLockedNotifier(TaskSynchroniser& data);
        ~ScopedLockedNotifier();
    };

public:
    TaskSynchroniser() = default;

public:
    bool waitUntilNotified(std::unique_lock<std::mutex>& lock, float seconds = 0.0f);
    bool waitUntilNotified(float seconds = 0.0f);
    void notify();

public:
    std::unique_lock<std::mutex> makeScopedLock();
    ScopedLockedNotifier makeScopedLockNotifier();
    bool isNotified() const;
};
