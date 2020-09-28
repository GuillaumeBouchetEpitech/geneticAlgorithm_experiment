
#pragma once

#include "IProducer.hpp"
#include "ThreadSynchroniser.hpp"

#include <functional>

#include <thread>

namespace multithreading
{
    class Consumer
    {
    private:
        std::thread _thread;
        ThreadSynchroniser _waitProducer;

        bool _running = false;

        WorkCallback _work;

        IProducer& _producer;

    public:
        explicit Consumer(IProducer& producer);
        ~Consumer();

    public:
        void execute(const WorkCallback& work);
        void quit();

    public:
        bool isRunning() const;
        bool isAvailable() const;

    private:
        void _threadedMethod();
    };

};
