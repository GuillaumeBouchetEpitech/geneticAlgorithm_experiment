
#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD
#   error "exclude this file to build natively or with multi thread support"
#endif

#include "Producer.hpp"

#include <algorithm>
#include <chrono>

#if defined __EMSCRIPTEN__
#   include <emscripten.h>
#endif

Producer::Producer(unsigned int totalCores)
{
    // clamp [1..8]
    const int totalConsumers = std::min(std::max(int(totalCores), 1), 8);

    _consumers.reserve(totalConsumers);
    for (int ii = 0; ii < totalConsumers; ++ii)
        _consumers.push_back(new Consumer(*this));

    _thread = std::thread(&Producer::threadedMethod, this);

    // here we wait for the thread to be running
    while (!_running)
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

Producer::~Producer()
{
    quit();
}

//
//

void Producer::push(const t_task::t_work& work)
{
    auto lockNotifier = _waitOneTask.makeScopedLockNotifier();

    _plannedTasks.push_back(t_task(work));
}

void Producer::push(const t_task::t_work& work, const t_task::t_work& complete)
{
    auto lockNotifier = _waitOneTask.makeScopedLockNotifier();

    _plannedTasks.push_back(t_task(work, complete));
}

void Producer::update()
{
    t_task* current;

    while (!_completedTasks.empty())
    {
        current = &_completedTasks.front();

        // must not be locked as it might push another task
        if (current->_oncomplete)
            current->_oncomplete();

        {
            auto lock = _waitOneTask.makeScopedLock();

            // this is locked in case a push_back happen at the same time
            _completedTasks.pop_front();
        }
    }
}

void Producer::quit()
{
    if (!_running)
        return;

    // clear the planned task(s) and wake up the running thread
    {
        auto lockNotifier = _waitOneTask.makeScopedLockNotifier();
        _plannedTasks.clear();
        _running = false;
    }

    if (_thread.joinable())
        _thread.join();

    for (auto* consumer : _consumers)
    {
        if (consumer->isRunning())
            consumer->quit();
        delete consumer;
    }
    _consumers.clear();
}

void Producer::waitUntilAllCompleted()
{
#if defined __EMSCRIPTEN__

    /**
     * Sad hack to make the pthread simulation works without warnings :(.
     *
     * Done because of this:
     * => https://emscripten.org/docs/porting/pthreads.html#blocking-on-the-main-browser-thread
     */

    while (!_plannedTasks.empty() || !_runningTasks.empty())
        emscripten_sleep(1); // this will yield

#else

    auto lock = _waitAllTask.makeScopedLock();

    // make the (main) thread wait for all tasks to be completed
    while (!_plannedTasks.empty() || !_runningTasks.empty())
        _waitAllTask.waitUntilNotified(lock);

#endif
}

//
//

void Producer::notifyWorkDone(Consumer* consumer)
{
    auto lockNotifier = _waitOneTask.makeScopedLockNotifier(); // we are locking

    // find the task in the "running" list
    auto comparison = [consumer](const t_task& task) { return task._consumer == consumer; };
    auto itTask = std::find_if(_runningTasks.begin(), _runningTasks.end(), comparison);

    if (itTask == _runningTasks.end())
        return; // <= this should never fail

    // move the task from "running" to "completed" (if any)
    if (itTask->_oncomplete)
        _completedTasks.push_back(*itTask);
    _runningTasks.erase(itTask);

    // wake up potentially waiting (main) thread
    if (_plannedTasks.empty() && _runningTasks.empty())
        _waitAllTask.notify();
}

void Producer::threadedMethod()
{
    _running = true;
    auto lock = _waitOneTask.makeScopedLock();
    while (_running)
    {
        _waitOneTask.waitUntilNotified(lock);

        // this part is locked

        if (!_running)
            break; // quit scenario

        while (!_plannedTasks.empty())
        {
            // find any available consumer
            auto comparison = [](const Consumer* consumer) { return consumer->isAvailable(); };
            auto itConsumer = std::find_if(_consumers.begin(), _consumers.end(), comparison);

            if (itConsumer == _consumers.end())
                break; // no consumer available, wait again

            t_task& currentTask = _plannedTasks.front();

            currentTask._consumer = *itConsumer;
            currentTask._consumer->execute(currentTask._work);

            _runningTasks.push_back(currentTask);
            _plannedTasks.pop_front();
        }
    }
}


