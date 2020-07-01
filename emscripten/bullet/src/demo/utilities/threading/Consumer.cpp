
#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD
#   error "exclude this file to build natively or with multi thread support"
#endif

#include "Consumer.hpp"

#include "Producer.hpp"

#include <chrono>

Consumer::Consumer(Producer& producer)
    : _producer(producer)
{
    _thread = std::thread(&Consumer::threadedMethod, this);

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

void Consumer::execute(const t_task::t_work& work)
{
    auto lockNotifier = _waitProducer.makeScopedLockNotifier();
    _work = work;
}

void Consumer::quit()
{
    if (!_running)
        return;

    {
        auto lockNotifier = _waitProducer.makeScopedLockNotifier();

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

void Consumer::threadedMethod()
{
    _running = true;

    auto lock = _waitProducer.makeScopedLock();
    while (_running)
    {
        _waitProducer.waitUntilNotified(lock);

         // this part is locked

        if (!_running)
            break; // quit scenario

        _work();

        _producer.notifyWorkDone(this);
    }
}
