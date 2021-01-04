
#pragma once

#include "IProducer.hpp"
#include "Consumer.hpp"
#include "ThreadSynchroniser.hpp"

#include <list>
#include <functional>

#include <thread>

namespace multithreading
{
    class Producer
        : public IProducer
    {
    private:
        struct Task
        {
        public:
            WorkCallback work = nullptr;
            Consumer* consumer = nullptr;

        public:
            Task(const WorkCallback& work);
        };

    private:
        std::thread _thread;
        ThreadSynchroniser _waitOneTask;
        ThreadSynchroniser _waitAllTask;

        bool _running = false;

        std::list<Consumer*> _consumers;
        std::list<Consumer*> _freeConsumers;
        std::list<Consumer*> _busyConsumers;

        std::list<Task> _plannedTasks;
        std::list<Task> _runningTasks;

    public:
        Producer(unsigned int totalCores);
        virtual ~Producer();

    public:
        void push(const WorkCallback& work);
        void quit();
        void waitUntilAllCompleted();

    private:
        virtual void _notifyWorkDone(Consumer* in_consumer) override;
        void _threadedMethod();
    };

};
