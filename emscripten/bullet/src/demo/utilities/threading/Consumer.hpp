
#pragma once

#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD
#   error "exclude this file to build natively or with multi thread support"
#endif

#include "t_task.hpp"
#include "TaskSynchroniser.hpp"

#include <functional>

#include <thread>

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
    void threadedMethod();
};

