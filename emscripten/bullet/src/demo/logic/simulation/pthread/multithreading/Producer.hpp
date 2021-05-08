
#pragma once

#include "internals/IProducer.hpp"
#include "internals/Consumer.hpp"
#include "internals/ThreadSynchroniser.hpp"

#include <list>
#include <functional>
#include <memory>
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
            std::shared_ptr<Consumer> consumer = nullptr;

        public:
            Task(const WorkCallback& work);
        };

    private:
        std::thread _thread;
        ThreadSynchroniser _waitOneTask;
        ThreadSynchroniser _waitAllTask;

        bool _running = false;

        std::list<std::shared_ptr<Consumer>> _consumers;
        std::list<std::shared_ptr<Consumer>> _freeConsumers;
        std::list<std::shared_ptr<Consumer>> _busyConsumers;

        std::list<Task> _plannedTasks;
        std::list<Task> _runningTasks;

    public:
        Producer() = default;
        virtual ~Producer();

    public:
        void initialise(unsigned int totalCores);
        void push(const WorkCallback& work);
        void quit();
        void waitUntilAllCompleted();
        bool allCompleted() const;

    private:
        virtual void _notifyWorkDone(Consumer* consumer) override;
        void _threadedMethod();
    };

};
