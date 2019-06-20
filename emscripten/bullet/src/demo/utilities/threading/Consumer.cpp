
#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD
#	error "exclude this file to build natively or with multi thread support"
#endif

#include "Consumer.hpp"

#include "Producer.hpp"

#include <chrono>

Consumer::Consumer(Producer& producer)
	: _producer(producer)
{
    _thread = std::thread(&Consumer::run, this);

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

void	Consumer::run()
{
    _running = true;

    auto lock = _waitProducer.makeScopedLock();
    while (_running)
    {
        _waitProducer.waitUntilNotified(lock);

        if (!_running)
            break;

        _work();

        _producer.notifyWorkDone(this);
    }
}
