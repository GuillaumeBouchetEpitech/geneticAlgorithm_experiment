
#include "ThreadSynchroniser.hpp"

#include <chrono>

namespace multithreading
{

    //
    //
    // ScopedLockedNotifier

    ThreadSynchroniser::ScopedLockedNotifier::ScopedLockedNotifier(ThreadSynchroniser& synchroniser)
        : _synchroniser(synchroniser)
    {
        _synchroniser._mutex.lock(); // scoped lock part
    }

    ThreadSynchroniser::ScopedLockedNotifier::~ScopedLockedNotifier()
    {
        // added value compared to a simple scoped lock
        // -> we notify before unlocking the mutex
        _synchroniser.notify();

        _synchroniser._mutex.unlock(); // scoped lock part
    }

    // ScopedLockedNotifier
    //
    //

    bool ThreadSynchroniser::waitUntilNotified(std::unique_lock<std::mutex>& lock, float seconds /*= 0.0f*/)
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

    void ThreadSynchroniser::notify()
    {
        _notified = true;
        _condVar.notify_one();
    }

    std::unique_lock<std::mutex> ThreadSynchroniser::makeScopedLock()
    {
        return std::unique_lock<std::mutex>(_mutex);
    }

    ThreadSynchroniser::ScopedLockedNotifier ThreadSynchroniser::makeScopedLockNotifier()
    {
        return ScopedLockedNotifier(*this);
    }

    bool ThreadSynchroniser::isNotified() const
    {
        return _notified;
    }

};
