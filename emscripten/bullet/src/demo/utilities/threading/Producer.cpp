
#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD
#   error "exclude this file to build natively or with multi thread support"
#endif

#include "Producer.hpp"

#include <algorithm>
#include <chrono>

Producer::Producer()
{
    const int totalConsumers = 3;

    _consumers.reserve(totalConsumers);
    for (int ii = 0; ii < totalConsumers; ++ii)
        _consumers.push_back(new Consumer(*this));

    _thread = std::thread(&Producer::threadedMethod, this);

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
    while (!_completedTasks.empty())
    {
        t_task* current = nullptr;

        {
            auto lock = _waitOneTask.makeScopedLock();

            current = &_completedTasks.front();
        }

        // must not be locked as it might push another task
        if (current->_oncomplete)
            current->_oncomplete();

        {
            auto lock = _waitOneTask.makeScopedLock();

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
    auto lock = _waitAllTask.makeScopedLock();

    while (!_plannedTasks.empty() || !_runningTasks.empty())
        _waitAllTask.waitUntilNotified(lock);
}

//
//

void Producer::notifyWorkDone(Consumer* consumer)
{
    auto lockNotifier = _waitOneTask.makeScopedLockNotifier();

    // find the task in the "running" list
    auto comparison = [consumer](const t_task& task) { return task._consumer == consumer; };
    auto it = std::find_if(_runningTasks.begin(), _runningTasks.end(), comparison);

    if (it == _runningTasks.end())
        return; // <= this should never fail

    // move the task from "running" to "completed"
    if (it->_oncomplete)
        _completedTasks.push_back(*it);
    _runningTasks.erase(it);

    // wake up potentially waiting main thread
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

        if (!_running)
            break;

        //

        while (!_plannedTasks.empty())
        {
            // look for any available consumer
            Consumer* available = nullptr;
            for (auto* consumer : _consumers)
                if (consumer->isAvailable())
                {
                    available = consumer;
                    break;
                }

            if (!available) // no worker available
                break;

            //

            t_task& currentTask = _plannedTasks.front();

            currentTask._consumer = available;
            available->execute(currentTask._work);

            _runningTasks.push_back(currentTask);
            _plannedTasks.pop_front();
        }
    }
}


