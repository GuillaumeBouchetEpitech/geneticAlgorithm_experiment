
#include "TaskSynchroniser.hpp"

#include <chrono>

namespace multiThreading
{

    //
    //
    // ScopedLockedNotifier

    TaskSynchroniser::ScopedLockedNotifier::ScopedLockedNotifier(TaskSynchroniser& synchroniser)
        : _synchroniser(synchroniser)
    {
        _synchroniser._mutex.lock(); // scoped lock part
    }

    TaskSynchroniser::ScopedLockedNotifier::~ScopedLockedNotifier()
    {
        // added value compared to a simple scoped lock
        // -> we notify before unlocking the mutex
        _synchroniser.notify();

        _synchroniser._mutex.unlock(); // scoped lock part
    }

    // ScopedLockedNotifier
    //
    //

    bool TaskSynchroniser::waitUntilNotified(std::unique_lock<std::mutex>& lock, float seconds /*= 0.0f*/)
    {
        _notified = false;

        if (seconds <= 0.0f)
        {
            while (!_notified) // loop to avoid spurious wakeups
                _condVar.wait(lock);
        }
        else
        {
            long int timeToWait = seconds * 1000;
            auto timeout = std::chrono::system_clock::now();
            timeout += std::chrono::milliseconds(timeToWait);

            while (!_notified) // loop to avoid spurious wakeups
                if (_condVar.wait_until(lock, timeout) == std::cv_status::timeout)
                    return false; // we did time out
        }
        return true; // we did not time out
    }

    void TaskSynchroniser::notify()
    {
        _notified = true;
        _condVar.notify_one();
    }

    std::unique_lock<std::mutex> TaskSynchroniser::makeScopedLock()
    {
        return std::unique_lock<std::mutex>(_mutex);
    }

    TaskSynchroniser::ScopedLockedNotifier TaskSynchroniser::makeScopedLockNotifier()
    {
        return ScopedLockedNotifier(*this);
    }

    bool TaskSynchroniser::isNotified() const
    {
        return _notified;
    }

};
