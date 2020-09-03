
#pragma once

#include "Consumer.hpp"
#include "TaskSynchroniser.hpp"
#include "t_task.hpp"

#include <list>
#include <vector>
#include <functional>

#include <thread>

namespace multiThreading
{

    class Producer
    {
        friend class Consumer; // <= so the consumers can call _notifyWorkDone()

    private:
        std::thread _thread;
        TaskSynchroniser _waitOneTask;
        TaskSynchroniser _waitAllTask;

        bool _running = true;

        std::vector<Consumer*> _consumers;

        std::list<t_task> _plannedTasks;
        std::list<t_task> _runningTasks;
        std::list<t_task> _completedTasks;

    public:
        Producer(unsigned int totalCores);
        ~Producer();

    public:
        void push(const t_task::t_work& work);
        void push(const t_task::t_work& work, const t_task::t_work& complete);
        void update();
        void quit();
        void waitUntilAllCompleted();

    private:
        void _notifyWorkDone(Consumer* in_consumer);
        void _threadedMethod();
    };

};
