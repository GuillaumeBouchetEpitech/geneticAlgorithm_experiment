
#pragma once

#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD
#	error "exclude this file to build natively or with multi thread support"
#endif

#include <mutex>
#include <condition_variable>

class TaskSynchroniser
{
private:
    std::mutex              _mutex;
    std::condition_variable _condVar;
    bool                    _notified = false;

public:
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
