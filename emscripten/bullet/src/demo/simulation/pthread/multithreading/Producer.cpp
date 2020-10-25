
#include "Producer.hpp"

#include <algorithm>
#include <chrono>

namespace multithreading
{

    Producer::Task::Task(const WorkCallback& work)
        : work(work)
    {}

    Producer::Producer(unsigned int totalCores)
    {
        // clamp [1..8]
        const int totalConsumers = std::min(std::max(int(totalCores), 1), 8);

        // launch consumers

        for (int ii = 0; ii < totalConsumers; ++ii)
        {
            Consumer * newConsumer = new Consumer(*this);

            _consumers.push_back(newConsumer);
            _freeConsumers.push_back(newConsumer);
        }

        _running = false; // the producer's thread will set it to true

        // launch producer thread

        _thread = std::thread(&Producer::_threadedMethod, this);

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

    void Producer::push(const WorkCallback& work)
    {
        auto lockNotifier = _waitOneTask.makeScopedLockNotifier();

        // this part is locked and will notify at the end of the scope

        _plannedTasks.push_back(Task(work));
    }

    void Producer::quit()
    {
        if (!_running)
            return;

        // clear the planned task(s) and wake up the running thread
        {
            auto lockNotifier = _waitOneTask.makeScopedLockNotifier();

            // this part is locked and will notify at the end of the scope

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

        _freeConsumers.clear();
        _busyConsumers.clear();
    }

    void Producer::waitUntilAllCompleted()
    {
    #if defined D_WEB_BUILD

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

        // this part is locked

        // make the (main) thread wait for all tasks to be completed
        while (!_plannedTasks.empty() || !_runningTasks.empty())
        {
            // wait -> release the lock for other thread(s)
            _waitAllTask.waitUntilNotified(lock);
        }

    #endif
    }

    //
    //

    void Producer::_notifyWorkDone(Consumer* consumer)
    {
        auto lockNotifier = _waitOneTask.makeScopedLockNotifier();

        // this part is locked and will notify at the end of the scope

        // find the task per consumer in the "running" list
        auto comparison = [consumer](const Task& task) { return task.consumer == consumer; };
        auto itTask = std::find_if(_runningTasks.begin(), _runningTasks.end(), comparison);

        if (itTask == _runningTasks.end())
            return; // <= this should never fail

        // move consumer from free to busy
        Consumer* currConsumer = itTask->consumer;
        _freeConsumers.push_back(currConsumer);
        auto itConsumer = std::find(_busyConsumers.begin(), _busyConsumers.end(), currConsumer);
        if (itConsumer != _busyConsumers.end()) // <= this should never fail
            _busyConsumers.erase(itConsumer);

        _runningTasks.erase(itTask);

        // wake up potentially waiting (main) thread
        if (_plannedTasks.empty() && _runningTasks.empty())
            _waitAllTask.notify();
    }

    void Producer::_threadedMethod()
    {
        auto lock = _waitOneTask.makeScopedLock();

        // this part is locked

        _running = true;

        while (_running)
        {
            // wait -> release the lock for other thread(s)
            _waitOneTask.waitUntilNotified(lock);

            // this part is locked

            if (!_running)
                break; // quit scenario

            while (!_plannedTasks.empty())
            {
                if (_freeConsumers.empty())
                    break; // no consumer available, wait again

                // move consumer from free to busy
                Consumer* currConsumer = _freeConsumers.front();
                _busyConsumers.push_back(currConsumer);
                _freeConsumers.pop_front();

                Task& currentTask = _plannedTasks.front();

                // run task
                currentTask.consumer = currConsumer;
                currConsumer->execute(currentTask.work);

                // move task from planned to running
                _runningTasks.push_back(currentTask);
                _plannedTasks.pop_front();
            }
        }
    }

};
