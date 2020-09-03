
#pragma once

#include "t_task.hpp"
#include "TaskSynchroniser.hpp"

#include <functional>

#include <thread>

namespace multiThreading
{

    class Consumer
    {
    private:
        std::thread _thread;
        TaskSynchroniser _waitProducer;

        bool _running = false;

        t_task::t_work _work;

        class Producer& _producer;

    public:
        Consumer(class Producer& producer);
        ~Consumer();

    public:
        void execute(const t_task::t_work& work);
        void quit();

    public:
        bool isRunning() const;
        bool isAvailable() const;

    private:
        void _threadedMethod();
    };

};
