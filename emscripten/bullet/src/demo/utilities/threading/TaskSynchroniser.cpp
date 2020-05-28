
#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD
#   error "exclude this file to build natively or with multi thread support"
#endif

#include "TaskSynchroniser.hpp"

#include <chrono>

TaskSynchroniser::ScopedLockedNotifier::ScopedLockedNotifier(TaskSynchroniser& data)
    : _data(data)
{
    _data._mutex.lock();
}

TaskSynchroniser::ScopedLockedNotifier::~ScopedLockedNotifier()
{
    _data.notify();
    _data._mutex.unlock();
}

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
                return false;
    }
    return true;
}

bool TaskSynchroniser::waitUntilNotified(float seconds /*= 0.0f*/)
{
    std::unique_lock<std::mutex> lock(_mutex);
    return waitUntilNotified(lock, seconds);
}

void TaskSynchroniser::notify()
{
    _notified = true;
    _condVar.notify_one();
}

std::unique_lock<std::mutex> TaskSynchroniser::makeScopedLock()
{
    return std::unique_lock<std::mutex>(this->_mutex);
}

TaskSynchroniser::ScopedLockedNotifier TaskSynchroniser::makeScopedLockNotifier()
{
    return ScopedLockedNotifier(*this);
}

bool TaskSynchroniser::isNotified() const
{
    return _notified;
}
