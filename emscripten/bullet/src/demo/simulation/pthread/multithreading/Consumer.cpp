
#include "Consumer.hpp"

#include <chrono>

namespace multithreading
{

    Consumer::Consumer(IProducer& producer)
        : _producer(producer)
    {
        _thread = std::thread(&Consumer::_threadedMethod, this);

        _running = false; // the consumer's thread will set it to true

        // here we wait for the thread to be running
        while (!_running)
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    Consumer::~Consumer()
    {
        quit();
    }

    //
    //

    void Consumer::execute(const WorkCallback& work)
    {
        auto lockNotifier = _waitProducer.makeScopedLockNotifier();

        // this part is locked and will notify at then end of the scope

        _work = work;
    }

    void Consumer::quit()
    {
        if (!_running)
            return;

        {
            auto lockNotifier = _waitProducer.makeScopedLockNotifier();

            // this part is locked and will notify at then end of the scope

            _running = false;
        }

        if (_thread.joinable())
            _thread.join();
    }

    //
    //

    bool Consumer::isRunning() const
    {
        return _running;
    }

    bool Consumer::isAvailable() const
    {
        return !_waitProducer.isNotified();
    }

    //
    //

    void Consumer::_threadedMethod()
    {
        _running = true;

        auto lock = _waitProducer.makeScopedLock();

        // this part is locked

        while (_running)
        {
            // wait -> release the lock for other thread(s)
            _waitProducer.waitUntilNotified(lock);

            // this part is locked

            if (!_running)
                break; // quit scenario

            _work();

            _producer._notifyWorkDone(this);
        }
    }

};
