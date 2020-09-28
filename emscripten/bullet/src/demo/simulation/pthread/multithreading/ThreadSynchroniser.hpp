
#pragma once

#include <mutex>
#include <condition_variable>

namespace multithreading
{

    // this class handle all locking and conditional variable interactions
    class ThreadSynchroniser
    {
    private:
        std::mutex              _mutex;
        std::condition_variable _condVar;
        bool                    _notified = false;

    public:
        // this class act like a scoped lock but notify before unlocking
        class ScopedLockedNotifier
        {
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
        bool waitUntilNotified(std::unique_lock<std::mutex>& lock, float seconds = 0.0f);
        void notify();

    public:
        std::unique_lock<std::mutex> makeScopedLock();
        ScopedLockedNotifier makeScopedLockNotifier();
        bool isNotified() const;
    };

};
