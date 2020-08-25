
#pragma once

#include <functional>

struct t_task
{
    friend class Producer; // <= so the Producer can set _consumer

public:
    typedef std::function<void()> t_work;

public:
    t_work _work = nullptr;
    t_work _oncomplete = nullptr;

private:
    class Consumer* _consumer = nullptr;

public:
    t_task() = default;
    t_task(const t_work& work);
    t_task(const t_work& work, const t_work& oncomplete);
};
